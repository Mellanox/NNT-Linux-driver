#ifndef MST_PCICONF_H
#define MST_PCICONF_H

#include "mst_device_defs.h"
#include "mst_ioctl_defs.h"


int read_pciconf(struct mst_device* mst_device, struct rw_operation* read_operation);
int write_pciconf(struct mst_device* mst_device, struct rw_operation* write_operation);
int init_pciconf(unsigned int command, void* user_buffer,
                 struct mst_device* mst_device);
int read_dword(struct read_dword_from_config_space* read_from_cspace, struct mst_device* mst_device);
int check_address_space_support(struct mst_device* mst_device);

#endif
