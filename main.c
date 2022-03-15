/*
 * Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <linux/module.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include "mst_defs.h"
#include "mst_device.h"
#include "mst_ioctl.h"
#include "mst_ioctl_defs.h"

MODULE_AUTHOR("Itay Avraham - Nvidia | itayavr@nvidia.com");
MODULE_DESCRIPTION("PCI Configuration access");

/* Passing MFT flag argument */
int mft_package = 0;

/* Create the file in sysfs. */
module_param(mft_package, int,
             S_IRUSR);

struct driver_info mst_driver_info;


static long mst_ioctl(struct file* file, unsigned int command,
                      unsigned long argument)
{
    void* user_buffer = (void*)argument;
    struct mst_device* mst_device = NULL;
    int error;

    error = mutex_lock_mst(file);
    CHECK_ERROR(error);

    /* Get the mst device structure */
    error = get_mst_device(file, &mst_device);
    if (error) {
            goto ReturnOnFinished;
    }

    switch (command) {
            case MST_GET_DMA_PAGES:
            case MST_RELEASE_DMA_PAGES:
                    error = dma_pages_ioctl(command, user_buffer,
                                            mst_device);
                    break;

            case MST_READ_DWORD_FROM_CONFIG_SPACE:
                    error = read_dword_ioctl(command, user_buffer,
                                             mst_device);
                    break;

            case MST_WRITE:
            case MST_READ:
            {   
                    struct rw_operation rw_operation;

                    /* Copy the request from user space. */
                    if (copy_from_user(&rw_operation, user_buffer,
                                sizeof(struct rw_operation)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }

                    switch(command) {
                            case MST_WRITE:
                                    mst_device->access.write(mst_device, &rw_operation);
                                    break;
                            case MST_READ:
                                    mst_device->access.read(mst_device, &rw_operation);
                                    break;
                            default:
                                    mst_error("ioctl not implemented, command id: %d\n", command);
                                    error = -EINVAL;
                                    goto ReturnOnFinished;
                    }

                    /* Copy the data to the user space. */
                    if (copy_to_user(user_buffer, &rw_operation,
                                sizeof(struct rw_operation)) != 0) {
                            error = -EFAULT;
                            goto ReturnOnFinished;
                    }
                    break;
            }
            case MST_GET_DEVICE_PARAMETERS:
                    error = get_mst_device_parameters(command, user_buffer,
                            mst_device);
                    
                    break;

            case MST_INIT:
                    error = mst_device->access.init(command, user_buffer,
                                                    mst_device);
                    break;
            case MST_PCI_CONNECTX_WA:
                    error = pci_connectx_wa(command, user_buffer,
                            mst_device);
                    break;
            default:
                mst_error("ioctl not implemented, command id: %x\n", command);
                error = -EINVAL;
                break;
                
    }

ReturnOnFinished:
    mutex_unlock_mst(file);

    return error;
}



static int mst_open(struct inode* inode, struct file* file)
{
    if (file->private_data) {
            return 0;
    }

    return set_private_data(inode, file);
}



struct file_operations fop = {
        .unlocked_ioctl = mst_ioctl,
        .open = mst_open,
        .owner = THIS_MODULE
};


static int __init mst_init_module(void)
{
    int first_minor_number = 0;
    int error = 0;
    dev_t device_numbers;

    /* Get the amount of the Nvidia devices. */
    if((mst_driver_info.contiguous_device_numbers =
            get_amount_of_nvidia_devices()) == 0) {
                mst_error("No devices found\n");
                goto ReturnOnFinished;
    }
    /* Allocate char driver region and assign major number */
    if((error =
            alloc_chrdev_region(&device_numbers, first_minor_number,
                                mst_driver_info.contiguous_device_numbers, MST_DRIVER_NAME)) != 0) {
                mst_error("failed to allocate chrdev_region\n");
                goto CharDeviceAllocated;
    }

    mst_driver_info.driver_major_number = MAJOR(device_numbers);

    /* create sysfs class. */
    if ((mst_driver_info.class_driver =
            class_create(THIS_MODULE, MST_CLASS_NAME)) == NULL) {
            mst_error("Class creation failed\n");
            error = -EFAULT;
            goto DriverClassAllocated;
    }

    /* Create device files for mstflint and MFT */
    if((error =
            create_mst_devices(mst_driver_info.contiguous_device_numbers, device_numbers,
                               mft_package, &fop)) == 0) {
            goto ReturnOnFinished;
    }

DriverClassAllocated:
    destroy_mst_devices();
    class_destroy(mst_driver_info.class_driver);

CharDeviceAllocated:
    unregister_chrdev_region(mst_driver_info.driver_major_number, mst_driver_info.contiguous_device_numbers);

ReturnOnFinished:
    return error;
}




static void __exit mst_cleanup_module(void)
{
    destroy_mst_devices();
    class_destroy(mst_driver_info.class_driver);
    unregister_chrdev_region(mst_driver_info.driver_major_number, mst_driver_info.contiguous_device_numbers);
}


module_init(mst_init_module);
module_exit(mst_cleanup_module);
