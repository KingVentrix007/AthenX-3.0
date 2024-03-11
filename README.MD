<p align="center">
  <b><font size="20">AthenX-3.0</font></b>
</p>

A new and improved version of AthenX-2.0. With realistic goals,better code,and more features.

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
    - Intel Ethernet i217
    - RTL8139
    - RTL8169
    - Intel 8254x
  - [ ] Wireless Network driver

- [ ] libc
- [ ] Other
  - [X] stdin/stdout/stderr
  - [ ] Special files (/dev/tty, /dev/fmb)

