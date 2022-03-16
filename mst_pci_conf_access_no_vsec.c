#include "mst_device_defs.h"
#include "mst_pci_conf_access_no_vsec.h"
#include "mst_defs.h"


int read_pciconf_no_full_vsec(struct mst_device* mst_device, struct rw_operation* read_operation)
{
    int error;
    
    if (mst_device->wo_address) {
        read_operation->offset |= 0x1;
    }

    /* Write the wanted address to address register. */
    error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_register,
                                   read_operation->offset);
    CHECK_ERROR(error);

	/* Read the result from data register */
    error = pci_read_config_dword(mst_device->pci_device, mst_device->pciconf_device.data_register,
                                  read_operation->data);
    CHECK_ERROR(error);

ReturnOnFinished:
        return error;
}


int write_pciconf_no_full_vsec(struct mst_device* mst_device, struct rw_operation* write_operation)
{
    int error;

    if (mst_device->wo_address) {
            /* write the data to the data register. */
            error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.data_register,
                                   *write_operation->data);
            CHECK_ERROR(error);
                /* Write the destination address to address register. */
            error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_register,
                                           write_operation->offset);
            CHECK_ERROR(error);
    } else {
            /* Write the destination address to address register. */
            error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.address_register,
                                           write_operation->offset);
            CHECK_ERROR(error);

            /* write the data to the data register. */
            error = pci_write_config_dword(mst_device->pci_device, mst_device->pciconf_device.data_register,
                                           *write_operation->data);
            CHECK_ERROR(error);
    }

ReturnOnFinished:
        return error;
}


int init_pciconf_no_full_vsec(unsigned int command, void* user_buffer,
                              struct mst_device* mst_device)
{
    return 0;
}
