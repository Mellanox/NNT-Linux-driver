#ifndef MST_IOCTL_H
#define MST_IOCTL_H

#include "mst_device_defs.h"

int dma_pages_ioctl(unsigned int command, void* user_buffer,
                    struct mst_device* mst_device);
int read_dword_ioctl(unsigned int command, void* user_buffer,
                     struct mst_device* mst_device);
int get_mst_device_parameters(unsigned int command, void* user_buffer,
                              struct mst_device* mst_device);
int pci_connectx_wa(unsigned int command, void* user_buffer,
                    struct mst_device* mst_device);
#endif
