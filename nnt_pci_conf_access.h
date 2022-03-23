#ifndef NNT_PCICONF_H
#define NNT_PCICONF_H

#include "nnt_device_defs.h"
#include "nnt_ioctl_defs.h"


int read_pciconf(struct nnt_device* nnt_device, struct rw_operation* read_operation);
int write_pciconf(struct nnt_device* nnt_device, struct rw_operation* write_operation);
int init_pciconf(unsigned int command, void* user_buffer,
                 struct nnt_device* nnt_device);
int read_dword(struct read_dword_from_config_space* read_from_cspace, struct nnt_device* nnt_device);
int check_address_space_support(struct nnt_device* nnt_device);

#endif
