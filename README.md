# MST-Linux-driver
This repository contains the new unified driver and will be replaced the old drivers of MFT and MSTFlint packages.<br/>
The Mellanox devices are accessed via PCI Memory Mapping or by PCI configuration cycles.<br/>
Note that PCI configuration access is slower and less safe than memory access.<br/>

This repository is maintained by Itay Avraham (itayavr@nvidia.com), please send any comments, suggestions, or questions to me.

## How to compile and load the driver.
Build the driver by using the make command and load the driver using the insmod command:<br/>
MSTFlint:<br/>
`make`<br/>
`insmod mst_driver.ko`<br/>
MFT:<br/>
`make`<br/>
`insmod mst_driver.ko mft_package=1`<br/>


