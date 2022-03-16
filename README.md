# MST-Linux-driver
This repository contains the new unified driver which will replace the old drivers of MFT and MSTFlint packages.<br/>
The Mellanox devices are accessed via PCI Memory Mapping or by PCI configuration cycles.<br/>
Note that PCI configuration access is slower and less safe than memory access.<br/>

The purpose of this driver is to provide a lightweight API for firmware management and debug tools for NVIDIA® devices:
- Generating a standard or customized NVIDIA® firmware image.
- Querying for firmware information.
- Burning a firmware image to a single Mellanox device via DMA access.
- Fast access to read & write registers of the NVIDIA® devices.

This repository is maintained by Itay Avraham (itayavr@nvidia.com), please send any comments, suggestions, or questions to me.

## How to compile and load the driver.
Build the driver by using the make command and load the driver using the insmod command:<br/>
- Build and load driver for MSTFLint

  ```
  make
  insmode insmod mst_driver.ko
  ```

- Build and load driver for MFT

  ```
  make
  insmod mst_driver.ko mft_package=1
  ```
 
## Requirements

- MFT version >=4.20.x
- MSTFlint version >=4.20.x

## How to access from user space<br/>
IOCTL system call will be the communication channel between user space and kernel space.<br/>
Here is a short explanation regarding the IOCTLS codes:<br/>


| IOCTL Code                         | Explanation                                            |
| ---------------------------------- | :-----------------------------------------------------|
| `MST_INIT`                         | Used to change bar number and map the new bar on the fly |
| `MST_STOP`                         | Used to change configuration registers on the fly        |
| `MST_GET_DEVICE_PARAMETERS`        | Get information about the Mellanox device                |
| `MST_WRITE`                        | Write a block of data to PCI memory or configuration space,<br/> Size is expressed as number of unsigned integers  |
| `MST_READ`                         | Read a block of data from PCI memory or configuration space,<br/> Size is expressed as number of unsigned integers |
| `MST_VPD_READ`                     | Reads PCI device VPD                                     |
| `MST_VPD_WRITE`                    | Writes PCI device VPD                                    |
| `MST_GET_DMA_PAGES`                | Pin user space memory into the kernel space for DMA purpose,<br/> User space application should allocate buffer as number of pages |
| `MST_RELEASE_DMA_PAGES`            | Unpin the user space memory                              |
| `MST_READ_DWORD_FROM_CONFIG_SPACE` | Read a dword from the PCI configuation space             |
| `MST_PCI_CONNECTX_WA`              | ConnectX ordering workaround                             |


## FAQ: Here are some questions that I sometimes get that might help.
- Q: What is the major number of the driver?<br/>
  - A: The major number allocated dynamically, no fixed number.<br/>
