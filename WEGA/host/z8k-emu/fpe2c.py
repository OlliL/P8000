#!/usr/bin/env python3
"""fpe2c.py - translate the WEGA kernel floating point emulator (fpe.s, PLZ/ASM)
into C, one Z8000 instruction per C statement.

    python3 fpe2c.py ../../src/uts/conf/fpe.s > fpe_gen.c

The output is the body of one C function that is included by epu.c.  It
works on a second Z8K context (k, the "kernel") with the same instruction
semantics as the emulator core; epu.c provides the macros and helpers.
Calls push a return number on the kernel stack and RET pops it, so code that
moves return addresses around (popl rr0,@rr14 ... jp @rr4) behaves as in the
original.  Tables (array [* long]) hold label numbers instead of addresses.
"""
import re
import sys

REG = {f'r{i}': i for i in range(16)}
REG['sp'] = 15
BREG = {f'rh{i}': i for i in range(8)}
BREG.update({f'rl{i}': i + 8 for i in range(8)})
LREG = {f'rr{i}': i for i in range(0, 16, 2)}
QREG = {f'rq{i}': i for i in range(0, 16, 4)}
CC = {'f': 0, 'lt': 1, 'le': 2, 'ule': 3, 'ov': 4, 'pe': 4, 'mi': 5, 'z': 6,
      'eq': 6, 'c': 7, 'ult': 7, 't': 8, 'ge': 9, 'gt': 10, 'ugt': 11,
      'nov': 12, 'po': 12, 'pl': 13, 'nz': 14, 'ne': 14, 'nc': 15, 'uge': 15}
FLAG = {'c': 0x80, 'z': 0x40, 's': 0x20, 'p': 0x10, 'v': 0x10}
# kernel routines outside fpe.s (mch.s); epu.c implements them natively
EXTERN = {'gettext', 'getmem', 'putmem'}

consts = {}


def die(line, msg):
    sys.exit(f'fpe.s:{line}: {msg}')


def expr(s, line):
    s = s.strip()
    total = 0
    for sign, tok in re.findall(r'([+-]?)\s*([%\w]+)', s):
        if tok.startswith('%'):
            v = int(tok[1:], 16)
        elif tok.isdigit():
            v = int(tok)
        elif tok in consts:
            v = consts[tok]
        else:
            die(line, f'bad expression {s!r}')
        total += -v if sign == '-' else v
    return total


def opd(s, line):
    """classify an operand: (kind, value...)"""
    s = s.strip()
    if s in REG:
        return ('R', REG[s])
    if s in BREG:
        return ('RB', BREG[s])
    if s in LREG:
        return ('RR', LREG[s])
    if s in QREG:
        return ('RQ', QREG[s])
    if s == 'flags':
        return ('FL',)
    if s.startswith('#'):
        return ('IM', expr(s[1:], line))
    m = re.fullmatch(r'@(rr\d+)', s)
    if m:
        return ('IR', LREG[m.group(1)])
    m = re.fullmatch(r'(rr\d+)\(#(.*)\)', s)
    if m:
        return ('BA', LREG[m.group(1)], expr(m.group(2), line) & 0xFFFF)
    m = re.fullmatch(r'(rr\d+)\((r\d+)\)', s)
    if m:
        return ('BX', LREG[m.group(1)], REG[m.group(2)])
    m = re.fullmatch(r'(\w+)\((r\d+)\)', s)
    if m:
        return ('X', m.group(1), REG[m.group(2)])
    if re.fullmatch(r'\w+', s):
        return ('L', s)
    die(line, f'bad operand {s!r}')


def addr(o):
    if o[0] == 'IR':
        return f'ir_ad(k, {o[1]})'
    if o[0] == 'BA':
        return f'BA(k, {o[1]}, 0x{o[2]:04x})'
    if o[0] == 'BX':
        return f'BX(k, {o[1]}, {o[2]})'
    return None


SZ = {'R': 2, 'RB': 1, 'RR': 4}


def src(o, sz):
    """C expression for a source operand of size sz"""
    if o[0] in ('R', 'RB', 'RR'):
        return f'rreg(k, {o[1]}, {sz})'
    if o[0] == 'IM':
        return f'0x{o[1] & {1: 0xFF, 2: 0xFFFF, 4: 0xFFFFFFFF}[sz]:x}'
    a = addr(o)
    if a:
        return f'mrd(k, {a}, {sz})'
    raise ValueError(o)


def dst(o, sz, v):
    if o[0] in ('R', 'RB', 'RR'):
        return f'wreg(k, {o[1]}, {sz}, {v});'
    a = addr(o)
    if a:
        return f'mwr(k, {a}, {sz}, {v});'
    raise ValueError(o)


ALU = {'add': 'O_ADD', 'sub': 'O_SUB', 'adc': 'O_ADC', 'sbc': 'O_SBC',
       'and': 'O_AND', 'or': 'O_OR', 'xor': 'O_XOR', 'cp': 'O_CP'}
ROT = {'rl': (1, 0), 'rr': (0, 0), 'rlc': (1, 1), 'rrc': (0, 1)}


class Gen:
    def __init__(self):
        self.out = []
        self.labels = []        # in order of definition
        self.used = set()
        self.nret = 0
        self.tables = {}
        self.cur = None         # current procedure

    def emit(self, s):
        self.out.append(s)

    def lab(self, name):
        if name in self.labels:
            die(0, f'duplicate label {name}')
        self.labels.append(name)
        self.emit(f'l_{name}:;')

    def target(self, name, line):
        if name in EXTERN:
            die(line, f'jump to {name} needs special handling')
        self.used.add((name, line))
        return f'l_{name}'

    def call(self, how):
        """push a return number, then `how`, then define the return label"""
        self.nret += 1
        n = self.nret
        return [f'PUSHRET(R_{n}); {how}', f'r_{n}:;']

    def insn(self, line, mn, ops, text):
        o = [opd(x, line) for x in ops]
        k = [x[0] for x in o]
        c = []
        e = c.append

        # --- control flow
        if mn in ('jr', 'jp', 'jpr'):
            if len(o) == 2:
                cond, t = ops[0].strip(), o[1]
                if cond not in CC:
                    die(line, f'bad condition {cond}')
                pre = f'if (cond(k, {CC[cond]})) '
            else:
                pre, t = '', o[0]
            if t[0] == 'IR':
                e(f'{pre}JPIND({t[1]});')
            elif t[1] in EXTERN:
                e(f'{pre}{{ NATIVE({t[1]}); RET; }}')
            else:
                e(f'{pre}JMP({self.target(t[1], line)});')
            return c
        if mn in ('call', 'callr', 'calr'):
            t = o[0]
            if t[0] == 'IR':
                return self.call(f'JPIND({t[1]});')
            if t[1] in EXTERN:
                return [f'NATIVE({t[1]});']
            return self.call(f'JMP({self.target(t[1], line)});')
        if mn == 'ret':
            if o:
                return [f'if (cond(k, {CC[ops[0].strip()]})) RET;']
            return ['RET;']
        if mn == 'djnz':
            return [f'if (--k->r[{o[0][1]}]) JMP({self.target(o[1][1], line)});']

        # --- loads
        if mn in ('ld', 'ldb', 'ldl', 'ldk'):
            sz = {'ld': 2, 'ldk': 2, 'ldb': 1, 'ldl': 4}[mn]
            if k[1] == 'X':
                if mn != 'ldl':
                    die(line, 'indexed table access must be ldl')
                return [f'wreg(k, {o[0][1]}, 4, TAB(T_{o[1][1]}, {o[1][2]}));']
            return [dst(o[0], sz, src(o[1], sz))]
        if mn == 'lda':
            return [f'LDA({o[0][1]}, {addr(o[1])});']
        if mn in ('ex', 'exb'):
            sz = 2 if mn == 'ex' else 1
            return [f'{{ uint32_t t_ = rreg(k, {o[0][1]}, {sz}); '
                    f'wreg(k, {o[0][1]}, {sz}, rreg(k, {o[1][1]}, {sz})); '
                    f'wreg(k, {o[1][1]}, {sz}, t_); }}']
        if mn == 'ldm':
            if k[0] == 'R':
                return [f'LDM_LOAD({o[0][1]}, {addr(o[1])}, {o[2][1]});']
            return [f'LDM_STORE({addr(o[0])}, {o[1][1]}, {o[2][1]});']
        if mn == 'ldir':
            return [f'LDIR({o[0][1]}, {o[1][1]}, {o[2][1]});']
        if mn in ('push', 'pushl'):
            sz = 4 if mn == 'pushl' else 2
            return [f'PUSH({o[0][1]}, {sz}, {src(o[1], sz)});']
        if mn in ('pop', 'popl'):
            sz = 4 if mn == 'popl' else 2
            return [f'{{ uint32_t t_ = POP({o[1][1]}, {sz}); {dst(o[0], sz, "t_")} }}']

        # --- arithmetic / logic
        base = mn
        sz = 2
        if mn.endswith('b') and mn[:-1] in ALU:
            base, sz = mn[:-1], 1
        elif mn.endswith('l') and mn[:-1] in ALU:
            base, sz = mn[:-1], 4
        if base in ALU:
            return [f'alu(k, {ALU[base]}, {sz}, {o[0][1]}, {src(o[1], sz)});']
        if mn in ('com', 'comb'):
            sz = 2 if mn == 'com' else 1
            return [f'COM({o[0][1]}, {sz});']
        if mn == 'neg':
            return [f'k->r[{o[0][1]}] = addsub(k, 0, k->r[{o[0][1]}], 0, 2, 1);']
        if mn in ('inc', 'dec', 'incb', 'decb'):
            sz = 1 if mn.endswith('b') else 2
            n = o[1][1] if len(o) > 1 else 1
            a = addr(o[0])
            if a:
                return [f'INCDEC_M({a}, {sz}, {n}, {int(mn.startswith("dec"))});']
            return [f'INCDEC_R({o[0][1]}, {sz}, {n}, {int(mn.startswith("dec"))});']
        if mn in ('test', 'testl', 'testb'):
            sz = {'test': 2, 'testl': 4, 'testb': 1}[mn]
            return [f'logf_(k, rreg(k, {o[0][1]}, {sz}), {sz});']
        if mn == 'exts':
            return [f'k->r[{o[0][1]}] = (k->r[{o[0][1] + 1}] & 0x8000) ? 0xFFFF : 0;']
        if mn == 'extsl':
            return [f'W32(k, {o[0][1]}, (R32(k, {o[0][1] + 2}) & 0x80000000u) ? 0xFFFFFFFFu : 0);']
        if mn == 'multl':
            return [f'MULTL({o[0][1]}, {src(o[1], 4)});']
        if mn in ('bit', 'bitb', 'set', 'setb', 'res', 'resb'):
            sz = 1 if mn.endswith('b') else 2
            kind = {'bit': 'BIT', 'set': 'SET', 'res': 'RES'}[mn.rstrip('b')]
            b = o[1][1]
            if b >= 8 * sz:
                die(line, 'bit number out of range')
            a = addr(o[0])
            if a:
                return [f'{kind}_M({a}, {sz}, {b});']
            return [f'{kind}_R({o[0][1]}, {sz}, {b});']
        m = re.fullmatch(r'(rl|rr|rlc|rrc)(b?)', mn)
        if m:
            left, thru = ROT[m.group(1)]
            sz = 1 if m.group(2) else 2
            n = o[1][1] if len(o) > 1 else 1
            if n not in (1, 2):
                die(line, 'rotate count must be 1 or 2')
            return [f'wreg(k, {o[0][1]}, {sz}, rotate(k, rreg(k, {o[0][1]}, {sz}), {sz}, {left}, {n}, {thru}));']
        if mn in ('sll', 'srl', 'sllb', 'srlb', 'sla', 'sra', 'slab', 'srab'):
            sz = 1 if mn.endswith('b') else 2
            n = o[1][1] if len(o) > 1 else 1
            cnt = n if mn.startswith('sl') else -n
            arith = int(mn[2] == 'a')
            return [f'wreg(k, {o[0][1]}, {sz}, shiftn(k, rreg(k, {o[0][1]}, {sz}), {sz}, {cnt}, {arith}));']
        if mn in ('rldb', 'rrdb'):
            return [f'{mn.upper()}({o[0][1]}, {o[1][1]});']
        if mn in ('tcc', 'tccb'):
            sz = 2 if mn == 'tcc' else 1
            return [f'if (cond(k, {CC[ops[0].strip()]})) wreg(k, {o[1][1]}, {sz}, rreg(k, {o[1][1]}, {sz}) | 1);']
        if mn == 'ldctlb':
            if k[0] == 'FL':
                return [f'k->fl = R8(k, {o[1][1]}) & 0xFC;']
            return [f'W8(k, {o[0][1]}, k->fl);']
        if mn in ('setflg', 'resflg', 'comflg'):
            mask = 0
            for f in ops:
                mask |= FLAG[f.strip()]
            op = {'setflg': '|=', 'resflg': '&= ~', 'comflg': '^='}[mn]
            return [f'k->fl {op}0x{mask:02x};']
        die(line, f'unknown instruction {text!r}')

    def run(self, path):
        text = open(path, encoding='latin-1').read()
        text = re.sub(r'!.*?!', lambda m: '\n' * m.group(0).count('\n'), text, flags=re.S)
        lines = text.split('\n')
        i = 0
        mode = None
        prev_flow = False
        while i < len(lines):
            line = i + 1
            s = lines[i].strip()
            i += 1
            if not s:
                continue
            if s in ('internal', 'global', 'external', 'constant', '$segmented', 'entry'):
                mode = s if s == 'constant' else None
                continue
            m = re.fullmatch(r'(\w+)\s+module', s)
            if m:
                continue
            if mode == 'constant':
                m = re.fullmatch(r'(\w+)\s*:=\s*(.+)', s)
                if m:
                    if m.group(2).strip() in REG:
                        consts[m.group(1)] = None
                        REG[m.group(1)] = REG[m.group(2).strip()]
                    else:
                        consts[m.group(1)] = expr(m.group(2), line)
                    continue
                mode = None
            m = re.match(r'(\w+)\s+array\s*\[\*\s*long\]\s*:=\s*\[(.*)', s)
            if m:
                name, body = m.group(1), m.group(2)
                while ']' not in body:
                    body += ' ' + lines[i].strip()
                    i += 1
                body = body[:body.index(']')]
                self.tables[name] = [x.strip() for x in body.split(',')]
                for x in self.tables[name]:
                    self.used.add((x, line))
                continue
            m = re.fullmatch(r'(\w+)\s+procedure', s)
            if m:
                if prev_flow:
                    self.emit(f'\t/* falls through into {m.group(1)} */')
                    sys.stderr.write(f'fpe.s:{line}: note: fall-through into {m.group(1)}\n')
                self.cur = m.group(1)
                self.emit(f'\n\t/* {m.group(1)} procedure */')
                self.lab(m.group(1))
                continue
            m = re.fullmatch(r'end\s+(\w+)', s)
            if m:
                continue
            m = re.fullmatch(r'(\w+):', s)
            if m:
                self.lab(m.group(1))
                continue
            m = re.fullmatch(r'([a-z]+)\s*(.*)', s)
            if not m:
                die(line, f'cannot parse {s!r}')
            mn, rest = m.group(1), m.group(2)
            ops = [x for x in rest.split(',')] if rest else []
            stmts = self.insn(line, mn, ops, s)
            asm = re.sub(r'\s+', ' ', s)
            self.emit(f'\tTR({line}); {stmts[0]}\t/* {asm} */')
            for x in stmts[1:]:
                self.emit(f'{x}')
            prev_flow = not ((mn == 'ret' and not ops) or (mn in ('jr', 'jp', 'jpr') and len(ops) == 1))
        for name, line in self.used:
            if name not in self.labels:
                die(line, f'undefined label {name}')

    def write(self, f):
        p = f.write
        p('/* fpe_gen.c - generated by fpe2c.py from src/uts/conf/fpe.s; do not edit.\n'
          ' * Included by epu.c.  One statement per instruction; the comment holds\n'
          ' * the original source line. */\n\n')
        p('enum {\n\tL_none_,\n')
        for name in self.labels:
            p(f'\tL_{name},\n')
        p('\tR_END_ = 0x1000,\n')
        for n in range(1, self.nret + 1):
            p(f'\tR_{n},\n')
        p('};\n\n')
        for name, ents in self.tables.items():
            p(f'static const uint32_t T_{name}[{len(ents)}] = {{\n')
            for j in range(0, len(ents), 4):
                p('\t' + ' '.join(f'L_{x},' for x in ents[j:j + 4]) + '\n')
            p('};\n')
        p('\nstatic int fpe_run(Z8K *k, Z8K *u)\n{\n\tuint32_t tgt_;\n\n')
        p('\tPUSHRET(R_END_);\n\tJMP(l__fpepu);\n')
        for s in self.out:
            p(s + '\n')
        p('\n\tFELL_OFF;\n\ndispatch:\n\tswitch (tgt_) {\n')
        for name in self.labels:
            p(f'\tcase L_{name}: goto l_{name};\n')
        for n in range(1, self.nret + 1):
            p(f'\tcase R_{n}: goto r_{n};\n')
        p('\tcase R_END_: return 0;\n\t}\n\tBAD_TARGET;\n\treturn -1;\n}\n')


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: fpe2c.py fpe.s > fpe_gen.c')
    g = Gen()
    g.run(sys.argv[1])
    g.write(sys.stdout)


main()
