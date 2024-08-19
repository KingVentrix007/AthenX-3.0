/*
 * File: acpi.c
 * Project: osdev
 * Original Source: https://github.com/pdoane/osdev/blob/master/acpi/acpi.c
 * Repository: https://github.com/pdoane/osdev/
 *
 * This file is part of the osdev project, originally developed by pdoane.
 * The original code is licensed under the Zlib license.
 *
 * Modifications made by Tristan Kuhn(KingVentrix007) on [8/12/2024].
 *
 * Zlib License:
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

// ------------------------------------------------------------------------------------------------
// acpi/acpi.c
// ------------------------------------------------------------------------------------------------

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#define MAX_CPU_COUNT 16
#define PACKED __attribute__((packed))
uint8_t *g_localApicAddr;
uint8_t *g_ioApicAddr;
// ------------------------------------------------------------------------------------------------
// Globals
unsigned int g_acpiCpuCount;
uint8_t g_acpiCpuIds[MAX_CPU_COUNT];

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
typedef struct AcpiHeader
{
    uint32_t signature;          // Signature to identify the table
    uint32_t length;             // Length of the table in bytes, including the header
    uint8_t revision;            // Version of the table
    uint8_t checksum;            // Checksum of the entire table
    uint8_t oem[6];              // OEM ID (6 characters)
    uint8_t oemTableId[8];       // OEM Table ID (8 characters)
    uint32_t oemRevision;        // OEM Revision number
    uint32_t creatorId;          // Creator ID
    uint32_t creatorRevision;    // Creator Revision number
} PACKED AcpiHeader;

// ------------------------------------------------------------------------------------------------
typedef struct AcpiFadt
{
    AcpiHeader header;           // Common ACPI table header
    uint32_t firmwareControl;    // 32-bit Physical address of FACS
    uint32_t dsdt;               // 32-bit Physical address of DSDT
    uint8_t reserved;            // Reserved (must be 0)
    uint8_t preferredPMProfile;  // Preferred power management profile
    uint16_t sciInterrupt;       // System control interrupt
    uint32_t smiCommandPort;     // SMI command port
    uint8_t acpiEnable;          // Value to write to SMI_CMD to enable ACPI
    uint8_t acpiDisable;         // Value to write to SMI_CMD to disable ACPI
    uint8_t s4BiosReq;           // Value to write to SMI_CMD to enter S4BIOS state
    uint8_t pstateControl;       // Processor performance state control
    uint32_t pm1aEventBlock;     // Power management 1a event register block address
    uint32_t pm1bEventBlock;     // Power management 1b event register block address
    uint32_t pm1aControlBlock;   // Power management 1a control register block address
    uint32_t pm1bControlBlock;   // Power management 1b control register block address
    uint32_t pm2ControlBlock;    // Power management 2 control register block address
    uint32_t pmTimerBlock;       // Power management timer control register block address
    uint32_t gpe0Block;          // General-purpose event 0 register block address
    uint32_t gpe1Block;          // General-purpose event 1 register block address
    uint8_t pm1EventLength;      // Byte length of PM1 event registers
    uint8_t pm1ControlLength;    // Byte length of PM1 control registers
    uint8_t pm2ControlLength;    // Byte length of PM2 control registers
    uint8_t pmTimerLength;       // Byte length of PM timer register
    uint8_t gpe0Length;          // Byte length of GPE0 registers
    uint8_t gpe1Length;          // Byte length of GPE1 registers
    uint8_t gpe1Base;            // GPE1 base offset
    uint8_t cstControl;          // C-state control
    uint16_t c2Latency;          // Worst case C2 latency in microseconds
    uint16_t c3Latency;          // Worst case C3 latency in microseconds
    uint16_t flushSize;          // Size of area flushed by FLSH instruction in 64-byte increments
    uint16_t flushStride;        // Number of 64-byte cache lines flushed by FLSH instruction
    uint8_t dutyOffset;          // Duty cycle index in processor P_CNT register
    uint8_t dutyWidth;           // Duty cycle bit width in processor P_CNT register
    uint8_t dayAlarm;            // RTC day alarm index in RTC alarm register
    uint8_t monthAlarm;          // RTC month alarm index in RTC alarm register
    uint8_t century;             // RTC century index in RTC alarm register
    uint16_t bootArchitectureFlags; // Boot architecture flags
    uint8_t reserved2;           // Reserved (must be 0)
    uint32_t flags;              // Fixed feature flags
    // Additional fields for 64-bit addresses in ACPI 2.0+
    uint64_t resetReg;           // 64-bit address of the reset register
    uint8_t resetValue;          // Value to write to the reset register
    uint16_t armBootArchitectureFlags; // ARM Boot Architecture Flags
    uint8_t minorVersion;        // Minor version number
    uint64_t xFirmwareControl;   // 64-bit physical address of FACS
    uint64_t xDsdt;              // 64-bit physical address of DSDT
    uint64_t xPm1aEventBlock;    // 64-bit physical address of PM1a event register block
    uint64_t xPm1bEventBlock;    // 64-bit physical address of PM1b event register block
    uint64_t xPm1aControlBlock;  // 64-bit physical address of PM1a control register block
    uint64_t xPm1bControlBlock;  // 64-bit physical address of PM1b control register block
    uint64_t xPm2ControlBlock;   // 64-bit physical address of PM2 control register block
    uint64_t xPmTimerBlock;      // 64-bit physical address of PM timer register block
    uint64_t xGpe0Block;         // 64-bit physical address of GPE0 register block
    uint64_t xGpe1Block;         // 64-bit physical address of GPE1 register block
} PACKED AcpiFadt;

// ------------------------------------------------------------------------------------------------
typedef struct AcpiMadt
{
    AcpiHeader header;           // Common ACPI table header
    uint32_t localApicAddr;      // Physical address of local APIC
    uint32_t flags;              // Multiple APIC flags
    // Followed by a variable number of APIC structures (type-length encoded)
} PACKED AcpiMadt;

// ------------------------------------------------------------------------------------------------
typedef struct ApicHeader
{
    uint8_t type;                // Type of APIC structure
    uint8_t length;              // Length of the APIC structure
} PACKED ApicHeader;

// ------------------------------------------------------------------------------------------------
typedef struct ApicLocalApic
{
    ApicHeader header;           // Common APIC header
    uint8_t acpiProcessorId;     // ACPI processor ID
    uint8_t apicId;              // APIC ID
    uint32_t flags;              // Flags
} PACKED ApicLocalApic;

// ------------------------------------------------------------------------------------------------
typedef struct ApicIoApic
{
    ApicHeader header;           // Common APIC header
    uint8_t ioApicId;            // I/O APIC ID
    uint8_t reserved;            // Reserved (must be 0)
    uint32_t ioApicAddress;      // Physical address of I/O APIC
    uint32_t globalSystemInterruptBase; // Global system interrupt base
} PACKED ApicIoApic;

// ------------------------------------------------------------------------------------------------
typedef struct ApicInterruptOverride
{
    ApicHeader header;           // Common APIC header
    uint8_t bus;                 // Bus source
    uint8_t source;              // IRQ source
    uint32_t interrupt;          // Global system interrupt
    uint16_t flags;              // Flags
} PACKED ApicInterruptOverride;

// ------------------------------------------------------------------------------------------------
typedef struct ApicNmiSource
{
    ApicHeader header;           // Common APIC header
    uint16_t flags;              // Flags
    uint32_t globalSystemInterrupt; // Global system interrupt
} PACKED ApicNmiSource;

// ------------------------------------------------------------------------------------------------
typedef struct ApicLocalApicNmi
{
    ApicHeader header;           // Common APIC header
    uint8_t acpiProcessorId;     // ACPI processor ID
    uint16_t flags;              // Flags
    uint8_t localApicLint;       // Local APIC LINT input
} PACKED ApicLocalApicNmi;

// ------------------------------------------------------------------------------------------------
typedef struct ApicLocalApicOverride
{
    ApicHeader header;           // Common APIC header
    uint16_t reserved;           // Reserved (must be 0)
    uint64_t localApicAddr;      // 64-bit physical address of local APIC
} PACKED ApicLocalApicOverride;

// ------------------------------------------------------------------------------------------------
static AcpiMadt *s_madt;


// APIC structure types
#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2
// ------------------------------------------------------------------------------------------------
static void AcpiParseFacp(AcpiFadt *facp)
{
    if (facp->smiCommandPort)
    {
        ////printf("Enabling ACPI\n");
        //IoWrite8(facp->smiCommandPort, facp->acpiEnable);

        // TODO - wait for SCI_EN bit
    }
    else
    {
        //printf("ACPI already enabled\n");
    }
}

// ------------------------------------------------------------------------------------------------
static void AcpiParseApic(AcpiMadt *madt)
{
    s_madt = madt;

    //printf("Local APIC Address = 0x%08x\n", madt->localApicAddr);
    g_localApicAddr = (uint8_t *)(uintptr_t)madt->localApicAddr;

    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    while (p < end)
    {
        ApicHeader *header = (ApicHeader *)p;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == APIC_TYPE_LOCAL_APIC)
        {
            ApicLocalApic *s = (ApicLocalApic *)p;

            //printf("Found CPU: %d %d %x\n", s->acpiProcessorId, s->apicId, s->flags);
            if (g_acpiCpuCount < MAX_CPU_COUNT)
            {
                g_acpiCpuIds[g_acpiCpuCount] = s->apicId;
                ++g_acpiCpuCount;
            }
        }
        else if (type == APIC_TYPE_IO_APIC)
        {
            ApicIoApic *s = (ApicIoApic *)p;

            //printf("Found I/O APIC: %d 0x%08x %d\n", s->ioApicId, s->ioApicAddress, s->globalSystemInterruptBase);
            g_ioApicAddr = (uint8_t *)(uintptr_t)s->ioApicAddress;
        }
        else if (type == APIC_TYPE_INTERRUPT_OVERRIDE)
        {
            ApicInterruptOverride *s = (ApicInterruptOverride *)p;

            //printf("Found Interrupt Override: %d %d %d 0x%04x\n", s->bus, s->source, s->interrupt, s->flags);
        }
        else
        {
            //printf("Unknown APIC structure %d\n", type);
        }

        p += length;
    }
}

// ------------------------------------------------------------------------------------------------
static void AcpiParseDT(AcpiHeader *header)
{
    uint32_t signature = header->signature;

    char sigStr[5];
    memcpy(sigStr, &signature, 4);
    sigStr[4] = 0;
    //printf("%s 0x%x\n", sigStr, signature);

    if (signature == 0x50434146)
    {
        AcpiParseFacp((AcpiFadt *)header);
    }
    else if (signature == 0x43495041)
    {
        AcpiParseApic((AcpiMadt *)header);
    }
}

// ------------------------------------------------------------------------------------------------
static void AcpiParseRsdt(AcpiHeader *rsdt)
{
    uint32_t *p = (uint32_t *)(rsdt + 1);
    uint32_t *end = (uint32_t *)((uint8_t*)rsdt + rsdt->length);

    while (p < end)
    {
        uint32_t address = *p++;
        AcpiParseDT((AcpiHeader *)(uintptr_t)address);
    }
}

// ------------------------------------------------------------------------------------------------
static void AcpiParseXsdt(AcpiHeader *xsdt)
{
    uint64_t *p = (uint64_t *)(xsdt + 1);
    uint64_t *end = (uint64_t *)((uint8_t*)xsdt + xsdt->length);

    while (p < end)
    {
        uint64_t address = *p++;
        AcpiParseDT((AcpiHeader *)(uintptr_t)address);
    }
}

// ------------------------------------------------------------------------------------------------
static bool AcpiParseRsdp(uint8_t *p)
{
    // Parse Root System Description Pointer
    //printf("RSDP found\n");

    // Verify checksum
    uint8_t sum = 0;
    for (unsigned int i = 0; i < 20; ++i)
    {
        sum += p[i];
    }

    if (sum)
    {
        //printf("Checksum failed\n");
        return false;
    }

    // Print OEM
    char oem[7];
    memcpy(oem, p + 9, 6);
    oem[6] = '\0';
    //printf("OEM = %s\n", oem);

    // Check version
    uint8_t revision = p[15];
    if (revision == 0)
    {
        //printf("Version 1\n");

        uint32_t rsdtAddr = *(uint32_t *)(p + 16);
        AcpiParseRsdt((AcpiHeader *)(uintptr_t)rsdtAddr);
    }
    else if (revision == 2)
    {
        //printf("Version 2\n");

        uint32_t rsdtAddr = *(uint32_t *)(p + 16);
        uint64_t xsdtAddr = *(uint64_t *)(p + 24);

        if (xsdtAddr)
        {
            AcpiParseXsdt((AcpiHeader *)(uintptr_t)xsdtAddr);
        }
        else
        {
            AcpiParseRsdt((AcpiHeader *)(uintptr_t)rsdtAddr);
        }
    }
    else
    {
        //printf("Unsupported ACPI version %d\n", revision);
    }

    return true;
}

// ------------------------------------------------------------------------------------------------
void AcpiInit()
{
    // TODO - Search Extended BIOS Area

    // Search main BIOS area below 1MB
    uint8_t *p = (uint8_t *)0x000e0000;
    uint8_t *end = (uint8_t *)0x000fffff;

    while (p < end)
    {
        uint64_t signature = *(uint64_t *)p;

        if (signature == 0x2052545020445352) // 'RSD PTR '
        {
            if (AcpiParseRsdp(p))
            {
                //printf("Found Rsdp\n");
                break;
            }
        }

        p += 16;
    }
}

// ------------------------------------------------------------------------------------------------
unsigned int AcpiRemapIrq(unsigned int irq)
{
    AcpiMadt *madt = s_madt;

    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    while (p < end)
    {
        ApicHeader *header = (ApicHeader *)p;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == APIC_TYPE_INTERRUPT_OVERRIDE)
        {
            ApicInterruptOverride *s = (ApicInterruptOverride *)p;

            if (s->source == irq)
            {
                return s->interrupt;
            }
        }

        p += length;
    }

    return irq;
}