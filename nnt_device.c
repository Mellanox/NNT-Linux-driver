#include "mst_device.h"
#include "mst_device_defs.h"
#include "mst_defs.h"
#include "mst_pci_conf_access.h"
#include "mst_pci_conf_access_no_vsec.h"
#include "mst_memory_access.h"
#include <linux/module.h>


MODULE_LICENSE("GPL");

/* Device list to check if device is available
     since it could be removed by hotplug event. */
LIST_HEAD(mst_device_list);

static struct pci_device_id pciconf_devices[] = {
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4099) },  /* ConnectX-3       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4103) },  /* ConnectX-3Pro    */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4113) },  /* ConnectX-IB      */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4115) },  /* ConnectX-4       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4117) },  /* ConnectX-4Lx     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4119) },  /* ConnectX-5       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4121) },  /* ConnectX-5EX     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4123) },  /* ConnectX-6       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4125) },  /* ConnectX-6DX     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4127) },  /* ConnectX-6LX     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 41682) }, /* BlueField        */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 41686) }, /* BlueField 2      */
        { 0, }
};


static struct pci_device_id livefish_pci_devices[] = {
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x01f6) }, /* ConnectX-3 Flash Recovery       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x01f8) }, /* ConnectX-3 Pro Flash Recovery   */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x01ff) }, /* ConnectX-IB Flash Recovery      */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x0209) }, /* ConnectX-4 Flash Recovery       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x020b) }, /* ConnectX-4Lx Flash Recovery     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x020d) }, /* ConnectX-5 Flash Recovery       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x020f) }, /* ConnectX-6 Flash Recovery       */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x0212) }, /* ConnectX-6DX Flash Recovery     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x0216) }, /* ConnectX-6LX Flash Recovery     */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 0x0211) }, /* BlueField Flash Recovery        */
        { 0, }
};


static struct pci_device_id bar_pci_devices[] = {
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4099) }, /* ConnectX-3    */
        { PCI_DEVICE(MST_NVIDIA_PCI_VENDOR, 4103) }, /* ConnectX-3Pro */
        { 0, }
};


int is_wo_gw(struct mst_device* mst_device)
{
	unsigned int data = 0;
    int error;

    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_register,
                                   MST_DEVICE_ID_OFFSET);
    CHECK_PCI_WRITE_ERROR(error, mst_device->pciconf_device.address_register,
                          MST_DEVICE_ID_OFFSET);

	/* Read the result from data register */
    error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_register,
                                  &data);
    CHECK_PCI_READ_ERROR(error, mst_device->pciconf_device.address_register);

	if (data == MST_WO_REG_ADDR_DATA) {
		    error = 1;
    }

ReturnOnFinished:
	return error;
}


int get_mst_device(struct file* file, struct mst_device** mst_device)
{
    int error_code = 0;
    *mst_device = file->private_data;

    if (!mst_device) {
            error_code = -EINVAL;
    }

    return error_code;
}


int create_file_name_mstflint(struct pci_dev* pci_device, struct mst_device* mst_dev,
                              enum mst_device_type device_type)
{
    sprintf(mst_dev->device_name, "%4.4x:%2.2x:%2.2x.%1.1x_%s",
            pci_domain_nr(pci_device->bus), pci_device->bus->number,
            PCI_SLOT(pci_device->devfn), PCI_FUNC(pci_device->devfn),
            (device_type == MST_PCICONF) ? MSTFLINT_PCICONF_DEVICE_NAME : MSTFLINT_MEMORY_DEVICE_NAME);

    mst_error("MSTFlint device name created: id: %d, slot id: %d, device name: %s\n",pci_device->device,
              PCI_FUNC(pci_device->devfn), mst_dev->device_name);

    return 0;
}



int create_file_name_mft(struct pci_dev* pci_device, struct mst_device* mst_dev,
                         enum mst_device_type device_type)
{
    sprintf(mst_dev->device_name, "/mst/mt%d_pci_%s%1.1x",
            pci_device->device,
            (device_type == MST_PCICONF) ? MFT_PCICONF_DEVICE_NAME : MFT_MEMORY_DEVICE_NAME,
            PCI_FUNC(pci_device->devfn));

    mst_error("MFT device name created: id: %d, slot id: %d, device name: %s\n",pci_device->device,
              PCI_FUNC(pci_device->devfn), mst_dev->device_name);

    return 0;
}


int mst_device_structure_init(struct mst_device** mst_device)
{
    /* Allocate mst device structure. */
    *mst_device =
            kzalloc(sizeof(struct mst_device),GFP_KERNEL);

    if (!(*mst_device)) {
            return -ENOMEM;
    }

    /* initialize mst structure. */
    memset(*mst_device, 0, sizeof(struct mst_device));

    return 0;
}




int create_mst_device(struct pci_dev* pci_device, enum mst_device_type device_type,
                      int is_mft_package)
{
    struct mst_device* mst_device = NULL;
    int error_code = 0;

    /* Allocate mst device info structure. */
    if((error_code =
            mst_device_structure_init(&mst_device)) != 0)
            goto ReturnOnError;

    if (is_mft_package) {
            /* Build the device file name of MFT. */
            if((error_code =
                    create_file_name_mft(pci_device, mst_device,
                                         device_type)) != 0)
                    goto ReturnOnError;
    } else {
            /* Build the device file name of MSTFlint. */
            if((error_code =
                    create_file_name_mstflint(pci_device, mst_device,
                                              device_type)) != 0)
                    goto ReturnOnError;
    }

    mst_device->pci_device = pci_device;
    mst_device->device_type = device_type;

    /* Add the mst device structure to the list. */
    list_add_tail(&mst_device->entry, &mst_device_list);

    return error_code;

ReturnOnError:
    if (mst_device) {
	        kfree(mst_device);
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
    struct mst_device* current_mst_device;
	struct mst_device* temp_mst_device;
    int major = MAJOR(device_number);
    int error_code;
    int minor = 0;
    int count = 1;

    /* Create necessary number of the devices. */
    list_for_each_entry_safe(current_mst_device, temp_mst_device,
                             &mst_device_list, entry) {
            // Create device with a new minor number.
            current_mst_device->device_number = MKDEV(major, minor);

            /* Init new device. */
            current_mst_device->mcdev.owner = THIS_MODULE;
            cdev_init(&current_mst_device->mcdev, fop);
            mutex_init(&current_mst_device->lock);

            /* Add device to the system. */
            if((error_code =
                cdev_add(&current_mst_device->mcdev, current_mst_device->device_number,
                         count)) != 0) {
                return error_code;
            }

            /* Create device node. */
            if (device_create(mst_driver_info.class_driver, NULL,
                              current_mst_device->device_number, NULL,
                              current_mst_device->device_name) == NULL) {
                mst_error("Device creation failed\n");
                return -1;
            }

            current_mst_device->pciconf_device.vendor_specific_capability =
                    pci_find_capability(current_mst_device->pci_device, VSEC_CAPABILITY_ADDRESS);
            current_mst_device->vpd_capability_address = pci_find_capability(current_mst_device->pci_device, PCI_CAP_ID_VPD);

            if (!current_mst_device->pciconf_device.vendor_specific_capability) {
                    current_mst_device->device_type = MST_PCICONF_NO_FULL_VSEC;
            }
            switch (current_mst_device->device_type) {
                    case MST_PCICONF:
                            current_mst_device->access.read = read_pciconf;
                            current_mst_device->access.write = write_pciconf;
                            current_mst_device->access.init = init_pciconf;
                            current_mst_device->pciconf_device.semaphore_offset =
                                    current_mst_device->pciconf_device.vendor_specific_capability + PCI_SEMAPHORE_OFFSET;
                            current_mst_device->pciconf_device.data_offset =
                                    current_mst_device->pciconf_device.vendor_specific_capability + PCI_DATA_OFFSET;
                            current_mst_device->pciconf_device.address_offset =
                                    current_mst_device->pciconf_device.vendor_specific_capability + PCI_ADDRESS_OFFSET;
                            break;

                    case MST_PCICONF_NO_FULL_VSEC:
                            current_mst_device->access.read = read_pciconf_no_full_vsec;
                            current_mst_device->access.write = write_pciconf_no_full_vsec;
                            current_mst_device->access.init = init_pciconf_no_full_vsec;
                            current_mst_device->pciconf_device.address_register = MST_CONF_ADDRES_REGISETER;
                            current_mst_device->pciconf_device.data_register = MST_CONF_DATA_REGISTER;
                            current_mst_device->wo_address = is_wo_gw(current_mst_device);
                            break;

                    case MST_PCI_MEMORY:
                            current_mst_device->access.read = read_memory;
                            current_mst_device->access.write = write_memory;
                            current_mst_device->access.init = init_memory;
                            current_mst_device->memory_device.connectx_wa_slot_p1 = 0;
                            current_mst_device->memory_device.hardware_memory_address =
                                    ioremap(pci_resource_start(current_mst_device->pci_device,
                                                               current_mst_device->memory_device.pci_memory_bar_address),
                                                               MST_MEMORY_SIZE);

                            if (current_mst_device->memory_device.hardware_memory_address <= 0) {
                                    mst_error("could not map device memory\n");
                            }

                            break;
            }

            minor++;
    }

    return 0;
}




int create_mst_devices(int contiguous_device_numbers, dev_t device_number,
                       int is_mft_package, struct file_operations* fop)
{
    struct pci_dev* pci_device = NULL;
    int error_code = 0;

    /* Find all Nvidia PCI devices. */
    while ((pci_device = pci_get_device(MST_NVIDIA_PCI_VENDOR, PCI_ANY_ID,
                                        pci_device)) != NULL) {
            /* Create pciconf device. */
            if (is_pciconf_device(pci_device)) {
                    if ((error_code =
                            create_mst_device(pci_device, MST_PCICONF,
                                              is_mft_package)) != 0) {
                        mst_error("Failed to create pci conf device\n");
                        goto ReturnOnFinished;
                    }
            }

            /* Create pci memory device. */
            if (is_memory_device(pci_device)) {
                    if ((error_code =
                            create_mst_device(pci_device, MST_PCI_MEMORY,
                                              is_mft_package)) != 0) {
                        mst_error("Failed to create pci conf device\n");
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

    mst_debug("Device found, id: %d, slot id: %d\n",pci_device->device, PCI_FUNC(pci_device->devfn));

ReturnOnFinished:
    return error_code;
}




int get_amount_of_nvidia_devices(void)
{
    int contiguous_device_numbers = 0;
    struct pci_dev* pci_device = NULL;

    /* Find all Nvidia PCI devices. */
    while ((pci_device = pci_get_device(MST_NVIDIA_PCI_VENDOR, PCI_ANY_ID,
                                  pci_device)) != NULL) {
            if (is_memory_device(pci_device) || is_pciconf_device(pci_device)) {
                contiguous_device_numbers++;
            }
    }

    return contiguous_device_numbers;
}




int set_private_data(struct inode* inode, struct file* file)
{
    struct mst_device* current_mst_device = NULL;
    struct mst_device* temp_mst_device = NULL;
    int minor = iminor(file_inode(file));

    /* Set private data to mst structure. */
    list_for_each_entry_safe(current_mst_device, temp_mst_device,
                             &mst_device_list, entry) {
            if (MINOR(current_mst_device->device_number) == minor) {
                    file->private_data = current_mst_device;
                    return 0;
            }
    }

    mst_error("failed to find device with minor=%d\n", minor);

    return -EINVAL;
}




int mutex_lock_mst(struct file* file)
{
    struct mst_device* mst_device;

    if (!file) {
        return 1;
    }

    mst_device = file->private_data;

    if (!mst_device) {
        return -EINVAL;
    }

    mutex_lock(&mst_device->lock);

    return 0;
}



void mutex_unlock_mst(struct file* file)
{
    struct mst_device* mst_device = file->private_data;

    mutex_unlock(&mst_device->lock);
}




void destroy_mst_devices(void)
{
    struct mst_device* current_mst_device;
    struct mst_device* temp_mst_device;

    /* free all mst_devices */
    list_for_each_entry_safe(current_mst_device, temp_mst_device,
                             &mst_device_list, entry) {
            /* Character device is no longer, it must be properly destroyed. */
            cdev_del(&current_mst_device->mcdev);
            device_destroy(mst_driver_info.class_driver, current_mst_device->device_number);
            list_del(&current_mst_device->entry);
            kfree(current_mst_device);
    }
}
