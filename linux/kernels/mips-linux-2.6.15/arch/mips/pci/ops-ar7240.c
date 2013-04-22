#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include "ar7240.h"
/*
 * PCI cfg an I/O routines are done by programming a 
 * command/byte enable register, and then read/writing
 * the data from a data regsiter. We need to ensure
 * these transactions are atomic or we will end up
 * with corrupt data on the bus or in a driver.
 */
static DEFINE_SPINLOCK(ar7100_pci_lock);

int
ar7240_local_read_config(int where, int size, uint32_t *value)
{
    unsigned long flags, addr, tval, mask;

    /* Make sure the address is aligned to natural boundary */
    if(where & (size - 1))
        return PCIBIOS_BAD_REGISTER_NUMBER;

    spin_lock_irqsave(&ar7100_pci_lock, flags);
    switch (size) {
        case 1:
            addr = where & ~3;
            mask = 0xff000000 >> ((where % 4) * 8);
            tval = ar7240_reg_rd(AR7240_PCI_CRP + addr);
            tval = tval & ~mask;
            *value = (tval >> ((4 - (where % 4))*8));
            break;
        case 2:
            addr = where & ~3;
            mask = 0xffff0000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(AR7240_PCI_CRP + addr);
            tval = tval & ~mask;
            *value = (tval >> ((4 - (where % 4))*8));
            break;
        case 4:
            *value = ar7240_reg_rd(AR7240_PCI_CRP + where);
            break;
        default:
            spin_unlock_irqrestore(&ar7100_pci_lock, flags);
            return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

int
ar7240_local_write_config(int where, int size, uint32_t value) 
{
    unsigned long flags, addr, tval, mask;

    /* Make sure the address is aligned to natural boundary */
    if(where & (size - 1))
        return PCIBIOS_BAD_REGISTER_NUMBER;

    spin_lock_irqsave(&ar7100_pci_lock, flags);
    switch (size) {
        case 1:
            addr = (AR7240_PCI_CRP + where) & ~3;
            mask = 0xff000000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(addr);
            tval = tval & ~mask;
            tval |= (value << ((4 - (where % 4))*8)) & mask;
            ar7240_reg_wr(addr,tval);
            break;
        case 2:
            addr = (AR7240_PCI_CRP + where) & ~3;
            mask = 0xffff0000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(addr);
            tval = tval & ~mask;
            tval |= (value << ((4 - (where % 4))*8)) & mask;
            ar7240_reg_wr(addr,tval);
            break;
        case 4:
            ar7240_reg_wr((AR7240_PCI_CRP + where),value);
            break;
        default:
            spin_unlock_irqrestore(&ar7100_pci_lock, flags);
            return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

static int 
ar7240_pci_read_config(struct pci_bus *bus, unsigned int devfn, int where,
                       int size, uint32_t *value)
{
    unsigned long flags, addr, tval, mask;
	if(devfn)
        return PCIBIOS_DEVICE_NOT_FOUND;

    /* Make sure the address is aligned to natural boundary */
    if(where & (size - 1))
        return PCIBIOS_BAD_REGISTER_NUMBER;

    spin_lock_irqsave(&ar7100_pci_lock, flags);
    switch (size) {
        case 1:
            addr = where & ~3;
            mask = 0xff000000 >> ((where % 4) * 8);
            tval = ar7240_reg_rd(AR7240_PCI_DEV_CFGBASE + addr);
            tval = tval & ~mask;
            *value = (tval >> ((4 - (where % 4))*8));
            break;
        case 2:
            addr = where & ~3;
            mask = 0xffff0000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(AR7240_PCI_DEV_CFGBASE + addr);
            tval = tval & ~mask;
            *value = (tval >> ((4 - (where % 4))*8));
            break;
        case 4:
            *value = ar7240_reg_rd(AR7240_PCI_DEV_CFGBASE + where);
            /*
             * WAR for BAR issue - We are unable to access the PCI device spac 
             * if we set the BAR with proper base address 
             */
            if(where == 0x10)
                ar7240_reg_wr((AR7240_PCI_DEV_CFGBASE + where),0xffff);
            break;
        default:
            spin_unlock_irqrestore(&ar7100_pci_lock, flags);
            return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

static int 
ar7240_pci_write_config(struct pci_bus *bus,  unsigned int devfn, int where, 
                        int size, uint32_t value)
{
    uint32_t flags, tval, addr, mask;

	if(devfn)
           return PCIBIOS_DEVICE_NOT_FOUND;

    /* Make sure the address is aligned to natural boundary */
    if(where & (size - 1))
        return PCIBIOS_BAD_REGISTER_NUMBER;

    spin_lock_irqsave(&ar7100_pci_lock, flags);
    switch (size) {
        case 1:
            addr = (AR7240_PCI_DEV_CFGBASE + where) & ~3;
            mask = 0xff000000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(addr);
            tval = tval & ~mask;
            tval |= (value << ((4 - (where % 4))*8)) & mask;
            ar7240_reg_wr(addr,tval);
            break;
        case 2:
            addr = (AR7240_PCI_DEV_CFGBASE + where) & ~3;
            mask = 0xffff0000 >> ((where % 4)*8);
            tval = ar7240_reg_rd(addr);
            tval = tval & ~mask;
            tval |= (value << ((4 - (where % 4))*8)) & mask;
            ar7240_reg_wr(addr,tval);
            break;
        case 4:
            ar7240_reg_wr((AR7240_PCI_DEV_CFGBASE + where),value);
            break;
        default:
            spin_unlock_irqrestore(&ar7100_pci_lock, flags);
            return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    spin_unlock_irqrestore(&ar7100_pci_lock, flags);
    return PCIBIOS_SUCCESSFUL;
}

struct pci_ops ar7240_pci_ops = {
	.read =  ar7240_pci_read_config,
	.write = ar7240_pci_write_config,
};
