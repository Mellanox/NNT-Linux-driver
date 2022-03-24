obj-m += nnt_driver.o
nnt_driver-objs += nnt_device.o nnt_dma.o nnt_pci_conf_access.o \
				   nnt_pci_conf_access_no_vsec.o nnt_memory_access.o \
				   nnt_ioctl.o main.o

PACKAGE_NAME = nnt-driver
PACKAGE_VERSION = 1.0.0
PACKAGE_RC = 1

GENERATED = ofed/debian/changelog ofed/debian/control \
  ofed/debian/control.no_dkms ofed/debian/rules


%: %.in
	sed \
		-e 's/@PACKAGE_NAME@/$(PACKAGE_NAME)/g' \
		-e 's/@PACKAGE_VERSION@/$(PACKAGE_VERSION)/g' \
		-e 's/@PACKAGE_RC@/$(PACKAGE_RC)/g' \
	<$< >$@


all: $(GENERATED)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(GENERATED)

generated: $(GENERATED)
