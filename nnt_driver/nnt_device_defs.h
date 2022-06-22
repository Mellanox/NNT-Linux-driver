#ifndef NNT_DEVICE_DEFS_H
#define NNT_DEVICE_DEFS_H


#include <linux/pci.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include "nnt_ioctl_defs.h"

#define NNT_DEVICE_ID_OFFSET        0xf0014
#define NNT_WO_REG_ADDR_DATA        0xbadacce5
#define NNT_NAME_SIZE               75
#define NNT_CONF_ADDRES_REGISETER	88
#define NNT_CONF_DATA_REGISTER		92
#define PCI_SEMAPHORE_OFFSET        0x0c
#define PCI_ADDRESS_OFFSET          0x10
#define PCI_DATA_OFFSET             0x14
#define NNT_MEMORY_SIZE				1024 * 1024
#define VSEC_CAPABILITY_ADDRESS     0x9

#define NNTFLINT_PCICONF_DEVICE_NAME    "nntconf"
#define NNTFLINT_MEMORY_DEVICE_NAME     "nntcr"
#define MFT_PCICONF_DEVICE_NAME         "pciconf"    
#define MFT_MEMORY_DEVICE_NAME          "pci_cr"

#define MST_BC_BUFFER_SIZE          256
#define MST_BC_MAX_MINOR            256

static struct pci_device_id pci_conf_devices[];
static struct pci_device_id livefish_pci_devices[];
static struct pci_device_id bar_pci_devices[];

struct nnt_dma_page {
    struct page** page_list;
    dma_addr_t* dma_address_list;
};


enum nnt_device_type {
    NNT_PCICONF,
    NNT_PCICONF_NO_VSEC,
    NNT_PCI_MEMORY
};


struct supported_address_space {
    int cr_space;
    int icmd;
    int semaphore;
};

/* Forward declaration */
struct nnt_device;
typedef int (*callback_read) (struct nnt_device* nnt_device, struct nnt_rw_operation* read_operation);
typedef int (*callback_write)(struct nnt_device* nnt_device, struct nnt_rw_operation* write_operation);
typedef int (*callback_init)(void* user_buffer, struct nnt_device* nnt_device);

struct nnt_access_callback {
    callback_read read;
    callback_write write;
    callback_init init;
};


struct nnt_device_pciconf {
    struct supported_address_space address_space;
    unsigned int address_register;
    unsigned int data_register;
    unsigned int semaphore_offset;
    unsigned int data_offset;
    unsigned int address_offset;
    unsigned int vendor_specific_capability;
    unsigned int vsec_capability_mask;
    unsigned int vsec_fully_supported;
};


struct nnt_device_pci {
    unsigned int bar_address;
    unsigned int bar_size;
};


struct nnt_device_memory {
    unsigned int pci_memory_bar_address;
	unsigned int connectx_wa_slot_p1;
    void* hardware_memory_address;
};


struct nnt_device {
    struct nnt_device_pciconf pciconf_device;
    struct nnt_device_memory memory_device;
    struct nnt_access_callback access;
    struct nnt_device_pci device_pci;
    struct pci_dev* pci_device;
    struct nnt_dma_page dma_page;
    struct list_head entry;
    struct cdev mcdev;
    struct mutex lock;
    enum nnt_device_type device_type;
    int vpd_capability_address;
    int wo_address;
    int buffer_used_bc;
    char device_name[NNT_NAME_SIZE];
    char buffer_bc[MST_BC_BUFFER_SIZE];
    dev_t device_number;
};


#endif
