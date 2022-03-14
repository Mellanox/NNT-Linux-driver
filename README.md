# MST-Linux-driver
This repository contains the new unified driver and will be replaced the old drivers of MFT and MSTFlint packages.<br/>
The Mellanox devices are accessed via PCI Memory Mapping or by PCI configuration cycles.<br/>
Note that PCI configuration access is slower and less safe than memory access.<br/>
<br/>
This repository is maintained by Itay Avraham (itayavr@nvidia.com), please send any comments, suggestions, or questions to me.

## How to compile and load the driver.
Build the driver by using the make command and load the driver using the insmod command:<br/>
| Package                         | Commands                                              |
| --------------------------------| :---------------------------------------------------: |
| MSTFlint                        | `make`<br/> `insmod mst_driver.ko`<br/>               |
| MFT                             | `make`<br/> `insmod mst_driver.ko mft_package=1`<br/> |

## How to access from user space<br/>
IOCTL system call will be the communication channel between the user space and kernel Space.<br/>
Here are short explanation regarding the IOCTLS codes:<br/>


| IOCTL Code                         | Explanation                                            |
| ---------------------------------- | :-----------------------------------------------------:|
| `MST_INIT`                         | Used to change bar number and map the new bar on the fly |
| `MST_STOP`                         | Used to change configuration registers on the fly        |
| `MST_GET_DEVICE_PARAMETERS`        | Get information about the Mellanox device                |
| `MST_WRITE`                        | Write a block of data to PCI memory or configuration space<br/> Size is expressed as number of unsigned integers  |
| `MST_READ`                         | Read a block of data from PCI memory or configuration space<br/> Size is expressed as number of unsigned integers |
| `MST_GET_DMA_PAGES`                | Pin user space memory into the kernel space for DMA purpose<br/> User space application should allocate buffer as number of pages |
| `MST_RELEASE_DMA_PAGES`            | Unpin the user space memroy                              |
| `MST_READ_DWORD_FROM_CONFIG_SPACE` | Read a dword from the PCI configuation space             |
| `MST_PCI_CONNECTX_WA`              | ConnectX ordering workaround                             |


## FAQ: Here are some questions that I sometimes get that might help.
- Q: What is the major number of the driver?<br/>
  - A: The major number allocated dynamically, no fixed number.<br/>
- Q: What is the MFT & MSTFlint package version that supports this driver?
  - A:  X > 4.20.
