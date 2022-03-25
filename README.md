# NNT-Linux-driver (NVIDIA® networking tools driver)
This repository is the kernel space part of [MFT](https://network.nvidia.com/products/adapter-software/firmware-tools/) & [MSTFlint](https://github.com/Mellanox/mstflint) packages aimed to provide a stable and well defined interface for NVIDIA® networking devices that are accessed via PCI Memory Mapping or by PCI configuration cycles.<br/>
The purpose of this driver is to provide a lightweight interface between kernel and userspace for firmware management and debug tools for NVIDIA® networking devices.<br/>
The API is described in section [Access from user space](#access-from-user-space).

This repository is maintained by [Itay Avraham](mailto:itayavr@nvidia.com?subject=[GitHub]%20NNT%20Linux%20driver), please send any comments, suggestions, or questions to the maintainer.

## How to compile and load the driver.
Build the driver by using the "make" command and load the driver by using the "insmod" command:<br/>
- Build and load driver for MSTFLint

  ```
  make
  insmode insmod nnt_linux_driver.ko
  ```

- Build and load driver for MFT

  ```
  make
  insmod nnt_linux_driver.ko mft_package=1
  ```
 
## Requirements

- Userspace packages versions:
  - MFT version >=4.20.x
  - MSTFlint version >=4.20.x
- The minimal GCC version should be 4.8.x.

## Access from user space<br/>
IOCTL system call will be the communication channel between the userspace and the kernel space.<br/>
Here is a short explanation regarding the IOCTLS codes:<br/>


| IOCTL Code                         | Explanation                                            |
| ---------------------------------- | :-----------------------------------------------------|
| `NNT_INIT`                         | Changes the bar number and map the new bar on the fly |
| `NNT_STOP`                         | Change configuration registers on the fly             |
| `NNT_GET_DEVICE_PARAMETERS`        | Pulls information about the NVIDIA® networking device |
| `NNT_WRITE`                        | Writes a block of data to the PCI memory or the configuration space,<br/> Size is expressed as number of unsigned integers  |
| `NNT_READ`                         | Reads a block of data from the PCI memory or the configuration space,<br/> Size is expressed as number of unsigned integers |
| `NNT_VPD_READ`                     | Reads PCI device VPD                                     |
| `NNT_VPD_WRITE`                    | Writes PCI device VPD                                    |
| `NNT_GET_DMA_PAGES`                | Pins the user space memory into the kernel space for DMA purpose,<br/> User space application should allocate buffer as number of pages |
| `NNT_RELEASE_DMA_PAGES`            | Unpin the user space memory                              |
| `NNT_READ_DWORD_FROM_CONFIG_SPACE` | Reads a dword from the PCI configuation space             |
| `NNT_PCI_CONNECTX_WA`              | ConnectX ordering workaround                             |


## FAQ:
- Q: What is the driver major number of the driver?<br/>
  - A: The driver's major version is allocated dynamically, no fixed number.<br/>
- Q: Which communication channel is preferred?<br>
  - A: PCI configuration access is slower and less safe than memory access.<br/>
