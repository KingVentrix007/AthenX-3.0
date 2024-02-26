#include <stdio.h>

#include "fis.h"
#include "ahci.h"

#include "pci_io.h"
#include "isr.h"
#include "vmm.h"
#include "stdlib.h"
uint32_t HBA_MEM_BASE_ADDRESS;
#define	AHCI_BASE	0x400000	// 4M
// Define DWORD as an unsigned 32-bit integer
typedef unsigned int DWORD;

// Define QWORD as an unsigned 64-bit integer
typedef unsigned long long QWORD;

// Define BYTE as an unsigned 8-bit integer
typedef unsigned char BYTE;
typedef unsigned short WORD;
#define HBA_PxIS_TFES   (1 << 30)       /* TFES - Task File Error Status */
#define FIS_SIZE 256 // Size of FIS (arbitrary value, adjust as needed)
#define NUM_PORTS 32 // Number of ports on the AHCI controller (adjust as needed)
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */
HBA_MEM *abar;
static int check_type(HBA_PORT *port);
void ahci_handler(REGISTERS *r);
void enable_ahci_mode(HBA_MEM *abar);
typedef struct {
    FIS_REG_H2D h2d_fis[NUM_PORTS]; // Host to Device FIS
    FIS_REG_D2H d2h_fis[NUM_PORTS]; // Device to Host FIS
} FIS_MEMORY;
FIS_MEMORY* init_fis_memory();
void setup(HBA_MEM* ptr);
void probe_port(HBA_MEM *abar);
void print_HBA_PORT(const HBA_PORT *port);
void print_HBA_MEM(const HBA_MEM *mem);
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define PORT_NUM 0
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4
 #define H2D_DATA_BASE_ADDRESS 0x1000000 // host to data を格納するアドレス
#define D2H_DATA_BASE_ADDRESS 0x1000000 // data to host を格納するアドレス
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

// Structure to represent FIS memory region

uint64_t *pages_for_ahci_start;
uint64_t *pages_for_ahci_start_virtual;

int ahci_main(void) {
	printf("ACHI MAIN\n");
	uint32_t mem_port = get_ahci_abar();
	
	// HBA_MEM* hba_mem_ptr = (HBA_MEM*)(uintptr_t)();
	isr_register_interrupt_handler(IRQ_BASE+11,ahci_handler);
	// setup(hba_mem_ptr);
	uint32_t va = mem_port;
	map(va,mem_port,PAGE_PRESENT | PAGE_WRITE);
    // setup(va);
	probe_port((HBA_MEM *)va);
    
    HBA_MEM* hba_mem_ptr = (HBA_MEM *)va;
	HBA_PORT port = (HBA_PORT )hba_mem_ptr->ports[0];
    enable_ahci_mode(hba_mem_ptr);
    printf("Enabled ahci mode\n");
    abar = hba_mem_ptr;
    char buf[100];
    if(read(&port,12,23,1,buf))
    {
        printf("Successfully read\n");
    }
    // Assume abar is the base address of AHCI controller memory-mapped registers
    // HBA_MEM* abar = (HBA_MEM*)get_ahci_abar();

    // Initialize AHCI ports
	// if(sata_read(&port, ))

	// debug_HBA_MEM(hba_mem_ptr);
	// printf("Read\n");
	// // testWrite(hba_mem_ptr);
	// testRead(hba_mem_ptr);
}
void probe_port(HBA_MEM *abar_temp)
{
    if(abar_temp == NULL)
    {
        printf("error: probe_port failed\n");
        return;
    }
    printf("probe_port at 0x%0X\n", abar_temp);
	// print_HBA_MEM(abar);
    // Search disk in implemented ports
    uint32_t pi = abar_temp->pi;
    printf("PI %u\n", pi);
    int i = 0;
    while (i < 32)
    {
        if (pi & 1)
        {
            // printf("%d\n", i);
            int dt = check_type(&abar_temp->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                printf("SATA drive found at port %d\n", i);
                port_rebase(abar_temp->ports, i);
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
                // printf("No drive found at port %d\n", i);
            }
        }
 
        pi >>= 1;
        i++;
    }
}

 
// Check device type
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
void ahci_handler(REGISTERS *r)
{
	printf("Called AHCI handler\n");
}
void print_HBA_PORT(const HBA_PORT *port) {
    printf("clb: 0x%08X\n", port->clb);
    printf("clbu: 0x%08X\n", port->clbu);
    printf("fb: 0x%08X\n", port->fb);
    printf("fbu: 0x%08X\n", port->fbu);
    printf("is: 0x%08X\n", port->is);
    printf("ie: 0x%08X\n", port->ie);
    printf("cmd: 0x%08X\n", port->cmd);
    printf("rsv0: 0x%08X\n", port->rsv0);
    printf("tfd: 0x%08X\n", port->tfd);
    printf("sig: 0x%08X\n", port->sig);
    printf("ssts: 0x%08X\n", port->ssts);
    printf("sctl: 0x%08X\n", port->sctl);
    printf("serr: 0x%08X\n", port->serr);
    printf("sact: 0x%08X\n", port->sact);
    printf("ci: 0x%08X\n", port->ci);
    printf("sntf: 0x%08X\n", port->sntf);
    printf("fbs: 0x%08X\n", port->fbs);
    for (int i = 0; i < 11; ++i) {
        printf("rsv1[%d]: 0x%08X\n", i, port->rsv1[i]);
    }
    for (int i = 0; i < 4; ++i) {
        printf("vendor[%d]: 0x%08X\n", i, port->vendor[i]);
    }
}
void print_HBA_MEM(const HBA_MEM *mem)
{
	printf("les goo\n");
    printf("cap: 0x%08X\n", mem->cap);
    printf("ghc: 0x%08X\n", mem->ghc);
    printf("is: 0x%08X\n", mem->is);
    printf("pi: 0x%08X\n", mem->pi);
    printf("vs: 0x%08X\n", mem->vs);
    printf("ccc_ctl: 0x%08X\n", mem->ccc_ctl);
    printf("ccc_pts: 0x%08X\n", mem->ccc_pts);
    printf("em_loc: 0x%08X\n", mem->em_loc);
    printf("em_ctl: 0x%08X\n", mem->em_ctl);
    printf("cap2: 0x%08X\n", mem->cap2);
    printf("bohc: 0x%08X\n", mem->bohc);
    // Print the values of vendor specific registers if needed
    for (int i = 0; i < 4; ++i)
    {
        printf("vendor[%d]: 0x%08X\n", i, mem->vendor[i]);
    }
    // Print the values of port control registers
    for (int i = 0; i < 32; ++i)
    {
        printf("Port %d:\n", i + 1);
        print_HBA_PORT(&mem->ports[i]);
    }
}
void enable_ahci_mode(HBA_MEM *abar) {
    // Check if AHCI is already enabled in BIOS (skipped in programmatic approach)

    // Set AHCI enable bit in GHC register
    abar->ghc |= (1U << 31); // Set bit 31 to enable AHCI mode

    // Wait for AHCI controller to become ready (optional)
    // You may need to wait for a short period for the AHCI controller to become ready after enabling AHCI mode. This can involve waiting for a specific status bit in the GHC register to be set.

    // AHCI mode enabled
}


int read(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, QWORD buf)  
{
    //   buf = KERNBASE + buf;
        
}
void port_rebase(HBA_PORT *port, int portno)
{
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
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
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
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