import re
import os

def parse_function_params(file_path, function_name):
    with open(file_path, 'r') as f:
        content = f.read()
    
    func_regex = re.compile(rf'^\s*\w[\w\s\*]*\b{re.escape(function_name)}\s*\(([^)]*)\)', re.MULTILINE)
    match = func_regex.search(content)
    if match:
        params = match.group(1).strip()
        if params:
            return params
        else:
            return "void"
    return None

def parse_structs(file_path):
    structs = {}
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Regex to find struct definitions (basic example)
    struct_regex = re.compile(r'struct\s+(\w+)\s*\{([^}]*)\}', re.DOTALL)
    matches = struct_regex.finditer(content)
    
    for match in matches:
        struct_name = match.group(1)
        struct_body = match.group(2).strip()
        structs[struct_name] = struct_body
    
    return structs

def parse_map_file(map_file, output_file, source_dir):
    functions = {}
    structs = {}

    largest_address = 0

    with open(map_file, 'r') as f:
        lines = f.readlines()

    in_text_section = False
    current_file_path = ""
    for line in lines:
        if line.startswith(' .text'):
            in_text_section = True
            parts = line.strip().split()
            if parts[-1].endswith('.o'):
                current_file_path = parts[-1][:-2] + ".c"
            else:
                print(f"Warning: No .o file path found in line: {line.strip()}")
                current_file_path = ""
            continue
        
        if in_text_section and re.match(r'^\.\w+', line):
            in_text_section = False
            continue
        
        if in_text_section:
            match = re.match(r'^\s+0x([0-9a-fA-F]+)\s+([^\s]+)', line)
            if match:
                address = int(match.group(1), 16)
                function_name = match.group(2)
                if address > largest_address:
                    largest_address = address
                
                if current_file_path:
                    if address == 0x0000000000000000 or function_name == "__kernel_text_section_end":
                        current_file_path = "INVALID"
                        function_name = "INVALID"
                    else:
                        c_file_path = os.path.join(source_dir, current_file_path)
                        c_file_path = c_file_path[len("./obj"):]
                        c_file_path = "./"+c_file_path

                        if os.path.exists(c_file_path):
                            print(f"Looking for file {c_file_path}")
                            params = parse_function_params(c_file_path, function_name)
                            if params:
                                functions[address] = f"{address:016x}:{current_file_path}:{function_name}:{params}"
                            else:
                                functions[address] = f"{address:016x}:{current_file_path}:{function_name}:none"
                        else:
                            use_path = c_file_path.removesuffix(".c")+".s"
                            use_path = use_path[len("./obj"):]
                            use_path = "./"+use_path
                            functions[address] = f"{address:016x}:{use_path}:{function_name}:none"
                else:
                    functions[address] = f"{address:016x}::{function_name}"
    
    # Create output with headers
    with open(output_file, 'w') as f:
        f.write(".map_debug_header\n")
        f.write(f"version: 1.0\n")
        f.write(f"largest_address: {largest_address:016x}\n")
        f.write("sections: .text\n")
        f.write("\n.map_debug_addr\n")
        
        for addr, func in sorted(functions.items()):
            f.write(f'{func}\n')
        
        f.write("\n.map_struct_header\n")
        
        # Find and write structs
        for file in os.listdir(source_dir):
            if file.endswith(".c"):
                structs.update(parse_structs(os.path.join(source_dir, file)))
        
        for struct_name, struct_body in structs.items():
            f.write(f'struct {struct_name} {{\n{struct_body}\n}}\n')
    
    print(f"Processed {len(functions)} functions and structs. Output written to {output_file}")

# Example usage
if __name__ == "__main__":
    map_file = 'AthenX.bin.map'
    output_file = './test/map_out.map'
    source_dir = './'  # Add your source directory here
    parse_map_file(map_file, output_file, source_dir)
