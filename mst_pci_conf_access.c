#include "mst_device_defs.h"
#include "mst_pci_conf_access_defs.h"
#include "mst_pci_conf_access.h"
#include "mst_defs.h"
#include <linux/delay.h>
#include <linux/uaccess.h>


int clear_vsec_semaphore(struct mst_device* mst_device)
{
    /* Clear the semaphore. */
    int error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.semaphore_offset,
                                      0);
    CHECK_PCI_WRITE_ERROR(error, mst_device->pciconf_device.semaphore_offset,
                          0)

ReturnOnFinished:
    return error;
}


int get_semaphore_ticket(struct mst_device* mst_device, unsigned int* lock_value,
                         unsigned int* counter)
{
    unsigned int counter_offset = mst_device->pciconf_device.vendor_specific_capability + PCI_COUNTER_OFFSET;
    int error;

    /* Read ticket. */
    error = pci_read_config_dword(mst_device->pci_device, counter_offset,
                                  counter);
    CHECK_PCI_READ_ERROR(error, counter_offset);

    /* Write to semaphore ticket. */
    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.semaphore_offset,
                                   *counter);
    CHECK_PCI_WRITE_ERROR(error, mst_device->pciconf_device.semaphore_offset,
                          *counter);

    /* Read back semaphore to make sure the
     *   ticket is equal to semphore */
    error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.semaphore_offset,
                                  lock_value);
    CHECK_PCI_READ_ERROR(error, mst_device->pciconf_device.semaphore_offset);

ReturnOnFinished:
    return error;
}




int lock_vsec_semaphore(struct mst_device* mst_device)
{
    unsigned int lock_value = -1;
    unsigned int counter = 0;
    unsigned int retries = 0;
    int error = 0;

    do {
            if (retries > SEMAPHORE_MAX_RETRIES) {
                    return -EINVAL;
            }

            /* Read the semaphore, until we will get 0. */
            error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.semaphore_offset,
                                          &lock_value);
            CHECK_PCI_READ_ERROR(error, mst_device->pciconf_device.semaphore_offset);

            /* Is semaphore taken ? */
            if (lock_value) {
                    retries++;
                    udelay(1000);
                    continue;
            }

            /* Get semaphore ticket */
            error = get_semaphore_ticket(mst_device, &lock_value,
                                         &counter);
            CHECK_ERROR(error);
    } while (counter != lock_value);

ReturnOnFinished:
    return error;
}




int read_dword(struct read_dword_from_config_space* read_from_cspace, struct mst_device* mst_device)
{
    int error;

    /* Take semaphore. */
    error = lock_vsec_semaphore(mst_device);
    CHECK_ERROR(error);

    /* Read dword from config space. */
    error = pci_read_config_dword(mst_device->pci_device, read_from_cspace->offset,
                                  &read_from_cspace->data);
    CHECK_PCI_READ_ERROR(error, read_from_cspace->offset);

ReturnOnFinished:
    /* Clear semaphore. */
    clear_vsec_semaphore(mst_device);
    return error;
}


int wait_on_flag(struct mst_device* mst_device, u8 expected_val)
{
	unsigned int flag;
	int retries;
	int error = -1;
    
    for (retries = 0; retries < IFC_MAX_RETRIES; retries++) {
            /* Read the flag. */
            error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_offset,
                                          &flag);
            CHECK_PCI_READ_ERROR(error, mst_device->pciconf_device.address_offset);

            flag = EXTRACT(flag, PCI_FLAG_BIT_OFFSET,
                           1);
            if (flag == expected_val) {
                    return 0;
            }
    }

ReturnOnFinished:
	return error;
}


int set_address_space(struct mst_device* mst_device, unsigned int space)
{
    unsigned int control_offset = mst_device->pciconf_device.vendor_specific_capability + PCI_CONTROL_OFFSET;
	unsigned int value;
	int error = 0;

    /* Read value from control offset. */
    error = pci_read_config_dword(mst_device->pci_device, control_offset,
                                  &value);
    CHECK_PCI_READ_ERROR(error, control_offset);
    
    /* Set the bit space indication and write it back. */
    value = MERGE(value, space,
                  PCI_SPACE_BIT_OFFSET, PCI_SPACE_BIT_LENGTH);
    error = pci_write_config_dword(mst_device->pci_device, control_offset,
                                   value);
    CHECK_PCI_WRITE_ERROR(error, control_offset,
                          value);
    
    /* Read status and make sure space is supported. */
    error = pci_read_config_dword(mst_device->pci_device, control_offset,
                                  &value);
    CHECK_PCI_READ_ERROR(error, control_offset);

    if (EXTRACT(value, PCI_STATUS_BIT_OFFSET,
                    PCI_STATUS_BIT_LEN) == 0) {
        error = -EINVAL;
    }

ReturnOnFinished:
    return error;
}


int check_address_space_support(struct mst_device* mst_device)
{
    int error;

    if ((!mst_device->pciconf_device.vendor_specific_capability) || (!mst_device->pci_device)) {
		    return 0;
    }
    
    /* Get semaphore ticket */
    error = lock_vsec_semaphore(mst_device);
    CHECK_ERROR(error);

    /* Is ICMD address space supported ?*/
    if(set_address_space(mst_device, ADDRESS_SPACE_ICMD) == 0) {
            mst_device->pciconf_device.address_space.icmd = 1;
    }

    /* Is CR Space address space supported ?*/
    if(set_address_space(mst_device, ADDRESS_SPACE_CR_SPACE) == 0) {
            mst_device->pciconf_device.address_space.cr_space = 1;
    }

    /* Is semaphore address space supported ?*/
    if(set_address_space(mst_device, ADDRESS_SPACE_SEMAPHORE) == 0) {
            mst_device->pciconf_device.address_space.semaphore = 1;
    }

ReturnOnFinished:
    /* Clear semaphore. */
    clear_vsec_semaphore(mst_device);

    return 0;
}



int set_rw_address(unsigned int* offset, unsigned int rw)
{
    u32 address = *offset;

    /* Last 2 bits must be zero as we only allow 30 bits addresses. */
    if (EXTRACT(address, 30,
                    2)) {
            return -1;
    }

    address = MERGE(address, rw,
                    PCI_FLAG_BIT_OFFSET, 1);
    *offset = address;

    return 0;
}




int read(struct mst_device* mst_device, unsigned int offset,
         unsigned int* data)
{
    int error = set_rw_address(&offset, READ_OPERATION);
    CHECK_ERROR(error);

    /* Write address. */
    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_offset,
                                   offset);
    CHECK_PCI_WRITE_ERROR(error, mst_device->pciconf_device.address_offset,
                          offset);

    error = wait_on_flag(mst_device, 1);
    CHECK_ERROR(error);

    /* Read data. */
    error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.data_offset,
                                  data);
    CHECK_PCI_READ_ERROR(error, mst_device->pciconf_device.data_offset);

ReturnOnFinished:
    return error;
}



int read_pciconf(struct mst_device* mst_device, struct rw_operation* read_operation)
{
    int counter;
    int error;

    /* Lock semaphore. */
    error = lock_vsec_semaphore(mst_device);
    CHECK_ERROR(error);
    
    /* Is CR Space address space supported ?*/
    error = set_address_space(mst_device, read_operation->address_space);
    CHECK_ERROR(error);

	for (counter = 0; counter < read_operation->size; counter += 4) {
            if (read(mst_device, read_operation->offset + counter,
                     &read_operation->data[counter >> 2])) {
                    error = counter;
                    goto ReturnOnFinished;
		    }
	}

ReturnOnFinished:
    /* Clear semaphore. */
    clear_vsec_semaphore(mst_device);

    return error;
}



int write(struct mst_device* mst_device, unsigned int offset,
          unsigned int data)
{
    int error = set_rw_address(&offset, WRITE_OPERATION);
    CHECK_ERROR(error);
    
    /* Write data. */
    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.data_offset,
                                   data);
    CHECK_PCI_WRITE_ERROR(error,mst_device->pciconf_device.data_offset,
                          data);

    /* Write address. */
    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_offset,
                                   offset);
    CHECK_PCI_WRITE_ERROR(error, mst_device->pciconf_device.address_offset,
                          offset);

    error = wait_on_flag(mst_device, 0);

ReturnOnFinished:
    return error;
}


int write_pciconf(struct mst_device* mst_device, struct rw_operation* write_operation)
{
    int counter;
    int error;

    /* Lock semaphore. */
    error = lock_vsec_semaphore(mst_device);
    CHECK_ERROR(error);
    
    /* Is CR Space address space supported ?*/
    error = set_address_space(mst_device, write_operation->address_space);
    CHECK_ERROR(error);

	for (counter = 0; counter < write_operation->size; counter += 4) {
            if (write(mst_device, write_operation->offset + counter,
                      write_operation->data[counter >> 2])) {
                    error = counter;
                    goto ReturnOnFinished;
		    }
	}

ReturnOnFinished:
    /* Clear semaphore. */
    clear_vsec_semaphore(mst_device);

    return error;
}


int init_pciconf(unsigned int command, void* user_buffer,
                 struct mst_device* mst_device)
{
    struct pciconf_init init;
    int error = 0;

    /* Copy the request from user space. */
    if (copy_from_user(&init, user_buffer,
                       sizeof(struct pciconf_init)) != 0) {
            error = -EFAULT;
            goto ReturnOnFinished;
    }
 
    mst_device->pciconf_device.address_register = init.address_register;
    mst_device->pciconf_device.address_register = init.address_data_register;
    mst_device->pciconf_device.vendor_specific_capability =
            pci_find_capability(mst_device->pci_device, VSEC_CAPABILITY_ADDRESS);

    mst_device->pciconf_device.semaphore_offset =
        mst_device->pciconf_device.vendor_specific_capability + ADDRESS_SPACE_SEMAPHORE;
    mst_device->pciconf_device.data_offset =
        mst_device->pciconf_device.vendor_specific_capability + PCI_DATA_OFFSET;
    mst_device->pciconf_device.address_offset =
        mst_device->pciconf_device.vendor_specific_capability + PCI_ADDRESS_OFFSET;

ReturnOnFinished:
    return error;
}
