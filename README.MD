<p align="center">
  <b><font size="20">AthenX-3.0</font></b>
</p>
Copyright 2024 Tristan Kuhn(KingVentrix007)

A **new** and **improved** version of AthenX-2.0. With realistic goals,better code,and more features.

## About

AthenX-3.0 is the successor of AthenX-2.0. It aims to support more features then AthenX-2.0 as well as use the lessons learned from the previous versions of AthenX to be faster and cleaner.

## Getting Started

### Prerequisites

- Unix based operating system

  - WSL(Ubuntu)
  - Linux
  - Ubuntu
- grub

```bash
sudo apt install grub-common
```

```bash
sudo apt install grub-pc-bin
```

- nasm

```bash
sudo apt install nasm
```

- gcc

```bash
sudo apt install gcc
```

- qemu

```bash
sudo apt install qemu-system
```

- xorriso

```bash
sudo apt install xorriso
```

- fdisk

```bash
sudo apt install fdisk
  ```

### Installation

1. Clone AthenX-3.0

```bash
git clone https://github.com/KingVentrix007/AthenX-2.0.git
```

2. Enter freshly cloned repository with wsl or other Unix system and run:

```bash
bash ./build-cross-compiler.sh
```

3. Add then add /cross/bin to .bashrc or .bash_profile

```bash
export PATH=$PATH:/your/new/path
```

4. Compile and run

```bash
make run
```

## Roadmap

- [X] Boot with Grub
- [X] VESA/VBE display
- [ ] Keyboard support
  - [X] PS/2(legacy) Keyboard driver
  - [ ] USB keyboard support
- [ ] HDD/SSD driver
  - [X] ATA/ATAPI driver
  - [ ] AHCI driver
  - [ ] USB storage driver
  - [ ] NVME driver
- [ ] File system support
  - [X] FAT12/16/32
    - [X] Read/Write/Format
  - [ ] ext2/3/4
  - [ ] NTFS
  - [ ] ISO 9660
- [ ] Memory support
  - [X] Physical memory manager
  - [X] Virtual memory manager
  - [X] Pageing and mapping support
  - [ ] Page swap with Disk
  - [ ] Variable page size support (Current is 4096)
- [ ] Multiprocessing / Threading support
  - [X] Context switching
  - [ ] Scheduling support
    - [X] Round-robin
    - [ ] Priority or unfair (I can't decide if it is really necessary)
- [ ] Program loading
  - [X] Load elf file (Incredibly limited, requires Scheduler to be disabled)
  - [ ] Load mulitple files at once
- [ ] Debugging system
  - [X] Second screen with some debug information(Press F2, not much there)
  - [X] com1 port, location logging
  - [ ] Dwarf support and fully featured debug terminal
- [ ] ACPI support
  - [X] Power on/off (limited)
  - [ ] Reboot
  - [ ] Power button support
- [ ] UI
  - [X] Basic shell (Virtually no commands)
  - [X] Screen scrolling (limited to the size of the screen)
  - [ ] Unlimited scroll back
- [X] PCI support (Basic bus scan)
- [ ] PCI Express support
- [ ] Network drivers
  - [ ] Ethernet cards
    - [ ] Intel Ethernet i217
    - [ ] RTL8139
    - [ ] RTL8169
    - [ ] Intel 8254x
  - [ ] Wireless Network driver

- [ ] libc
- [ ] Other
  - [X] stdin/stdout/stderr
  - [ ] Special files
    - [X] /dev/null
    - [ ] /dev/fmb
    - [ ] /dev/random
    - [X] /dev/tty(Pretty much just printf)
    - [ ] /dev/ Devices generated at runtime(Currently hardcoded)

## Contributions
You can make pull requests, but I do have limited time, so your code might take time to be implemented
## License

1. This `README.md` is distributed under the MIT License. See `MIT.txt` for more information.
2. This operating system is protected by the `The MIT License`, if you use, redistribute, re-upload or share this code you have to comply with `MIT` and any other Licenses specified within the source code files. I would also appreciate it if credit would be given to me if you use this project.
3. Licenses specific to files will be stored in the `OtherLicenses` folder
4. If I have missed any copyright licenses, **Please tell me** and I will try to fix it as soon as possible
5. Some code, mainly older parts of libk, are from AthenX-2.0, meaning that that part of the code can be treated as `GNU General Public License v3.0`
<!-- up to commit 0042b667bb0f27fa8f3405d608e77dbf3c3ec78 on Tue Mar 12 06:15:09 2024 +0200 AthenX-3.0 was not publicly available   -->
## Libraries used

- [Ultraembedded/fat_io_lib](https://github.com/ultraembedded/fat_io_lib): FAT file system library
- [mpaland/printf](https://github.com/mpaland/printf/tree/master): printf(and other functions) library
- [benhoyt/inih](https://github.com/benhoyt/inih/tree/master): INI file parser library
- [gcc-mirror/gcc](https://github.com/gcc-mirror/gcc/tree/master/libiberty): Implementations of compliant libc code
-[hisahi/scanf](https://github.com/hisahi/scanf): scanf(and other functions) library

## Acknowledgments

I would like to say thank you to people and websites that have helped me learn about OS development

- [araujo88 / primus-os](https://github.com/araujo88/primus-os/tree/main): I used his project as a model to learn from
- [GitHub](https://github.com/): Free code
- [othneildrew
/
Best-README-Template](https://github.com/othneildrew/Best-README-Template) This README
- [OsDev.org](https://wiki.osdev.org/Main_Page): Lots if informative pages and articles


- [OS](https://github.com/pritamzope/OS): I used A LOT of his code, ATA driver, VESA, FPU, etc
- My mom:  she did all my editing and made sure my grammar was correct
- And lastly: Anyone else that I have missed, thank you.
