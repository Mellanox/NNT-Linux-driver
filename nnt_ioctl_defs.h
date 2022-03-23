#ifndef NNT_IOCTL_DEFS_H
#define NNT_IOCTL_DEFS_H

#define NNT_MAGIC_NUMBER                    0xD3
#define MAX_BUFFER_BLOCK_SIZE               256
#define NNT_MAX_PAGES_SIZE                  32
#define NNT_CONNECTX_WA_SIZE                3

#define NNT_WRITE                           _IOW (NNT_MAGIC_NUMBER, 1, struct rw_operation)
#define NNT_READ                            _IOW (NNT_MAGIC_NUMBER, 2, struct rw_operation)
#define NNT_GET_DMA_PAGES                   _IOR (NNT_MAGIC_NUMBER, 3, struct page_info)
#define NNT_RELEASE_DMA_PAGES               _IOR (NNT_MAGIC_NUMBER, 4, struct page_info)
#define NNT_READ_DWORD_FROM_CONFIG_SPACE    _IOR (NNT_MAGIC_NUMBER, 5, struct read_dword_from_config_space)
#define NNT_GET_DEVICE_PARAMETERS           _IOR (NNT_MAGIC_NUMBER, 6, struct device_parameters)
#define NNT_INIT                            _IOR (NNT_MAGIC_NUMBER, 7, struct pciconf_init)
#define NNT_PCI_CONNECTX_WA                 _IOR (NNT_MAGIC_NUMBER, 8, u_int32_t)
#define NNT_VPD_READ                        _IOR (NNT_MAGIC_NUMBER, 9, struct nnt_vpd)
#define NNT_VPD_WRITE                       _IOW (NNT_MAGIC_NUMBER, 10, struct nnt_vpd)



struct nnt_vpd {
	unsigned int offset;
	unsigned int timeout;
	unsigned int data;
};


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
    struct page_address page_address_array[NNT_MAX_PAGES_SIZE];
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


struct nnt_connectx_wa {
	unsigned int connectx_wa_slot_p1;
};

#endif
