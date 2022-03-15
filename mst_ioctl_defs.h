#ifndef MST_IOCTL_DEFS_H
#define MST_IOCTL_DEFS_H

#define MST_MAGIC_NUMBER                            0xD3
#define MAX_BUFFER_BLOCK_SIZE                       256
#define ADDRESS_SPACE_VENDOR_SPECIFIC_CAPABILITY    0x9
#define ADDRESS_SPACE_ICMD                          0x3
#define ADDRESS_SPACE_CR_SPACE                      0x2
#define ADDRESS_SPACE_SEMAPHORE                     0xa


#define MST_WRITE                           _IOW (MST_MAGIC_NUMBER, 1, struct rw_operation)
#define MST_READ                            _IOW (MST_MAGIC_NUMBER, 2, struct rw_operation)
#define MST_GET_DMA_PAGES                   _IOR (MST_MAGIC_NUMBER, 3, struct page_info)
#define MST_RELEASE_DMA_PAGES               _IOR (MST_MAGIC_NUMBER, 4, struct page_info)
#define MST_READ_DWORD_FROM_CONFIG_SPACE    _IOR (MST_MAGIC_NUMBER, 5, struct read_dword_from_config_space)
#define MST_GET_DEVICE_PARAMETERS           _IOR (MST_MAGIC_NUMBER, 6, struct device_parameters)
#define MST_INIT                            _IOR (MST_MAGIC_NUMBER, 7, struct pciconf_init)
#define MST_PCI_CONNECTX_WA                 _IOR (MST_MAGIC_NUMBER, 8, u_int32_t)


#define MST_MAX_PAGES_SIZE                  32
#define MST_CONNECTX_WA_SIZE                3

struct pciconf_init {
	unsigned int address_register;
	unsigned int address_data_register;
};


struct device_parameters {
	unsigned int domain;
	unsigned int bus;
	unsigned int slot;
	unsigned int func;
	unsigned int pci_memory_bar_address;
	unsigned int device;
	unsigned int vendor;
	unsigned int subsystem_device;
	unsigned int subsystem_vendor;
	unsigned int vendor_specific_capability;
};


struct page_address {
    u_int64_t dma_address;
    u_int64_t virtual_address;
};


struct page_info {
    unsigned int total_pages;
    unsigned long page_pointer_start;
    struct page_address page_address_array[MST_MAX_PAGES_SIZE];
};


struct read_dword_from_config_space {
    unsigned int offset;
    unsigned int data;
};


struct rw_operation {
	unsigned int offset;
	unsigned int size;
	unsigned int data[MAX_BUFFER_BLOCK_SIZE / 4];
    unsigned int address_space;
};


struct mst_connectx_wa {
	unsigned int connectx_wa_slot_p1;
};

#endif
