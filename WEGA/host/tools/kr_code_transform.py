# Do our best in transforming K&R C-Code to modern ANSI Code - needs manual editing afterwards

import re
import os
import sys

def transform_kr_to_ansi(content):
    # Pattern to find: ReturnType \n FunctionName(args) \n ParameterBlock {
    kr_pattern = re.compile(
        r'^([a-zA-Z_][a-zA-Z0-9_\s\*]*?)\n\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*([^\{]*?)\{',
        re.MULTILINE
    )

    def replace_func(match):
        ret_type = match.group(1).strip()
        func_name = match.group(2)
        args_str = match.group(3).strip()
        params_block = match.group(4)
        full_match_text = match.group(0)
        
        if args_str and any(' ' in a.strip() for a in args_str.split(',')):
            return full_match_text

        if any(op in ret_type for op in ['=', 'if ', 'for ', 'while ']):
            return full_match_text

        args_list = [a.strip() for a in args_str.split(',')] if args_str else []
        
        param_types = {}
        statements = params_block.split(';')
        for stmt in statements:
            stmt = stmt.strip()
            if not stmt:
                continue
            
            stmt = re.sub(r'^register\s+', '', stmt)
            
            decl_match = re.match(r'^([a-zA-Z_][a-zA-Z0-9_\s\*]+?)\s+([a-zA-Z_][a-zA-Z0-9_,\s\*]+)$', stmt)
            if decl_match:
                type_part = decl_match.group(1).strip()
                vars_part = decl_match.group(2).strip()
                
                for var in vars_part.split(','):
                    var_name = var.strip()
                    if ' ' in var_name:
                        parts = var_name.rsplit(' ', 1)
                        actual_type = (type_part + ' ' + parts[0]).strip()
                        actual_name = parts[1].strip('* ')
                        if '*' in var_name:
                            actual_type += ' *'
                        param_types[actual_name] = actual_type
                    else:
                        param_types[var_name] = type_part

        if args_list:
            ansi_args = []
            for arg in args_list:
                clean_arg = arg.strip('* ')
                if clean_arg in param_types:
                    ansi_args.append(f"{param_types[clean_arg]} {arg}")
                else:
                    # CONSERVATIVE: If type not found, do NOT transform the function.
                    # This avoids breaking the build with wrong 'int' defaults.
                    return full_match_text
            
            signature = f"{ret_type} {func_name}({', '.join(ansi_args)})"
            return f"{signature}\n{{"
        else:
            signature = f"{ret_type} {func_name}()"
            return f"{signature}\n{{"

    return kr_pattern.sub(replace_func, content)

def process_file(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()
    transformed = transform_kr_to_ansi(content)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(transformed)

if __name__ == '__main__':
    for path in sys.argv[1:]:
        process_file(path)
