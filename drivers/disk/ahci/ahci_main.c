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
#define MAX_CMD_SLOT 32

void setup_cmd_list(HBA_PORT *port);
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
#define HBA_PxIE_DHRE 0x1   // Device to Host Register FIS interrupt enable
#define HBA_PxIE_PCE  0x2   // PIO Setup FIS interrupt enable
#define HBA_PxIE_DSE  0x4   // DMA Setup FIS interrupt enable
#define HBA_PxIE_SDBE 0x8   // Set Device Bits FIS interrupt enable

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3
static int check_type(HBA_PORT *port);
void port_rebase(HBA_PORT *port, int portno);
void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
static int write_to_disk(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, void *buf);
static int read_from_disk(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, void *buf);
void* allocate_and_map_memory(uint32_t size) {
    void* addr = malloc(size); // Allocate memory
    if (!addr) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    void* addr2 = addr;
    map(addr2, addr, PAGE_PRESENT|PAGE_WRITE); // Map memory
    printf_com("%s = Mapping %p to %p\n",__func__,addr2,addr);
    return addr2;
}
void print_HBA_PORT(const HBA_PORT *port) {
    printf_com("clb: 0x%08X\n", port->clb);
    printf_com("clbu: 0x%08X\n", port->clbu);
    printf_com("fb: 0x%08X\n", port->fb);
    printf_com("fbu: 0x%08X\n", port->fbu);
    printf_com("is: 0x%08X\n", port->is);
    printf_com("ie: 0x%08X\n", port->ie);
    printf_com("cmd: 0x%08X\n", port->cmd);
    printf_com("rsv0: 0x%08X\n", port->rsv0);
    printf_com("tfd: 0x%08X\n", port->tfd);
    printf_com("sig: 0x%08X\n", port->sig);
    printf_com("ssts: 0x%08X\n", port->ssts);
    printf_com("sctl: 0x%08X\n", port->sctl);
    printf_com("serr: 0x%08X\n", port->serr);
    printf_com("sact: 0x%08X\n", port->sact);
    printf_com("ci: 0x%08X\n", port->ci);
    printf_com("sntf: 0x%08X\n", port->sntf);
    printf_com("fbs: 0x%08X\n", port->fbs);
    for (int i = 0; i < 11; ++i) {
        printf_com("rsv1[%d]: 0x%08X\n", i, port->rsv1[i]);
    }
    for (int i = 0; i < 4; ++i) {
        printf_com("vendor[%d]: 0x%08X\n", i, port->vendor[i]);
    }
}
void print_HBA_MEM(const HBA_MEM *mem)
{
	printf_com("les goo\n");
    printf_com("cap: 0x%08X\n", mem->cap);
    printf_com("ghc: 0x%08X\n", mem->ghc);
    printf_com("is: 0x%08X\n", mem->is);
    printf_com("pi: 0x%08X\n", mem->pi);
    printf_com("vs: 0x%08X\n", mem->vs);
    // printf_com("ccc_ctl: 0x%08X\n", mem->ccc_ctl);
    printf_com("ccc_pts: 0x%08X\n", mem->ccc_pts);
    printf_com("em_loc: 0x%08X\n", mem->em_loc);
    printf_com("em_ctl: 0x%08X\n", mem->em_ctl);
    printf_com("cap2: 0x%08X\n", mem->cap2);
    printf_com("bohc: 0x%08X\n", mem->bohc);
    // Print the values of vendor specific registers if needed
    for (int i = 0; i < 4; ++i)
    {
        // printf("vendor[%d]: 0x%08X\n", i, mem->vendor[i]);
    }
    // Print the values of port control registers
    for (int i = 0; i < 32; ++i)
    {
        // printf("Port %d:\n", i + 1);
        
    }
}
#define SECTOR_SIZE 512
#define NUM_SECTORS 12
#define MAGIC_NUMBER 0xDEADBEEF

// Test function to write and read back data
void test(HBA_PORT *port) {
    // Perform write operation
    uint64_t write_data = (uint64_t )malloc(4096);
    memset((void *)write_data, MAGIC_NUMBER, 4096);
    // for (size_t i = 0; i < 4096; i++)
    // {
    //  write_data[i] = i; write_data = MAGIC_NUMBER;
    // }
    
    if (write_to_disk(port, 0, 0, 1, write_data) != 0) {
        printf("Write failed\n");
        return;
    }

    // Perform read operation
    uint64_t *read_data = (uint64_t *)malloc(4096);
    memset(read_data, 0, 4096);
    if (read_from_disk(port, 0, 0, 1, read_data) != 0) {
        printf("Read failed\n");
        return;
    }

    // Verify data
    if (read_data == MAGIC_NUMBER) {
        printf("Write and read back successful\n");
    } else {
        printf("read_data == %08X\n\n",*read_data);
        printf("write_data == %08X\n\n",&write_data);

        // printf("Verification failed: Expected %08X, got %08X\n", write_data, *read_data);
    }
}
void group_read(HBA_PORT *port) {
    DWORD read_buf[SECTOR_SIZE / sizeof(DWORD) * NUM_SECTORS];

    // Read 12 sectors starting from sector 0
    if (read_from_disk(port, 0, 0, NUM_SECTORS, read_buf) == 0) {
        // Print out the read data
        for (int i = 0; i < NUM_SECTORS * SECTOR_SIZE / sizeof(DWORD); i++) {
            printf(">>%08x ", read_buf[i]);
            if ((i + 1) % (SECTOR_SIZE / sizeof(DWORD)) == 0) {
                printf("\n");
            }
        }

        // Zero out the buffer after printing
        memset(read_buf, 0, sizeof(read_buf));
    } else {
        printf("Read failed\n");
    }
}
void AHCI_handler(REGISTERS *r)
{
    printf("Called AHCI interrupt\n");
}
void init_port(HBA_PORT *port) {
    // Clear interrupt enable bits
    port->ie = 0;

    // Clear pending interrupts
    port->is = port->is;

    // Setup command list and FIS
    setup_cmd_list(port);
    setup_fis(port);

    // Enable interrupts
    port->ie = HBA_PxIE_DHRE | HBA_PxIE_PCE | HBA_PxIE_DSE | HBA_PxIE_SDBE;

    // Start command engine
    start_cmd(port);
}
void setup_cmd_list(HBA_PORT *port) {
    // Allocate and initialize Command List
    port->clb = (uint32_t)malloc(sizeof(HBA_CMD_HEADER) * MAX_CMD_SLOT);
    memset((void *)port->clb, 0, sizeof(HBA_CMD_HEADER) * MAX_CMD_SLOT);
    port->clbu = 0;

    // Initialize FIS
    port->fb = (uint32_t)malloc(sizeof(FIS_REG_H2D));
    memset((void *)port->fb, 0, sizeof(FIS_REG_H2D));
    port->fbu = 0;
}

int ahci_main()
{
    pci_config_register *dev = get_ahci_abar();
    HBA_MEM *abar = (HBA_MEM *)dev->base_address_5;
    printf("IRQ == %d\n",IRQ_BASE+dev->interrupt_line);
    isr_register_interrupt_handler(IRQ_BASE+dev->interrupt_line,AHCI_handler);
    map((uint32)abar,(uint32)abar,PAGE_PRESENT|PAGE_WRITE);
    printf_com("Mapping abar to %p\n",abar);
    abar->ghc |= 0x80000000; // Enable AHCI
    abar->ghc |= 0x00000001; // HBA reset
    while (abar->ghc & 0x00000001);
    print_HBA_MEM(abar);
    // printf("%p\n", abar);
    int32_t pi = abar->pi;
    int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
                // stop_cmd(&abar->ports[i]);
				printf("SATA drive found at port %d\n", i);
                // abar->ports;
                // map(&abar->ports[i],&abar->ports[i],PAGE_PRESENT);
              
                port_rebase(&abar->ports[i], i);
                init_port(&abar->ports[i]);
                // start_cmd(&abar->ports[i]); 
                // print_HBA_PORT(&abar->ports[i]);
                // group_read(&abar->ports[i]);
                  char read_buf[512] ;
                                    // memset(read_buf, 0, 512);
                print_HBA_PORT(&abar->ports[i]);
                test(&abar->ports[i]);
                printf("\nread_buf == [%s]\n",read_buf);
                // return;
                
                // start_cmd(&abar->ports[i]);
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
        // printf("%d",i);
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
void memset_ahci(void *dest, uint8_t val, uint32_t len)
{
    // printf("MEMSET: %p\n",dest);
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
void port_rebase(HBA_PORT *port, int portno) {
    // printf("%d: Stopping command engine\n", __LINE__);
    stop_cmd(port);
    // printf("%d\n", __LINE__);

    // Command list offset: 1K*portno
    port->clb = (DWORD)allocate_and_map_memory(1024);
    if (!port->clb) return;
    port->clbu = 0;
    // printf("port->clb: %p\n", (void*)port->clb);
    memset_ahci((void*)(port->clb), 0, 1024);
    // printf("%d\n", __LINE__);

    // FIS offset: 32K + 256*portno
    port->fb = (DWORD)allocate_and_map_memory(256);
    if (!port->fb) return;
    port->fbu = 0;
    // printf("port->fb: %p\n", (void*)port->fb);
    memset_ahci((void*)(port->fb), 0, 256);
    // printf("%d\n", __LINE__);

    // Command table offset: 40K + 8K*portno
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    // printf("%d\n", __LINE__);
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
        cmdheader[i].ctba = (DWORD)allocate_and_map_memory(256);
        if (!cmdheader[i].ctba) return;
        cmdheader[i].ctbau = 0;
        // printf("cmdheader[%d].ctba: %p\n", i, (void*)cmdheader[i].ctba);
        memset_ahci((void*)cmdheader[i].ctba, 0, 256);
    }
    printf("Starting command engine\n");
    start_cmd(port);
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
static int read_from_disk(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, void *buf) {
    stop_cmd(port);

    // Setup command header
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    cmdheader[0].cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD); // Command FIS length
    cmdheader[0].w = 0; // Read operation
    cmdheader[0].prdtl = (WORD)((count - 1) >> 4) + 1; // PRDT entries count

    // Setup command table
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader[0].ctba);
    memset_ahci(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader[0].prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

    // Setup PRDT entry
    cmdtbl->prdt_entry[0].dba = (DWORD)buf; // Physical address of data buffer
    cmdtbl->prdt_entry[0].dbc = (count << 9) - 1; // Byte count: 512 bytes per sector
    cmdtbl->prdt_entry[0].i = 1; // Interrupt on completion

    // Setup command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl >> 8);
    cmdfis->lba2 = (BYTE)(startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (BYTE)(startl >> 24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth >> 8);

    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;

    start_cmd(port);

    // Wait for command completion
    while (port->ci & 1);

    // Check for errors
    if (port->is & HBA_PxIS_TFES) {
        printf("Read disk error\n");
        return -1;
    }

    return 0;
}
static int write_to_disk(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, void *buf) {
    stop_cmd(port);

    // Setup command header
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    cmdheader[0].cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD); // Command FIS length
    cmdheader[0].w = 1; // Write operation
    cmdheader[0].prdtl = (WORD)((count - 1) >> 4) + 1; // PRDT entries count

    // Setup command table
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader[0].ctba);
    memset_ahci(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader[0].prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

    // Setup PRDT entry
    cmdtbl->prdt_entry[0].dba = (DWORD)buf; // Physical address of data buffer
    cmdtbl->prdt_entry[0].dbc = (count << 9) - 1; // Byte count: 512 bytes per sector
    cmdtbl->prdt_entry[0].i = 1; // Interrupt on completion

    // Setup command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;

    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl >> 8);
    cmdfis->lba2 = (BYTE)(startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (BYTE)(startl >> 24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth >> 8);

    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;

    start_cmd(port);

    // Wait for command completion
    while (port->ci & 1);

    // Check for errors
    if (port->is & HBA_PxIS_TFES) {
        printf("Write disk error\n");
        return -1;
    }

    return 0;
}

