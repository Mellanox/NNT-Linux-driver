KERNAL_VERSION ?= $(shell uname -r)
CPU_ARCH ?= $(shell uname -m)

# Oracle Linux OS.
ifneq ($(shell if (echo $(KERNAL_VERSION) | grep -qE 'uek'); then \
                                       echo "YES"; else echo ""; fi),)
override WITH_MAKE_PARAMS += ctf-dir=$(CWD)/.ctf
endif

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

ifneq ($(findstring ppc64, $(CPU_ARCH)),)
obj-m += nnt_ppc_driver.o
nnt_ppc_driver-objs += nnt_ppc_driver_main.o
endif

obj-m += nnt_linux_driver.o
nnt_linux_driver-objs += nnt_device.o nnt_dma.o nnt_pci_conf_access.o \
				   		 nnt_pci_conf_access_no_vsec.o nnt_memory_access.o \
				   		 nnt_ioctl.o nnt_linux_driver_main.o

all: $(GENERATED)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) CONFIG_CTF= CONFIG_CC_STACKPROTECTOR_STRONG= $(WITH_MAKE_PARAMS) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(GENERATED)

generated: $(GENERATED)
