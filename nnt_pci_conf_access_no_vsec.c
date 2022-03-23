#include "nnt_device_defs.h"
#include "nnt_pci_conf_access_no_vsec.h"
#include "nnt_defs.h"


int read_no_full_vsec(struct nnt_device* nnt_device, unsigned int offset,
                      unsigned int* data)
{
    int error;
    
    if (nnt_device->wo_address) {
        offset |= 0x1;
    }

    /* Write the wanted address to address register. */
    error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.address_register,
                                   offset);
    CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.address_register,
                          offset);

	/* Read the result from data register */
    error = pci_read_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.data_register,
                                  data);
    CHECK_PCI_READ_ERROR(error, nnt_device->pciconf_device.data_register);

ReturnOnFinished:
        return error;
}


int read_pciconf_no_full_vsec(struct nnt_device* nnt_device, struct rw_operation* read_operation)
{
    int counter;
    int error = 0;

    for (counter = 0; counter < read_operation->size; counter += 4) {
            if (read_no_full_vsec(nnt_device, read_operation->offset + counter,
                                  &read_operation->data[counter >> 2])) {
                    error = counter;
                    goto ReturnOnFinished;
            }
	}

ReturnOnFinished:
    return error;
}


int write_no_full_vsec(struct nnt_device* nnt_device, unsigned int offset,
                       unsigned int data)
{
    int error;

    if (nnt_device->wo_address) {
            /* write the data to the data register. */
            error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.data_register,
                                   data);
            CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.data_register,
                                  data);
                /* Write the destination address to address register. */
            error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.address_register,
                                           offset);
            CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.address_register,
                                  offset);
    } else {
            /* Write the destination address to address register. */
            error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.address_register,
                                           offset);
            CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.address_register,
                                  offset);

            /* write the data to the data register. */
            error = pci_write_config_dword(nnt_device->pci_device, nnt_device->pciconf_device.data_register,
                                           data);
            CHECK_PCI_WRITE_ERROR(error, nnt_device->pciconf_device.data_register,
                                  data);
    }

ReturnOnFinished:
        return error;
}


int write_pciconf_no_full_vsec(struct nnt_device* nnt_device, struct rw_operation* write_operation)
{
    int counter;
    int error = 0;

	for (counter = 0; counter < write_operation->size; counter += 4) {
            if (write_no_full_vsec(nnt_device, write_operation->offset + counter,
                                   write_operation->data[counter >> 2])) {
                    error = counter;
                    goto ReturnOnFinished;
		    }
	}

ReturnOnFinished:
    return error;
}


int init_pciconf_no_full_vsec(unsigned int command, void* user_buffer,
                              struct nnt_device* nnt_device)
{
    return 0;
}
