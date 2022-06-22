all:
	cd mst_backward_compatibility/mst_pci && $(MAKE)
	cd mst_backward_compatibility/mst_pciconf && $(MAKE)

clean:
	$(MAKE) clean -C mst_backward_compatibility/mst_pci
	$(MAKE) clean -C mst_backward_compatibility/mst_pciconf
