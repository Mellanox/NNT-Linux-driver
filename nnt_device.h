#ifndef MST_DEVICE_H
#define MST_DEVICE_H


#include <linux/pci.h>
#include "mst_device_defs.h"


int create_mst_devices(int contiguous_device_numbers, dev_t device_number,
                       int is_mft_package, struct file_operations* fop);
void destroy_mst_devices(void);
int is_memory_device(struct pci_dev* pdev);
int is_pciconf_device(struct pci_dev* pdev);
int get_amount_of_nvidia_devices(void);
int set_private_data(struct inode* inode, struct file* file);
int get_mst_device(struct file* file, struct mst_device** mst_device);
int mutex_lock_mst(struct file* file);
void mutex_unlock_mst(struct file* file);

#endif
