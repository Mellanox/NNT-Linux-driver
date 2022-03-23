#ifndef NNT_IOCTL_H
#define NNT_IOCTL_H

#include "nnt_device_defs.h"

int dma_pages_ioctl(unsigned int command, void* user_buffer,
                    struct nnt_device* nnt_device);
int read_dword_ioctl(unsigned int command, void* user_buffer,
                     struct nnt_device* nnt_device);
int get_nnt_device_parameters(unsigned int command, void* user_buffer,
                              struct nnt_device* nnt_device);
int pci_connectx_wa(unsigned int command, void* user_buffer,
                    struct nnt_device* nnt_device);
int vpd_read(unsigned int command, struct nnt_vpd* vpd,
             struct nnt_device* nnt_device);
int vpd_write(unsigned int command, struct nnt_vpd* vpd,
              struct nnt_device* nnt_device);
#endif
