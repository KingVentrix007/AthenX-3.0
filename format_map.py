import re

def parse_map_file(map_file, output_file):
    functions = {}

    with open(map_file, 'r') as f:
        lines = f.readlines()

    in_text_section = False
    current_file_path = ""
    for line in lines:
        # Check for the start of the .text section (with a leading space)
        if line.startswith(' .text'):
            in_text_section = True
            # Split the line by spaces and take the last part
            parts = line.strip().split()
            if parts[-1].endswith('.o'):
                current_file_path = parts[-1][:-2] + ".c"
                print("Found .o file path:", current_file_path)
            else:
                print(f"Warning: No .o file path found in line: {line.strip()}")
                current_file_path = ""
            continue
        
        # End parsing when another section starts
        if in_text_section and re.match(r'^\.\w+', line):
            in_text_section = False
            continue
        
        # Parse the function addresses and names in the .text section
        if in_text_section:
            match = re.match(r'^\s+0x([0-9a-fA-F]+)\s+([^\s]+)', line)
            if match:
                address = match.group(1)
                function_name = match.group(2)
                if current_file_path:
                    if(address == "0000000000000000"):
                        current_file_path = "INVALID"
                        function_name = "INVALID"
                    functions[address] = f"{address}:{current_file_path}:{function_name}"
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
    parse_map_file(map_file, output_file)

