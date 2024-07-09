
#include "pci_io.h"
#include "net/e1000.h"
#include "vmm.h"
#include "stdlib.h"
#include "isr.h"
int read_from_reg_e1000(uint64_t ioaddr,uint32_t reg);
int write_to_reg_e1000(uint64_t ioaddr,uint32_t reg,uint32_t val);
void reset_nic(uint32_t ioaddr);
bool detectEEProm();
uint32_t e1000_read32 (uint64_t p_address);
void e1000_write32 (uint64_t p_address,uint32_t p_value);
void e1000_writeCommand(uint16_t p_address, uint32_t p_value);
uint32_t e1000_readCommand(uint16_t p_address);
uint32_t eepromRead( uint8_t addr);
int sendPacket(const void * p_data, uint16_t p_len);
void fire (void);
uint64_t e1000_ioaddr_g;
uint8_t mac [6];
struct e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC]; // Receive Descriptor Buffers
struct e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC]; // Transmit Descriptor Buffers
uint16_t rx_cur;      // Current Receive Descriptor Buffer
uint16_t tx_cur;      // Current Transmit Descriptor Buffer

void *e1000_malloc(size_t size)
{
    void *addr = malloc(size);
    // Calculate the number of pages needed
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    void *current_addr = addr;

    // Map each page
    for (size_t i = 0; i < num_pages; i++)
    {
        void *page_addr = (void *)((uintptr_t)current_addr + i * PAGE_SIZE);
        map((uint32_t)page_addr, (uint32_t)page_addr, PAGE_PRESENT | PAGE_WRITE);
        printf_com("mapping pa:0x%x to va:0x%x\n", page_addr, page_addr);
    }

    return addr;
}

int init_e1000()
{
    pci_config_register *dev = get_e1000_data();
    if(dev == NULL)
    {
        printf("Unable to to find network interface\n");
        return -1;
    }
    init_achi_pci(dev->bus,dev->slot,dev->func);
    printf("Found e1000 network device\n");
    uint32_t e1000_ioaddr = dev->base_address_0;
    printf_com("E1000 BAR0 == %x\n",dev->base_address_0);
    printf_com("E1000 BAR1 == %x\n",dev->base_address_1);
    printf_com("E1000 BAR2 == %x\n",dev->base_address_2);
    printf_com("E1000 BAR3 == %x\n",dev->base_address_2);
    printf_com("E1000 BAR4 == %x\n",dev->base_address_4);
    printf_com("E1000 BAR5 == %x\n",dev->base_address_5);
    map((uint32_t)e1000_ioaddr,(uint32_t)e1000_ioaddr,PAGE_PRESENT|PAGE_WRITE);
    for (size_t i = 1; i <14 ; i++)
    {
       map((uint32_t)e1000_ioaddr+(PAGE_SIZE*i),(uint32_t)e1000_ioaddr+(PAGE_SIZE*i),PAGE_PRESENT|PAGE_WRITE);
    }
    
    
     printf("MMIO is at 0x%x\n",e1000_ioaddr);
     e1000_ioaddr_g = e1000_ioaddr;
    reset_nic(e1000_ioaddr);
    bool EEProm_exists = detectEEProm();
    
    if(EEProm_exists == true)
    {
        printf("EEProm detected\n");
    }
    else
    {
        printf("EEProm not found\n");
        return -1;
    }
    isr_register_interrupt_handler(IRQ_BASE+dev->interrupt_line, fire);
    // for(int i = 0; i < 0x80; i++)
    //     e1000_writeCommand(0x5200 + i*4, 0);
    readMACAddress();
    print_mac();
    rxinit();
    txinit();     
    sendDummyPacket();
    send_dhcp_request();

}
void reset_nic(uint32_t ioaddr) {
    e1000_writeCommand(REG_CTRL, CTRL_RST);
    
}

int write_to_reg_e1000(uint64_t ioaddr,uint32_t reg,uint32_t val)
{
    *(uint32_t *)(ioaddr + reg) = val;
}
int read_from_reg_e1000(uint64_t ioaddr,uint32_t reg)
{
    uint32_t val = *(uint32_t *)(ioaddr + reg);
    return val;
}
bool detectEEProm()
{
    uint32_t val = 0;
    e1000_writeCommand(REG_EEPROM, 0x1); 
    bool eerprom_exists;
    for(int i = 0; i < 1000 && ! eerprom_exists; i++)
    {
            val =  e1000_readCommand( REG_EEPROM);
            if(val & 0x10)
                    eerprom_exists = true;
            else
                    eerprom_exists = false;
    }
    return eerprom_exists;
}



void e1000_write32 (uint64_t p_address,uint32_t p_value)
{
    printf_com("e1000_write32(uint64_t p_address(0x%x),uint32_t p_value(0x%x))\n",p_address,p_value);
    (*((volatile uint32_t*)(p_address)))=(p_value);
    
}
uint32_t e1000_read32 (uint64_t p_address)
{
    return *((volatile uint32_t*)(p_address));
    
}

void e1000_writeCommand(uint16_t p_address, uint32_t p_value)
{
    printf_com("e1000_writeCommand(uint16_t p_address(0x%x),uint32_t p_value(0x%x))\n",p_address,p_value);
    e1000_write32(e1000_ioaddr_g+p_address,p_value);
}

uint32_t e1000_readCommand(uint16_t p_address)
{
    return e1000_read32(e1000_ioaddr_g+p_address);
}

uint32_t eepromRead( uint8_t addr)
{
	uint16_t data = 0;
	uint32_t tmp = 0;
        
        
    e1000_writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
    while( !((tmp = e1000_readCommand(REG_EEPROM)) & (1 << 4)) );
        
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

void readMACAddress()
{
   
        uint32_t temp;
        temp = eepromRead( 0);
        mac[0] = temp &0xff;
        mac[1] = temp >> 8;
        temp = eepromRead( 1);
        mac[2] = temp &0xff;
        mac[3] = temp >> 8;
        temp = eepromRead( 2);
        mac[4] = temp &0xff;
        mac[5] = temp >> 8;
}
void print_mac()
{

    printf("MAC Address: ");
    for (int i = 0; i < 6; ++i) {
        printf("%02x", mac[i]); // PRIx8 is a macro to print uint8_t as hexadecimal
        if (i < 5) {
            printf(":"); // Print colon between each byte except the last one
        }
    }
    printf("\n");

}


void rxinit()
{
    printf_com("rxinit\n");
    uint8_t * ptr;
    struct e1000_rx_desc *descs;

    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
 
    ptr = (uint8_t *)(e1000_malloc(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16));

    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        rx_descs[i]->addr = (uint64_t)(uint8_t *)(e1000_malloc(8192 + 16));
        rx_descs[i]->status = 0;
    }
    printf("Allocations complete\n");
    printf_com("(uint32_t)((uint64_t)ptr >> 32) == 0x%x\n",(uint32_t)((uint64_t)ptr >> 32) );
    e1000_writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    printf("%d\n__LINE__",__LINE__);
    e1000_writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
    printf("%d\n__LINE__",__LINE__);

    e1000_writeCommand(REG_RXDESCLO, (uint64_t)ptr);
    printf("%d\n__LINE__",__LINE__);

    e1000_writeCommand(REG_RXDESCHI, 0);
    printf("%d\n__LINE__",__LINE__);

    e1000_writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
    printf("%d\n__LINE__",__LINE__);

    e1000_writeCommand(REG_RXDESCHEAD, 0);
    printf("%d\n__LINE__",__LINE__);

    e1000_writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    printf("%d\n__LINE__",__LINE__);

    rx_cur = 0;
    e1000_writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
    printf("%d\n__LINE__",__LINE__);
    
}


void txinit()
{    
    uint8_t *  ptr;
    struct e1000_tx_desc *descs;
    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
    ptr = (uint8_t *)(e1000_malloc(sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16));

    descs = (struct e1000_tx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
        tx_descs[i]->addr = 0;
        tx_descs[i]->cmd = 0;
        tx_descs[i]->status = TSTA_DD;
    }

    e1000_writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    e1000_writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));


    //now setup total length of descriptors
    e1000_writeCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);


    //setup numbers
    e1000_writeCommand( REG_TXDESCHEAD, 0);
    e1000_writeCommand( REG_TXDESCTAIL, 0);
    tx_cur = 0;
    e1000_writeCommand(REG_TCTRL,  TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);

    // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards 
    // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
    // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
    e1000_writeCommand(REG_TCTRL,  0b0110000000000111111000011111010);
    e1000_writeCommand(REG_TIPG,  0x0060200A);

}

int sendPacket(const void * p_data, uint16_t p_len)
{    
    tx_descs[tx_cur]->addr = (uint64_t)p_data;
    tx_descs[tx_cur]->length = p_len;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    tx_descs[tx_cur]->status = 0;
    uint8_t old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    e1000_writeCommand(REG_TXDESCTAIL, tx_cur);   
    while(!(tx_descs[old_cur]->status & 0xff));    
    return 0;
}
int sendDummyPacket() {
    // Define a dummy packet with some arbitrary data
    const char dummy_data[] = "This is a dummy packet.";
    uint16_t packet_length = sizeof(dummy_data);

    // Call the sendPacket function with the dummy data and its length
    return sendPacket(dummy_data, packet_length);
}
void fire (void)
{
    if (1==1)
    {        
        /* This might be needed here if your handler doesn't clear interrupts from each device and must be done before EOI if using the PIC.
           Without this, the card will spam interrupts as the int-line will stay high. */
        e1000_writeCommand(REG_IMASK, 0x1);
       
        uint32_t status = e1000_readCommand(0xc0);
        if(status & 0x04)
        {
            printf("Start link()\n");
            // startLink();
        }
        else if(status & 0x10)
        {
           // good threshold
        }
        else if(status & 0x80)
        {
            handleReceive();
        }
    }
}
void handleReceive()
{
    printf("HandleReceive()\n");
    uint16_t old_cur;
    bool got_packet = false;
 
    while((rx_descs[rx_cur]->status & 0x1))
    {
            got_packet = true;
            uint8_t *buf = (uint8_t *)rx_descs[rx_cur]->addr;
            uint16_t len = rx_descs[rx_cur]->length;

            // Here you should inject the received packet into your network stack


            rx_descs[rx_cur]->status = 0;
            old_cur = rx_cur;
            rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
            e1000_writeCommand(REG_RXDESCTAIL, old_cur );
    }    
}