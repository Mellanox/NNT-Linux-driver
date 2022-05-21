KERNAL_VERSION ?= $(shell uname -r)
CPU_ARCH ?= $(shell uname -m)

# Oracle Linux OS.
ifneq ($(shell if (echo $(KERNAL_VERSION) | grep -qE 'uek'); then \
                                       echo "YES"; else echo ""; fi),)
override WITH_MAKE_PARAMS += ctf-dir=$(CWD)/.ctf
endif

NNT_DRIVER_LOCATION = ../../
PACKAGE_NAME = nnt-driver
PACKAGE_VERSION = 1.0.0
PACKAGE_RC = 1

GENERATED = $(NNT_DRIVER_LOCATION)/ofed/debian/changelog $(NNT_DRIVER_LOCATION)/ofed/debian/control \
  $(NNT_DRIVER_LOCATION)/ofed/debian/control.no_dkms $(NNT_DRIVER_LOCATION)/ofed/debian/rules


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

all: $(GENERATED)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) CONFIG_CTF= CONFIG_CC_STACKPROTECTOR_STRONG= $(WITH_MAKE_PARAMS) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(GENERATED)

generated: $(GENERATED)
