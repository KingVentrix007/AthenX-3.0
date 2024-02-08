<p align="center">
  <b><font size="6">AthenX-3.0</font></b>
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
  - [ ] ext2/3/4
  - [ ] NTFS
  - [ ] ISO 9660

- [ ] ACPI support
  - [ ] Power on/off
  - [ ] Reboot
  - [ ] Power button support

- [ ] PCI support
- [ ] PCI Express support
- [ ] Network drivers
  - [ ] Ethernet cards
    - Intel Ethernet i217
    - RTL8139
    - RTL8169
    - Intel 8254x
  - [ ] Wireless Network driver

- [ ] libc
