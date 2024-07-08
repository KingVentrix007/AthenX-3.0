import re

def parse_map_file(map_file):
    functions = {}

    with open(map_file, 'r') as f:
        lines = f.readlines()
    
    in_text_section = False
    for line in lines:
        # Check for the start of the .text section
        if line.startswith('.text'):
            in_text_section = True
            continue
        
        # End parsing when another section starts
        if in_text_section and re.match(r'^\.\w+', line):
            break
        
        # Parse the function addresses and names in the .text section
        if in_text_section:
            match = re.match(r'^\s+0x([0-9a-fA-F]+)\s+([^\s]+)', line)
            if match:
                address = int(match.group(1), 16)
                function_name = match.group(2)
                functions[address] = function_name

    return functions

def find_function_by_address(address, functions):
    # Find the closest address which is less than or equal to the given address
    closest_address = None
    for func_address in functions:
        if func_address <= address and (closest_address is None or func_address > closest_address):
            closest_address = func_address
    
    if closest_address is not None:
        return functions[closest_address]
    return None

# Example usage
map_file = 'AthenX.bin.map'
functions = parse_map_file(map_file)

address = 0x10134c  # Example address
function_name = find_function_by_address(address, functions)
if function_name:
    print(f'Address 0x{address:x} corresponds to function: {function_name}')
else:
    print(f'No function found for address 0x{address:x}')

# 0x10134c
# 0x156ef6
