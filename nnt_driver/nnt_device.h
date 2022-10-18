#ifndef NNT_DEVICE_H
#define NNT_DEVICE_H


#include <linux/pci.h>
#include "nnt_device_defs.h"


int create_nnt_devices(dev_t device_number, int is_alloc_chrdev_region,
                       struct file_operations* fop, int nnt_device_flag);
void destroy_nnt_devices(int is_alloc_chrdev_region);
void destroy_nnt_devices_bc(void);
int destroy_nnt_device_bc(struct nnt_device* nnt_device);
int is_memory_device(struct pci_dev* pdev);
int is_pciconf_device(struct pci_dev* pdev);
int get_amount_of_nvidia_devices(void);
int set_private_data(struct file* file);
void set_private_data_open(struct file* file);
int set_private_data_bc(struct file* file, unsigned int bus,
                        unsigned int devfn, unsigned int domain);
int get_nnt_device(struct file* file, struct nnt_device** nnt_device);
int mutex_lock_nnt(struct file* file);
void mutex_unlock_nnt(struct file* file);

#endif
