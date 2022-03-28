#include "nnt_device.h"
#include "nnt_device_defs.h"
#include "nnt_defs.h"
#include "nnt_pci_conf_access.h"
#include "nnt_pci_conf_access_no_vsec.h"
#include "nnt_memory_access.h"
#include "nnt_device_list.h"
#include <linux/module.h>


MODULE_LICENSE("GPL");

/* Device list to check if device is available
     since it could be removed by hotplug event. */
LIST_HEAD(nnt_device_list);


int is_wo_gw(struct nnt_device* nnt_device)
{
	unsigned int data = 0;
    int error;

    error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.address_register,
                                   NNT_DEVICE_ID_OFFSET);
    CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.address_register,
                          NNT_DEVICE_ID_OFFSET);

	/* Read the result from data register */
    error = pci_read_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.address_register,
                                  &data);
    CHECK_PCI_READ_ERROR(error, nnt_device->pciconf_device.address_register);

	if (data == NNT_WO_REG_ADDR_DATA) {
		    error = 1;
    }

ReturnOnFinished:
	return error;
}


int get_nnt_device(struct file* file, struct nnt_device** nnt_device)
{
    int error_code = 0;
    *nnt_device = file->private_data;

    if (!nnt_device) {
            error_code = -EINVAL;
    }

    return error_code;
}


int create_file_name_nntflint(struct pci_dev* pci_device, struct nnt_device* nnt_dev,
                              enum nnt_device_type device_type)
{
    sprintf(nnt_dev->device_name, "%4.4x:%2.2x:%2.2x.%1.1x_%s",
            pci_domain_nr(pci_device->bus), pci_device->bus->number,
            PCI_SLOT(pci_device->devfn), PCI_FUNC(pci_device->devfn),
            (device_type == NNT_PCICONF) ? NNTFLINT_PCICONF_DEVICE_NAME : NNTFLINT_MEMORY_DEVICE_NAME);

    nnt_error("NNTFlint device name created: id: %d, slot id: %d, device name: %s\n",pci_device->device,
              PCI_FUNC(pci_device->devfn), nnt_dev->device_name);

    return 0;
}



int create_file_name_mft(struct pci_dev* pci_device, struct nnt_device* nnt_dev,
                         enum nnt_device_type device_type)
{
    sprintf(nnt_dev->device_name, "/nnt/mt%d_pci_%s%1.1x",
            pci_device->device,
            (device_type == NNT_PCICONF) ? MFT_PCICONF_DEVICE_NAME : MFT_MEMORY_DEVICE_NAME,
            PCI_FUNC(pci_device->devfn));

    nnt_error("MFT device name created: id: %d, slot id: %d, device name: %s\n",pci_device->device,
              PCI_FUNC(pci_device->devfn), nnt_dev->device_name);

    return 0;
}


int nnt_device_structure_init(struct nnt_device** nnt_device)
{
    /* Allocate nnt device structure. */
    *nnt_device =
            kzalloc(sizeof(struct nnt_device),GFP_KERNEL);

    if (!(*nnt_device)) {
            return -ENOMEM;
    }

    /* initialize nnt structure. */
    memset(*nnt_device, 0, sizeof(struct nnt_device));

    return 0;
}




int create_nnt_device(struct pci_dev* pci_device, enum nnt_device_type device_type,
                      int is_mft_package)
{
    struct nnt_device* nnt_device = NULL;
    int error_code = 0;

    /* Allocate nnt device info structure. */
    if((error_code =
            nnt_device_structure_init(&nnt_device)) != 0)
            goto ReturnOnError;

    if (is_mft_package) {
            /* Build the device file name of MFT. */
            if((error_code =
                    create_file_name_mft(pci_device, nnt_device,
                                         device_type)) != 0)
                    goto ReturnOnError;
    } else {
            /* Build the device file name of NNTFlint. */
            if((error_code =
                    create_file_name_nntflint(pci_device, nnt_device,
                                              device_type)) != 0)
                    goto ReturnOnError;
    }

    nnt_device->pci_device = pci_device;
    nnt_device->device_type = device_type;

    /* Add the nnt device structure to the list. */
    list_add_tail(&nnt_device->entry, &nnt_device_list);

    return error_code;

ReturnOnError:
    if (nnt_device) {
	        kfree(nnt_device);
    }

    return error_code;
}




int is_pciconf_device(struct pci_dev* pci_device)
{
    return (pci_match_id(pciconf_devices, pci_device) ||
            pci_match_id(livefish_pci_devices, pci_device));
}




int is_memory_device(struct pci_dev* pci_device)
{
    return (pci_match_id(bar_pci_devices, pci_device) &&
            !pci_match_id(livefish_pci_devices, pci_device));
}




int create_devices(int contiguous_device_numbers, dev_t device_number,
                   struct file_operations* fop)
{
    struct nnt_device* current_nnt_device;
	struct nnt_device* temp_nnt_device;
    int major = MAJOR(device_number);
    int error_code;
    int minor = 0;
    int count = 1;

    /* Create necessary number of the devices. */
    list_for_each_entry_safe(current_nnt_device, temp_nnt_device,
                             &nnt_device_list, entry) {
            // Create device with a new minor number.
            current_nnt_device->device_number = MKDEV(major, minor);

            /* Init new device. */
            current_nnt_device->mcdev.owner = THIS_MODULE;
            cdev_init(&current_nnt_device->mcdev, fop);
            mutex_init(&current_nnt_device->lock);

            /* Add device to the system. */
            if((error_code =
                cdev_add(&current_nnt_device->mcdev, current_nnt_device->device_number,
                         count)) != 0) {
                return error_code;
            }

            /* Create device node. */
            if (device_create(nnt_driver_info.class_driver, NULL,
                              current_nnt_device->device_number, NULL,
                              current_nnt_device->device_name) == NULL) {
                nnt_error("Device creation failed\n");
                return -1;
            }

            current_nnt_device->pciconf_device.vendor_specific_capability =
                    pci_find_capability(current_nnt_device->pci_device, VSEC_CAPABILITY_ADDRESS);
            current_nnt_device->vpd_capability_address = pci_find_capability(current_nnt_device->pci_device, PCI_CAP_ID_VPD);

            if (!current_nnt_device->pciconf_device.vendor_specific_capability) {
                    current_nnt_device->device_type = NNT_PCICONF_NO_FULL_VSEC;
            }
            switch (current_nnt_device->device_type) {
                    case NNT_PCICONF:
                            current_nnt_device->access.read = read_pciconf;
                            current_nnt_device->access.write = write_pciconf;
                            current_nnt_device->access.init = init_pciconf;
                            current_nnt_device->pciconf_device.semaphore_offset =
                                    current_nnt_device->pciconf_device.vendor_specific_capability + PCI_SEMAPHORE_OFFSET;
                            current_nnt_device->pciconf_device.data_offset =
                                    current_nnt_device->pciconf_device.vendor_specific_capability + PCI_DATA_OFFSET;
                            current_nnt_device->pciconf_device.address_offset =
                                    current_nnt_device->pciconf_device.vendor_specific_capability + PCI_ADDRESS_OFFSET;
                            break;

                    case NNT_PCICONF_NO_FULL_VSEC:
                            current_nnt_device->access.read = read_pciconf_no_full_vsec;
                            current_nnt_device->access.write = write_pciconf_no_full_vsec;
                            current_nnt_device->access.init = init_pciconf_no_full_vsec;
                            current_nnt_device->pciconf_device.address_register = NNT_CONF_ADDRES_REGISETER;
                            current_nnt_device->pciconf_device.data_register = NNT_CONF_DATA_REGISTER;
                            current_nnt_device->wo_address = is_wo_gw(current_nnt_device);
                            break;

                    case NNT_PCI_MEMORY:
                            current_nnt_device->access.read = read_memory;
                            current_nnt_device->access.write = write_memory;
                            current_nnt_device->access.init = init_memory;
                            current_nnt_device->memory_device.connectx_wa_slot_p1 = 0;
                            current_nnt_device->memory_device.hardware_memory_address =
                                    ioremap(pci_resource_start(current_nnt_device->pci_device,
                                                               current_nnt_device->memory_device.pci_memory_bar_address),
                                                               NNT_MEMORY_SIZE);

                            if (current_nnt_device->memory_device.hardware_memory_address <= 0) {
                                    nnt_error("could not map device memory\n");
                            }

                            break;
            }

            minor++;
    }

    return 0;
}




int create_nnt_devices(int contiguous_device_numbers, dev_t device_number,
                       int is_mft_package, struct file_operations* fop)
{
    struct pci_dev* pci_device = NULL;
    int error_code = 0;

    /* Find all Nvidia PCI devices. */
    while ((pci_device = pci_get_device(NNT_NVIDIA_PCI_VENDOR, PCI_ANY_ID,
                                        pci_device)) != NULL) {
            /* Create pciconf device. */
            if (is_pciconf_device(pci_device)) {
                    if ((error_code =
                            create_nnt_device(pci_device, NNT_PCICONF,
                                              is_mft_package)) != 0) {
                        nnt_error("Failed to create pci conf device\n");
                        goto ReturnOnFinished;
                    }
            }

            /* Create pci memory device. */
            if (is_memory_device(pci_device)) {
                    if ((error_code =
                            create_nnt_device(pci_device, NNT_PCI_MEMORY,
                                              is_mft_package)) != 0) {
                        nnt_error("Failed to create pci conf device\n");
                        goto ReturnOnFinished;
                    }
            }
    }

    /* Create the devices. */
    if((error_code =
            create_devices(contiguous_device_numbers, device_number,
                           fop)) != 0) {
            return error_code;
    }

    nnt_debug("Device found, id: %d, slot id: %d\n",pci_device->device, PCI_FUNC(pci_device->devfn));

ReturnOnFinished:
    return error_code;
}




int get_amount_of_nvidia_devices(void)
{
    int contiguous_device_numbers = 0;
    struct pci_dev* pci_device = NULL;

    /* Find all Nvidia PCI devices. */
    while ((pci_device = pci_get_device(NNT_NVIDIA_PCI_VENDOR, PCI_ANY_ID,
                                  pci_device)) != NULL) {
            if (is_memory_device(pci_device) || is_pciconf_device(pci_device)) {
                contiguous_device_numbers++;
            }
    }

    return contiguous_device_numbers;
}




int set_private_data(struct inode* inode, struct file* file)
{
    struct nnt_device* current_nnt_device = NULL;
    struct nnt_device* temp_nnt_device = NULL;
    int minor = iminor(file_inode(file));

    /* Set private data to nnt structure. */
    list_for_each_entry_safe(current_nnt_device, temp_nnt_device,
                             &nnt_device_list, entry) {
            if (MINOR(current_nnt_device->device_number) == minor) {
                    file->private_data = current_nnt_device;
                    return 0;
            }
    }

    nnt_error("failed to find device with minor=%d\n", minor);

    return -EINVAL;
}




int mutex_lock_nnt(struct file* file)
{
    struct nnt_device* nnt_device;

    if (!file) {
        return 1;
    }

    nnt_device = file->private_data;

    if (!nnt_device) {
        return -EINVAL;
    }

    mutex_lock(&nnt_device->lock);

    return 0;
}



void mutex_unlock_nnt(struct file* file)
{
    struct nnt_device* nnt_device = file->private_data;

    mutex_unlock(&nnt_device->lock);
}




void destroy_nnt_devices(void)
{
    struct nnt_device* current_nnt_device;
    struct nnt_device* temp_nnt_device;

    /* free all nnt_devices */
    list_for_each_entry_safe(current_nnt_device, temp_nnt_device,
                             &nnt_device_list, entry) {
            /* Character device is no longer, it must be properly destroyed. */
            cdev_del(&current_nnt_device->mcdev);
            device_destroy(nnt_driver_info.class_driver, current_nnt_device->device_number);
            list_del(&current_nnt_device->entry);
            kfree(current_nnt_device);
    }
}
