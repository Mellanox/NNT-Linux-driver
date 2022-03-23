obj-m += nnt_driver.o
nnt_driver-objs += nnt_device.o nnt_dma.o nnt_pci_conf_access.o \
				   nnt_pci_conf_access_no_vsec.o nnt_memory_access.o \
				   nnt_ioctl.o main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
