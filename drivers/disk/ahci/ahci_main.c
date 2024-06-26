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

#define SECTOR_SIZE 512

HBA_PORT *port;

void probe_port(HBA_MEM *abar);
static int check_type(HBA_PORT *port);
void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
void port_rebase(HBA_PORT *port, int portno);
int ahci_write_sector(HBA_PORT *port, uint64_t start_lba, void *buf, uint32_t count);
int ahci_read_sector(HBA_PORT *port, uint64_t start_lba, void *buf, uint32_t count);


int ahci_write_sector_fat(uint32 sector, uint8 *buffer, uint32 sector_count)
{
	int ret =  ahci_write_sector(port, sector, buffer,sector_count);
	// printf("%s == %d\n",__func__,ret);
	return ret;
}
int ahci_read_sector_fat(uint32 sector, uint8 *buffer, uint32 sector_count)
{
	int ret = ahci_read_sector(port, sector, buffer, sector_count);
	// printf("%s == %d\n",__func__,ret);
	return ret;
}
uint32_t ahci_malloc(size_t size, size_t alignment,int line,char msg[1200])
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
    return (uint32_t)aligned_addr;

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
    isr_register_interrupt_handler(IRQ_BASE+irq, ahci_isr); //Register IRQ handler, using interrupt line given in the PCI register. This interrupt line may be shared with other devices, so the usual implications of this apply.

    enable_ahci_mode_and_interrupts(abar); //Enable AHCI mode and interrupts in global host control register.

    probe_port(abar); // Scan all ports
    // printf("Successfully initialized AHCI controller\n");
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
            // printf("\nScanning ports\n");
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				// printf("SATA drive found at port %d\n", i);
                port_rebase(&abar->ports[i],i);
				uint32_t start_sector = 0; // Starting LBA
				uint32_t num_sectors = 32; // Number of sectors to read
				char buffer[SECTOR_SIZE * 32]; // Buffer to hold 32 sectors of data
				char write_buffer[512]; // Buffer to hold 1 sector of data
				port = &abar->ports[i];
    			// Fill buffer with data to write (example data)
			

				
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
    // Stop command engine and check status
    stop_cmd(port);
    if (port->cmd & HBA_PxCMD_CR) {
        printf_com("AHCI :: Failed to stop command engine for port %d\n", portno);
        return;
    }

    // Command list offset: 1K*portno
    uint32_t clb_addr = ahci_malloc(1024, 4096, __LINE__, "port->clb-> Command list");
    if (clb_addr == 0) {
        printf_com("AHCI :: Failed to allocate Command List buffer for port %d\n", portno);
        return;
    }
    printf_com("AHCI :: Command List buffer allocated at %p\n", clb_addr);
    port->clb = clb_addr;
    port->clbu = 0;
    memset((void*)(port->clb), 0, 1024);
	  uint32_t test_value = 0xDEADBEEF;
    *(volatile uint32_t*)(port->clb) = test_value;
    uint32_t read_back = *(volatile uint32_t*)(port->clb);
    if (read_back != test_value) {
        printf("AHCI :: Failed to verify Command List buffer for port %d. Expected %x, got %x\n", portno, test_value, read_back);
        return;
    } else {
        // printf("AHCI :: Successfully verified Command List buffer for port %d. Value: %x\n", portno, read_back);
    }
    memset((void*)(port->clb), 0, 1024);

    // FIS offset: 32K+256*portno
    uint32_t fb_addr = ahci_malloc(256, 4096, __LINE__, "FIS entry");
    if (fb_addr == 0) {
        printf("AHCI :: Failed to allocate FIS buffer for port %d\n", portno);
        return;
    }
    printf_com("AHCI :: FIS buffer allocated at %p\n", fb_addr);
    port->fb = fb_addr;
    port->fbu = 0;
    memset((void*)(port->fb), 0, 256);

    // Command table offset: 40K + 8K*portno
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table

        uint32_t ct_addr = ahci_malloc(256, 4096, __LINE__, "Command table");
        if (ct_addr == 0) {
            printf_com("AHCI :: Failed to allocate Command Table buffer for port %d, command header %d\n", portno, i);
            return;
        }
        printf_com("AHCI :: Command Table buffer allocated at %p\n", ct_addr);
        cmdheader[i].ctba = ct_addr;
        cmdheader[i].ctbau = 0;
        memset((void*)cmdheader[i].ctba, 0, 256);
    }

    // Start command engine and check status
    start_cmd(port);
    // if (port->cmd & HBA_PxCMD_CR) {
    //     // printf("AHCI :: Failed to start command engine for port %d\n", portno);
    //     return;
    // }

    // printf("Done Rebasing port %d\n", portno);
}

// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	printf_com("AHCI :: port->clb == %p\n",port->clb);
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
//41CC1000
//41CC1000

int ahci_read_sector(HBA_PORT *port, uint64_t start_lba, void *buf, uint32_t count)
{
    port->is = (uint32_t)-1; // Clear pending interrupt bits

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // Command FIS size
    cmdheader->w = 0; // Read from device
    cmdheader->prdtl = (count + 7) / 8; // PRDT entries count

    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

    // Setup PRDT
    for (int i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = (uint32_t)buf + (i * 4096);
        cmdtbl->prdt_entry[i].dbc = 4096 - 1; // 4KB
        cmdtbl->prdt_entry[i].i = 1; // Interrupt on completion
    }
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dba = (uint32_t)buf + ((cmdheader->prdtl - 1) * 4096);
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dbc = ((count * 512) % 4096) - 1;
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].i = 1; // Interrupt on completion

    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (uint8_t)(start_lba & 0xFF);
    cmdfis->lba1 = (uint8_t)((start_lba >> 8) & 0xFF);
    cmdfis->lba2 = (uint8_t)((start_lba >> 16) & 0xFF);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (uint8_t)((start_lba >> 24) & 0xFF);
    cmdfis->lba4 = (uint8_t)((start_lba >> 32) & 0xFF);
    cmdfis->lba5 = (uint8_t)((start_lba >> 40) & 0xFF);

    cmdfis->countl = count & 0xFF; // Sector count
    cmdfis->counth = (count >> 8) & 0xFF;

    // Wait for port to be ready
    uint32_t timeout = 0;
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && timeout < 1000000)
    {
        timeout++;
    }

    if (timeout >= 1000000)
    {
        return 0; // Timeout, return failure
    }

    port->ci = 1; // Issue command

    // Wait for completion
    while (1)
    {
        if ((port->ci & 1) == 0) break;
        if (port->is & HBA_PxIS_TFES) // Task file error
        {
            return 0; // Read disk error
        }
    }

    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        return 0; // Read disk error
    }

    return 1; // Read successfully
}
int ahci_write_sector(HBA_PORT *port, uint64_t start_lba, void *buf, uint32_t count)
{
    port->is = (uint32_t)-1; // Clear pending interrupt bits

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // Command FIS size
    cmdheader->w = 1; // Write to device
    cmdheader->prdtl = (count + 7) / 8; // PRDT entries count

    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

    // Setup PRDT
    for (int i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = (uint32_t)buf + (i * 4096);
        cmdtbl->prdt_entry[i].dbc = 4096 - 1; // 4KB
        cmdtbl->prdt_entry[i].i = 1; // Interrupt on completion
    }
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dba = (uint32_t)buf + ((cmdheader->prdtl - 1) * 4096);
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dbc = ((count * 512) % 4096) - 1;
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].i = 1; // Interrupt on completion

    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;

    cmdfis->lba0 = (uint8_t)(start_lba & 0xFF);
    cmdfis->lba1 = (uint8_t)((start_lba >> 8) & 0xFF);
    cmdfis->lba2 = (uint8_t)((start_lba >> 16) & 0xFF);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (uint8_t)((start_lba >> 24) & 0xFF);
    cmdfis->lba4 = (uint8_t)((start_lba >> 32) & 0xFF);
    cmdfis->lba5 = (uint8_t)((start_lba >> 40) & 0xFF);

    cmdfis->countl = count & 0xFF; // Sector count
    cmdfis->counth = (count >> 8) & 0xFF;

    // Wait for port to be ready
    uint32_t timeout = 0;
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && timeout < 1000000)
    {
        timeout++;
    }

    if (timeout >= 1000000)
    {
        return 0; // Timeout, return failure
    }

    port->ci = 1; // Issue command

    // Wait for completion
    while (1)
    {
        if ((port->ci & 1) == 0) break;
        if (port->is & HBA_PxIS_TFES) // Task file error
        {
            return 0; // Write disk error
        }
    }

    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        return 0; // Write disk error
    }

    return 1; // Write successfully
}