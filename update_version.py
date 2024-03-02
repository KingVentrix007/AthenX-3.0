# update_version.py

def update_version():
    with open('./kernel/info.c', 'r') as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        if 'VERSION_COMPILE' in line:
            current_compile_version = int(line.split()[-1])
            new_compile_version = current_compile_version + 1
            lines[i] = f'#define VERSION_COMPILE {new_compile_version}\n'
            break

    with open('./kernel/info.c', 'w') as file:
        file.writelines(lines)

    print(f'Updated VERSION_COMPILE to {new_compile_version}')

if __name__ == "__main__":
    update_version()
