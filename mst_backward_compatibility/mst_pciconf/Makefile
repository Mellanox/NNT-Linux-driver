KERNAL_VERSION ?= $(shell uname -r)
CPU_ARCH ?= $(shell uname -m)

# Oracle Linux OS.
ifneq ($(shell if (echo $(KERNAL_VERSION) | grep -qE 'uek'); then \
                                       echo "YES"; else echo ""; fi),)
override WITH_MAKE_PARAMS += ctf-dir=$(CWD)/.ctf
endif

NNT_DRIVER_ROOT_LOCATION = ../../
NNT_DRIVER_LOCATION = ../../nnt_driver
PACKAGE_NAME = nnt-driver
PACKAGE_VERSION = 1.0.0
PACKAGE_RC = 1

%: %.in
	sed \
		-e 's/@PACKAGE_NAME@/$(PACKAGE_NAME)/g' \
		-e 's/@PACKAGE_VERSION@/$(PACKAGE_VERSION)/g' \
		-e 's/@PACKAGE_RC@/$(PACKAGE_RC)/g' \
	<$< >$@

obj-m += mst_pciconf.o
EXTRA_CFLAGS= -I$(PWD)/$(NNT_DRIVER_LOCATION)
mst_pciconf-objs += $(NNT_DRIVER_LOCATION)/nnt_device.o $(NNT_DRIVER_LOCATION)/nnt_dma.o $(NNT_DRIVER_LOCATION)/nnt_pci_conf_access.o \
		 			$(NNT_DRIVER_LOCATION)/nnt_pci_conf_access_no_vsec.o $(NNT_DRIVER_LOCATION)/nnt_memory_access.o \
		   	        $(NNT_DRIVER_LOCATION)/nnt_ioctl.o mst_pciconf_bc.o

all:
	make -C /lib/modules/$(KERNAL_VERSION)/build M=$(PWD) CONFIG_CTF= CONFIG_CC_STACKPROTECTOR_STRONG= $(WITH_MAKE_PARAMS) modules

clean:
	make -C /lib/modules/$(KERNAL_VERSION)/build M=$(PWD) clean
