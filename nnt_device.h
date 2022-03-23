#ifndef NNT_DEVICE_H
#define NNT_DEVICE_H


#include <linux/pci.h>
#include "nnt_device_defs.h"


int create_nnt_devices(int contiguous_device_numbers, dev_t device_number,
                       int is_mft_package, struct file_operations* fop);
void destroy_nnt_devices(void);
int is_memory_device(struct pci_dev* pdev);
int is_pciconf_device(struct pci_dev* pdev);
int get_amount_of_nvidia_devices(void);
int set_private_data(struct inode* inode, struct file* file);
int get_nnt_device(struct file* file, struct nnt_device** nnt_device);
int mutex_lock_nnt(struct file* file);
void mutex_unlock_nnt(struct file* file);

#endif
