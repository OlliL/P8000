# WEGA types to fixed width types in the code of the files given (not in comments and strings): int/short -> int16_t, unsigned -> uint16_t, long -> int32_t, unsigned long -> uint32_t. usage: z8types.py file... (changes the files in place; see HANDOFF, Port rules, Loaders)
import re,sys
# WEGA types -> fixed width: int/short -> int16_t, unsigned [int] -> uint16_t, long -> int32_t, unsigned long -> uint32_t (code only, not comments/strings)
tok=re.compile(r'(/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\')',re.S)
def conv(code):
    code=re.sub(r'\bunsigned\s+long(\s+int)?\b','uint32_t',code)
    code=re.sub(r'\bunsigned\s+(?:short|int)\b','uint16_t',code)
    code=re.sub(r'\bunsigned\b(?!\s*(?:char|long))','uint16_t',code)
    code=re.sub(r'\blong(\s+int)?\b','int32_t',code)
    code=re.sub(r'\bshort(\s+int)?\b','int16_t',code)
    code=re.sub(r'\bint\b(?!\s*(?:main\b|argc\b))','int16_t',code)
    return code
for f in sys.argv[1:]:
    s=open(f).read()
    parts=tok.split(s)
    for i in range(0,len(parts),2): parts[i]=conv(parts[i])
    s=''.join(parts)
    s=s.replace('int16_t main(int16_t argc','int main(int argc')
    open(f,'w').write(s)
