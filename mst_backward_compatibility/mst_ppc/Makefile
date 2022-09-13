KERNAL_VERSION ?= $(shell uname -r)
CPU_ARCH ?= $(shell uname -m)

# Oracle Linux OS.
ifneq ($(shell if (echo $(KERNAL_VERSION) | grep -qE 'uek'); then \
                                       echo "YES"; else echo ""; fi),)
override WITH_MAKE_PARAMS += ctf-dir=$(CWD)/.ctf
endif

NNT_DRIVER__ROOT_LOCATION = ../../
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

ifneq ($(findstring ppc64, $(CPU_ARCH)),)
obj-m += mst_ppc_pci_reset.o
EXTRA_CFLAGS= -I$(PWD)/$(NNT_DRIVER_LOCATION)
endif

all:
	make -C /lib/modules/$(KERNAL_VERSION)/build M=$(PWD) CONFIG_CTF= CONFIG_CC_STACKPROTECTOR_STRONG= $(WITH_MAKE_PARAMS) modules

clean:
	make -C /lib/modules/$(KERNAL_VERSION)/build M=$(PWD) clean
