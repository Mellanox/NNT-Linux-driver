#include <linux/module.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include "nnt_ioctl.h"
#include "nnt_defs.h"
#include "nnt_device.h"
#include "nnt_ioctl_defs.h"
#include "nnt_pci_conf_access.h"
#include "mst_kernel.h"


MODULE_AUTHOR("Itay Avraham <itayavr@nvidia.com>");
MODULE_DESCRIPTION("NNT Linux driver (NVIDIA® networking tools driver), this is the backward compatibility driver");
MODULE_LICENSE("Dual BSD/GPL");

struct driver_info nnt_driver_info;
static char* name = "mstflint_access";


static int mstflint_bc_open(struct inode* inode, struct file* file)
{
    if (file->private_data) {
            return 0;
    }

    set_private_data_open(file);
    
    return 0;
}



static int mstflint_release(struct inode* inode, struct file* file)
{
    int error = 0;
    unsigned int slot_mask;
    struct nnt_device* nnt_device = NULL;

    /*
     * make sure the device is available since it
     * could be removed by hotplug event
     * if available grab its lock
     */
    error = mutex_lock_nnt(file);
    CHECK_ERROR(error);

    /* Get the nnt device structure */
    error = get_nnt_device(file, &nnt_device);
    if (error) {
            goto ReturnOnFinished;
    }
    
    slot_mask = ~(1 << (nnt_device->connectx_wa_slot_p1 - 1));
    nnt_device->connectx_wa_slots &= slot_mask;

    nnt_device->connectx_wa_slot_p1 = 0;

ReturnOnFinished:
    mutex_unlock_nnt(file);

    return error;
}



static long ioctl(struct file* file, unsigned int command,
                  unsigned long argument)
{
    void* user_buffer = (void*)argument;
    struct nnt_device* nnt_device = NULL;
    int error;

     /* By convention, any user gets read access
     * and is allowed to use the device.
     * Commands with no direction are administration
     * commands, and you need write permission
     * for this */

    if ( _IOC_DIR(command) == _IOC_NONE ) {
            if (!(file->f_mode & FMODE_WRITE)) {
                    return -EPERM;
            }
    } else {
            if (!(file->f_mode & FMODE_READ)) {
                    return -EPERM;
            }
    }
    error = mutex_lock_nnt(file);
    CHECK_ERROR(error);

    /* Get the nnt device structure */
    error = get_nnt_device(file, &nnt_device);
    if (error) {
            goto ReturnOnFinished;
    }

    switch (command)
    {
            case MST_WRITE4:
            {
                    struct nnt_rw_operation rw_operation;
                    struct mst_write4_st mst_write;

                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_write, user_buffer,
                                       sizeof(struct mst_write4_st))) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }
                    rw_operation.data[0] = mst_write.data;
                    rw_operation.offset = mst_write.offset;
                    rw_operation.size = 4;

                    error = nnt_device->access.write(nnt_device, &rw_operation);

                    break;
            }
            case PCICONF_WRITE4_BUFFER:
            {
                    struct nnt_rw_operation rw_operation;
                    struct mst_write4_st mst_write;

                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_write, user_buffer,
                                       sizeof(struct mst_write4_st))) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    rw_operation.data[0] = mst_write.data;
                    rw_operation.offset = mst_write.offset;
                    rw_operation.address_space = mst_write.address_space;
                    rw_operation.size = 4;

                    error = nnt_device->access.write(nnt_device, &rw_operation);

                    break;
            }
            case MST_READ4:
            {
                    struct nnt_rw_operation rw_operation;
                    struct mst_read4_st mst_read;

                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_read, user_buffer,
                                       sizeof(struct mst_read4_st))) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    rw_operation.offset = mst_read.offset;
                    rw_operation.size = 4;
                    rw_operation.address_space = mst_read.address_space;

                    error = nnt_device->access.read(nnt_device, &rw_operation);
                    if (error) {
                        goto ReturnOnFinished;
                    }

                    mst_read.data = rw_operation.data[0];
                      
                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &mst_read,
                                sizeof(struct mst_read4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }
                break;
            }
            case PCICONF_READ4_BUFFER:
            {
                    struct nnt_rw_operation rw_operation;
                    struct mst_read4_st mst_read;
                    
                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_read, user_buffer,
                                       sizeof(struct mst_read4_st))) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    rw_operation.offset = mst_read.offset;
                    rw_operation.address_space = mst_read.address_space;
                    rw_operation.size = 4;
                    error = nnt_device->access.read(nnt_device, &rw_operation);
                    if (error) {
                        goto ReturnOnFinished;
                    }

                    mst_read.data = rw_operation.data[0];
                      
                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &mst_read,
                                sizeof(struct mst_read4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    break;
            }
            case PCICONF_VPD_READ4:
            {
                    int vpd_default_timeout = 2000;
                    struct mst_vpd_read4_st mst_vpd_read;
                    struct nnt_vpd nnt_vpd;

                    if (!nnt_device->vpd_capability_address) {
                            nnt_error("Device %s not support Vital Product Data\n", nnt_device->device_name);
                            error = -ENODEV;
                            goto ReturnOnFinished;
                    }

                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_vpd_read, user_buffer,
                                sizeof(struct mst_vpd_read4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    nnt_vpd.offset = mst_vpd_read.offset;
                    nnt_vpd.data = mst_vpd_read.data;

                    if (!nnt_vpd.timeout) {
                        nnt_vpd.timeout = vpd_default_timeout;
                    }

                    error = vpd_read(&nnt_vpd, nnt_device);
                    if (error) {
                        goto ReturnOnFinished;
                    }

                    mst_vpd_read.offset = nnt_vpd.offset;
                    mst_vpd_read.data = nnt_vpd.data;

                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &mst_vpd_read,
                                sizeof(struct mst_vpd_read4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    break;
            }
            case PCICONF_VPD_WRITE4:
            {
                    int vpd_default_timeout = 2000;
                    struct mst_vpd_write4_st mst_vpd_write;
                    struct nnt_vpd nnt_vpd;

                    if (!nnt_device->vpd_capability_address) {
                            nnt_error("Device %s not support Vital Product Data\n", nnt_device->device_name);
                            error = -ENODEV;
                            goto ReturnOnFinished;
                    }

                    /* Copy the request from user space. */
                    if (copy_from_user(&mst_vpd_write, user_buffer,
                                sizeof(struct mst_vpd_write4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    nnt_vpd.offset = mst_vpd_write.offset;
                    nnt_vpd.data = mst_vpd_write.data;

                    if (!nnt_vpd.timeout) {
                        nnt_vpd.timeout = vpd_default_timeout;
                    }

                    error = vpd_write(&nnt_vpd, nnt_device);
                    if (error) {
                        goto ReturnOnFinished;
                    }

                    mst_vpd_write.offset = nnt_vpd.offset;
                    mst_vpd_write.data = nnt_vpd.data;

                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &mst_vpd_write,
                                sizeof(struct mst_vpd_write4_st)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    break;    
            }
            case PCICONF_GET_DMA_PAGES:
            {
                    error = dma_pages_ioctl(NNT_GET_DMA_PAGES, user_buffer,
                                            nnt_device);
                    break;
            }
            case PCICONF_RELEASE_DMA_PAGES:
            {
                    error = dma_pages_ioctl(NNT_RELEASE_DMA_PAGES, user_buffer,
                                            nnt_device);
                    break;
            }
            case PCICONF_READ_DWORD_FROM_CONFIG_SPACE:
            {
                    struct nnt_read_dword_from_config_space nnt_read_from_cspace;

                    /* Copy the request from user space. */
                    if (copy_from_user(&nnt_read_from_cspace, user_buffer,
                                       sizeof(struct nnt_read_dword_from_config_space))) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    /* Read the dword. */
                    if (read_dword(&nnt_read_from_cspace, nnt_device)) {
                        goto ReturnOnFinished;
                    }

                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &nnt_read_from_cspace,
                                sizeof(struct nnt_read_dword_from_config_space)) != 0) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    break;
            }
            case MST_PARAMS:
            {
                    struct nnt_device_parameters nnt_parameters;
                    struct mst_params mst_params;

                    error = get_nnt_device_parameters(&nnt_parameters, nnt_device);
                    if (error) {
                            goto ReturnOnFinished;
                    }

                    mst_params.bus = nnt_parameters.bus;
                    mst_params.bar = 0;
                    mst_params.domain = nnt_parameters.domain;
                    mst_params.func = nnt_parameters.function;
                    mst_params.slot = nnt_parameters.slot;
                    mst_params.device = nnt_parameters.device;
                    mst_params.vendor = nnt_parameters.vendor;
                    mst_params.subsystem_device = nnt_parameters.subsystem_device;
                    mst_params.subsystem_vendor = nnt_parameters.subsystem_vendor;
                    mst_params.vendor_specific_cap = nnt_parameters.vendor_specific_capability;

                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &mst_params,
                                sizeof(struct mst_params))!= 0) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }

                    break;
            }
            case PCI_CONNECTX_WA:
	        {
                    unsigned int slot_mask;

                    /* Slot exists validation. */
                    if (nnt_device->connectx_wa_slot_p1)
                    {
                        nnt_error("slot exits for file %s, slot:0x%x\n", nnt_device->device_name, nnt_device->connectx_wa_slot_p1);
                        error = -EPERM;
                        goto ReturnOnFinished;
                    }

                    /* Find first un(set) bit. and remember the slot. */
                    nnt_device->connectx_wa_slot_p1 = ffs(~nnt_device->connectx_wa_slots);
                    if (nnt_device->connectx_wa_slot_p1 == 0 || nnt_device->connectx_wa_slot_p1 > CONNECTX_WA_SIZE)
                    {
                        error = -ENOLCK;
                        goto ReturnOnFinished;
                    }

                    slot_mask = 1 << (nnt_device->connectx_wa_slot_p1 - 1);

                    /* Set the slot as taken. */
                    nnt_device->connectx_wa_slots |= slot_mask;

                    if (copy_to_user(user_buffer, &nnt_device->connectx_wa_slot_p1,
                                     sizeof(unsigned int)) != 0) {
                        error = -EFAULT;
                        goto ReturnOnFinished;
                    }
                    break;

            }
            default:
                    error = -EINVAL;
                    break;
                        
    }

ReturnOnFinished:
    mutex_unlock_nnt(file);

    return error;
}


struct file_operations fop = {
        .unlocked_ioctl = ioctl,
        .open = mstflint_bc_open,
        .release = mstflint_release,
        .owner = THIS_MODULE
};


static int __init mstflint_init_module(void)
{
    int is_alloc_chrdev_region = 1;
    int first_minor_number = 0;
    int error = 0;

    /* Get the amount of the Nvidia devices. */
    if((nnt_driver_info.contiguous_device_numbers =
            get_amount_of_nvidia_devices()) == 0) {
                nnt_error("No devices found\n");
                goto ReturnOnFinished;
    }

    /* Allocate char driver region and assign major number */
    if((error =
            alloc_chrdev_region(&nnt_driver_info.device_number, first_minor_number,
                                nnt_driver_info.contiguous_device_numbers, name)) != 0) {
                nnt_error("failed to allocate chrdev_region\n");
                goto CharDeviceAllocated;
    }

    /* create sysfs class. */
    if ((nnt_driver_info.class_driver =
            class_create(THIS_MODULE, NNT_CLASS_NAME)) == NULL) {
            nnt_error("Class creation failed\n");
            error = -EFAULT;
            goto DriverClassAllocated;
    }

    /* Create device files for MSTflint. */
    if((error =
            create_nnt_devices(nnt_driver_info.device_number, is_alloc_chrdev_region,
                               &fop, NNT_PCICONF_DEVICES)) == 0) {
            goto ReturnOnFinished;
    }

DriverClassAllocated:
    destroy_nnt_devices(is_alloc_chrdev_region);
    class_destroy(nnt_driver_info.class_driver);

CharDeviceAllocated:
    unregister_chrdev_region(nnt_driver_info.device_number, nnt_driver_info.contiguous_device_numbers);

ReturnOnFinished:
    return error;
}




static void __exit mstflint_cleanup_module(void)
{
    int is_alloc_chrdev_region = 1;

    destroy_nnt_devices(is_alloc_chrdev_region);
    class_destroy(nnt_driver_info.class_driver);
    unregister_chrdev_region(nnt_driver_info.device_number, nnt_driver_info.contiguous_device_numbers);
}


module_init(mstflint_init_module);
module_exit(mstflint_cleanup_module);
