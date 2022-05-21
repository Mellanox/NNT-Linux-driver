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
#include "mst_pci_bc.h"


MODULE_AUTHOR("Itay Avraham <itayavr@nvidia.com>");
MODULE_DESCRIPTION("NNT Linux driver (NVIDIA® networking tools driver), this is the backward compatibility driver");

struct driver_info nnt_driver_info;
static int major_number = -1;
static char* name = "mst_pci";

#define  INIT   PCI_INIT
#define  STOP   PCI_STOP
#define  PCI_PARAMS_ PCI_PARAMS
#define  CONNECTX_WA PCI_CONNECTX_WA


static int mst_pciconf_bc_open(struct inode* inode, struct file* file)
{
    if (file->private_data) {
            return 0;
    }

    set_private_data_open(file);
    
    return 0;
}


static ssize_t mst_pciconf_bc_read(struct file* file, char* buf,
                                   size_t count, loff_t* f_pos)
{
    struct nnt_device* nnt_device = NULL;
    int error;

    /* Get the nnt device structure */
    error = get_nnt_device(file, &nnt_device);
    if (error) {
            nnt_error("nnt device is null \n");
            count = -EFAULT;
            goto ReturnOnFinished;
    }

    error = mutex_lock_nnt(file);

	if (*f_pos >= nnt_device->buffer_used_bc) {
		    count = 0;
		    goto MutexUnlock;
	}

	if (*f_pos + count > nnt_device->buffer_used_bc) {
		    count = nnt_device->buffer_used_bc - *f_pos;
    }

	if (copy_to_user(buf,nnt_device->buffer_bc + *f_pos, count)) {
		    count = -EFAULT;
		    goto MutexUnlock;
	}

	*f_pos += count;

MutexUnlock:
    mutex_unlock_nnt(file);
ReturnOnFinished:
	return count;
}


static ssize_t mst_pciconf_bc_write(struct file* file, const char* buf,
                                    size_t count, loff_t* f_pos)
{
    struct nnt_device* nnt_device = NULL;
    int error;

    /* Get the nnt device structure */
    error = get_nnt_device(file, &nnt_device);
    if (error) {
            nnt_error("nnt device is null \n");
            count = -EFAULT;
            goto ReturnOnFinished;
    }

    error = mutex_lock_nnt(file);

	if (*f_pos >= MST_BC_BUFFER_SIZE) {
		    count = 0;
		    goto MutexUnlock;
	}

	if (*f_pos + count > MST_BC_BUFFER_SIZE) {
		    count = MST_BC_BUFFER_SIZE - *f_pos;
    }

	if (copy_from_user(nnt_device->buffer_bc + *f_pos, buf, count)) {
		    count = -EFAULT;
		    goto MutexUnlock;
	}

	*f_pos += count;

	if (nnt_device->buffer_used_bc < *f_pos) {
            nnt_device->buffer_used_bc = *f_pos;
    }

MutexUnlock:
    mutex_unlock_nnt(file);
ReturnOnFinished:
	return count;
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

    if (command != INIT) {
            error = mutex_lock_nnt(file);
            CHECK_ERROR(error);

            /* Get the nnt device structure */
            error = get_nnt_device(file, &nnt_device);
            if (error) {
                    nnt_error("nnt device is null \n");
                    goto ReturnOnFinished;
            }
    }

    switch (command) {
	case INIT:
    {
			struct mst_pci_init_st mst_init;
			struct pci_bus* bus = NULL;

            /* Copy the request from user space. */
            if (copy_from_user(&mst_init, user_buffer,
                               sizeof(struct mst_pci_init_st))) {
                error = -EFAULT;
                goto ReturnOnFinished;
            }

			bus = pci_find_bus(mst_init.domain, mst_init.bus);

			if (!bus) {
				error = -ENXIO;
				goto ReturnOnFinished;
			}

			nnt_device->pci_device = NULL;
			nnt_device->pci_device = pci_get_slot (bus, mst_init.devfn);

			if (!nnt_device->pci_device) {
				error = -ENXIO;
				goto ReturnOnFinished;
			}

			if (mst_init.bar >= DEVICE_COUNT_RESOURCE) {
                error = -ENXIO;
				goto ReturnOnFinished;
			}

			nnt_device->device_pci.bar_address =
                nnt_device->pci_device->resource[mst_init.bar].start;
			nnt_device->device_pci.bar_size =
                nnt_device->pci_device->resource[mst_init.bar].end + 1 - nnt_device->pci_device->resource[mst_init.bar].start;

			if (nnt_device->device_pci.bar_size == 1) {
				nnt_device->device_pci.bar_size = 0;
			}

            nnt_device->buffer_used_bc = 0;
			break;
		}
                
    case PCI_PARAMS_:
    {
            struct mst_pci_params_st params;
			params.bar = nnt_device->device_pci.bar_address;
			params.size = nnt_device->device_pci.bar_size;

			if (copy_to_user(user_buffer, &params,
                             sizeof(struct mst_pci_params_st))) {
				error = -EFAULT;
                goto ReturnOnFinished;
			}

            break;
    }
    case CONNECTX_WA:
	{
            struct nnt_connectx_wa connectx_wa;
            error = pci_connectx_wa(&connectx_wa, nnt_device);

            /* Copy the data to the user space. */
            if (copy_to_user(user_buffer, &connectx_wa,
                             sizeof(struct nnt_connectx_wa)) != 0) {
                    error = -EFAULT;
                    goto ReturnOnFinished;
            }

            break;
	}
	case STOP:
            break;
    default:
            nnt_error("ioctl not implemented, command id: %x\n", command);
            error = -EINVAL;
            break;
                
    }

ReturnOnFinished:
    mutex_unlock_nnt(file);

    return error;
}

static int mst_release(struct inode* inode, struct file* file)
{
    struct nnt_device* nnt_device = NULL;
    int error;

    /* Get the nnt device structure */
    error = get_nnt_device(file, &nnt_device);
    if (error) {
            nnt_error("nnt device is null \n");
            goto ReturnOnFinished;
    }

	if (nnt_device->memory_device.connectx_wa_slot_p1) {
		unsigned int mask = 0;

        error = mutex_lock_nnt(file);
        CHECK_ERROR(error);

		mask = ~(1 << (nnt_device->memory_device.connectx_wa_slot_p1 - 1));
		
        nnt_device->memory_device.connectx_wa_slot_p1 &= mask; // Fix me

        nnt_device->memory_device.connectx_wa_slot_p1 = 0;
        mutex_unlock_nnt(file);
	}

ReturnOnFinished:
	return 0;
}


struct file_operations fop = {
        .unlocked_ioctl = ioctl,
        .open = mst_pciconf_bc_open,
        .write = mst_pciconf_bc_write,
        .read = mst_pciconf_bc_read,
        .release = mst_release,
        .owner = THIS_MODULE
};


static int __init mst_pciconf_init_module(void)
{
    dev_t device_number = -1;
    int is_mft_package = 1;
    int error = 0;
    
    /* Allocate char driver region and assign major number */
    major_number = register_chrdev(0, name,
                                   &fop);
	if (major_number <=0 ) {
		nnt_error("Unable to register character mst pciconf driver.\n");
		error = -EINVAL;
	}

    /* Create device files for MFT. */
    error = create_nnt_devices(device_number, is_mft_package,
                               &fop);
   
    return error;
}




static void __exit mst_pciconf_cleanup_module(void)
{
    unregister_chrdev(major_number, name);
}


module_init(mst_pciconf_init_module);
module_exit(mst_pciconf_cleanup_module);
