obj-m += mst_driver.o
mst_driver-objs += mst_device.o mst_dma.o mst_pci_conf_access.o \
				   mst_pci_conf_access_no_vsec.o mst_memory_access.o \
				   mst_ioctl.o main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
