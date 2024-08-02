import re
import os

def parse_function_params(file_path, function_name):
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Regex to find function definitions (assuming they are at the beginning of a line)
    func_regex = re.compile(rf'^\s*\w[\w\s\*]*\b{re.escape(function_name)}\s*\(([^)]*)\)', re.MULTILINE)
    match = func_regex.search(content)
    if match:
        params = match.group(1).strip()
        if params:
            return params
        else:
            return "void"
    return None
kill_pattern = r'.*(\*fill\*|\*\(\.rodata\)|\.rodata).*'
def parse_map_file(map_file, output_file, source_dir):
    functions = {}

    with open(map_file, 'r') as f:
        lines = f.readlines()

    in_text_section = False
    current_file_path = ""
    for line in lines:
        # print(line)
        # Check for the start of the .text section (with a leading space)
        if line.startswith(' .text') or line.startswith('.text'):
            
            in_text_section = True
            # Split the line by spaces and take the last part
            parts = line.strip().split()
            if parts[-1].endswith('.o'):
                current_file_path = parts[-1][:-2] + ".c"
            else:
                print(f"Warning: No .o file path found in line: {line.strip()}")
                current_file_path = ""
            continue
        
        # End parsing when another section starts
        if in_text_section and re.match(r'^\.\w+', line):
            in_text_section = False
            continue
        kill_match = re.match(kill_pattern, line)
        if(in_text_section and kill_match):
            in_text_section = False
            continue
        # Parse the function addresses and names in the .text section
        if in_text_section:
            print(line)
            match = re.match(r'^\s+0x([0-9a-fA-F]+)\s+([^\s]+)', line)
            if match:
                address = match.group(1)
                function_name = match.group(2)
                # if function_name == ".":
                    # print("Line ", line)
                if current_file_path:
                    if address == "0000000000000000" or function_name == "__kernel_text_section_end":
                        current_file_path = "INVALID"
                        function_name = "INVALID"
                    else:
                        c_file_path = os.path.join(source_dir, current_file_path)
                        c_file_path = c_file_path[len("./obj"):]
                        c_file_path = "./" + c_file_path

                        if os.path.exists(c_file_path):
                            params = parse_function_params(c_file_path, function_name)
                            if params:
                                functions[address] = f"{address}:{current_file_path}:{function_name}:{params}"
                            else:
                                print(f"Function {function_name} not found in {c_file_path}, skipping.")
                        else:
                            use_path = c_file_path.removesuffix(".c") + ".s"
                            use_path = use_path[len("./obj"):]
                            use_path = "./" + use_path
                            functions[address] = f"{address}:{use_path}:{function_name}:none"
                else:
                    functions[address] = f"{address}::{function_name}"  # No .o path found, use "::"

    # Write the functions to the output file
    with open(output_file, 'w') as f:
        for path_function in functions.values():
            f.write(f'{path_function}\n')

    print(f"Processed {len(functions)} functions. Output written to {output_file}")

# Example usage
if __name__ == "__main__":
    map_file = 'AthenX.bin.map'
    output_file = './sysroot/debug/debug.map'
    source_dir = './'  # Add your source directory here
    parse_map_file(map_file, output_file, source_dir)
