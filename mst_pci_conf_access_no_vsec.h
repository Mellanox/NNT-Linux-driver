#ifndef MST_PCICONF_NO_VSEC_H
#define MST_PCICONF_NO_VSEC_H

#include "mst_device_defs.h"
#include "mst_ioctl_defs.h"


int read_pciconf_no_full_vsec(struct mst_device* mst_device, struct rw_operation* read_operation);
int write_pciconf_no_full_vsec(struct mst_device* mst_device, struct rw_operation* write_operation);
int init_pciconf_no_full_vsec(unsigned int command, void* user_buffer,
                              struct mst_device* mst_device);

#endif
