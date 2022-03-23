#ifndef NNT_PCICONF_NO_VSEC_H
#define NNT_PCICONF_NO_VSEC_H

#include "nnt_device_defs.h"
#include "nnt_ioctl_defs.h"


int read_pciconf_no_full_vsec(struct nnt_device* nnt_device, struct rw_operation* read_operation);
int write_pciconf_no_full_vsec(struct nnt_device* nnt_device, struct rw_operation* write_operation);
int init_pciconf_no_full_vsec(unsigned int command, void* user_buffer,
                              struct nnt_device* nnt_device);

#endif
