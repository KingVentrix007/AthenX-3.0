#include <stdio.h>

#include "fis.h"
#include "ahci.h"

#include "pci_io.h"
#include "isr.h"
#include "vmm.h"
#include "stdlib.h"
#include "printf.h"
#include "string.h"
uint32_t HBA_MEM_BASE_ADDRESS;
#define AHCI_BASE 0x400000 // 4M
typedef unsigned int DWORD;
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
typedef unsigned long long QWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
#define HBA_PxIS_TFES (1 << 30)
#define FIS_SIZE 256
#define NUM_PORTS 32
#define HBA_PxCMD_CR (1 << 15)
#define HBA_PxCMD_FR (1 << 14)
#define HBA_PxCMD_FRE (1 << 4)
#define HBA_PxCMD_SUD (1 << 1)
#define HBA_PxCMD_ST (1 << 0)
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3
#define HBA_RESET 0x01      // HBA reset bit in Global Host Control register
#define AHCI_ENABLE 0x80000000  // AHCI Enable bit in Global Host Control register
#define AHCI_IE 0x00000002      // Interrupt Enable bit in Global Host Control register 


void probe_port(HBA_MEM *abar);
static int check_type(HBA_PORT *port);
void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
void port_rebase(HBA_PORT *port, int portno);

void *ahci_malloc(size_t size, size_t alignment,int line,char msg[1200])
{
    if (alignment & (alignment - 1)) {
        // Alignment must be a power of two
        return NULL;
    }

    size_t total_size = size + alignment - 1;
    void *raw_addr = malloc(total_size);

    if (raw_addr == NULL)
    {
        return NULL;
    }

    uintptr_t aligned_addr = ((uintptr_t)raw_addr + alignment - 1) & ~(alignment - 1);
    map(aligned_addr, aligned_addr, PAGE_PRESENT | PAGE_WRITE);
    printf_com("AHCI :: Allocating %d bytes of memory at %p for %s(%d)\n",total_size,aligned_addr,msg,line);
    return (void *)aligned_addr;

}
void reset_ahci_controller(HBA_MEM *abar) {
    // Request HBA reset
    abar->ghc |= HBA_RESET;
    
    // Wait for reset to complete
    while (abar->ghc & HBA_RESET) {
    }
}
void ahci_isr(REGISTERS16 *reg)
{
    printf("ahci isr called\n");
}
void enable_ahci_mode_and_interrupts(HBA_MEM *abar) {
    // Set the AHCI Enable bit and Interrupt Enable bit
    abar->ghc |= (AHCI_ENABLE | AHCI_IE);
}

int ahci_main()
{
    printf_com("AHCI :: Initializing AHCI\nFrom this point on, debug logs are related to AHCI or functions called in AHCI\n----------------------------------------------------------------\n");
    pci_config_register *dev = get_ahci_abar();
    HBA_MEM *abar = (HBA_MEM *)dev->base_address_5;
    init_achi_pci(dev->bus,dev->slot,dev->func); //Enable interrupts, DMA, and memory space access in the PCI command register
    // enable_bus_mastering(dev->bus,dev->slot,dev->func);
    map((uint32_t)abar,(uint32_t)abar,PAGE_PRESENT|PAGE_WRITE); //Memory map BAR 5 register as uncacheable.

    reset_ahci_controller(abar); //Reset controller

    int irq = dev->interrupt_line;
    isr_register_interrupt_handler(irq, ahci_isr); //Register IRQ handler, using interrupt line given in the PCI register. This interrupt line may be shared with other devices, so the usual implications of this apply.

    enable_ahci_mode_and_interrupts(abar); //Enable AHCI mode and interrupts in global host control register.

    probe_port(abar); // Scan all ports
    printf_com("AHCI :: AHCI debug logs end here\n----------------------------------------------------------------\n");



}
void probe_port(HBA_MEM *abar)
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
            printf("\nScanning ports\n");
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				printf("SATA drive found at port %d\n", i);
                port_rebase(&abar->ports[i],i);
                return;
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				printf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				printf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				printf("PM drive found at port %d\n", i);
			}
			else
			{
				printf("No drive found at port %d\n", i);
			}
		}

		pi >>= 1;
		i ++;
	}
}
static int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}





void port_rebase(HBA_PORT *port, int portno)
{
	stop_cmd(port);	// Stop command engine

	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
    uint32_t clb_adder = ahci_malloc(1024,1024*4,__LINE__,"port->clb-> Command list");
    printf_com("AHCI :: clb is at %p\n",clb_adder);
	port->clb = clb_adder;
	port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);

	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
    uint32_t fb_adder = ahci_malloc(256,4096,__LINE__,"FIS entry");
    printf_com("AHCI :: fb is at %p\n",fb_adder);
    port->fb = fb_adder;
	// port->fb = ahci_malloc(256,256,__LINE__,"FIS entry");
	port->fbu = 0;
	memset((void*)(port->fb), 0, 256);

	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
        uint32_t ct_adder = ahci_malloc(256,4096,__LINE__,"Command table");
        printf_com("AHCI :: ct is at %p\n",ct_adder);
        cmdheader[i].ctba = ct_adder;
		// cmdheader[i].ctba = ahci_malloc(256,256,__LINE__,"Command table");
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}

	start_cmd(port);	// Start command engine
    printf("Done Rebasing port %d\n", portno);
}

// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}

// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}

}