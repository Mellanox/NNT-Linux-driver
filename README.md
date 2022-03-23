# NTT-Linux-driver
This repository is the kernel space part of [MFT](https://network.nvidia.com/products/adapter-software/firmware-tools/) & [MSTFlint](https://github.com/Mellanox/mstflint) packages aimed to provide a stable and well defined interface for NVIDIA® networking devices that are accessed via PCI Memory Mapping or by PCI configuration cycles.<br/>
The purpose of this driver is to provide a lightweight interface between kernel and userspace for firmware management and debug tools for NVIDIA® networking devices.<br/>
The API is described in section [Access from user space](#access-from-user-space).

This repository is maintained by [Itay Avraham](mailto:itayavr@nvidia.com?subject=[GitHub]%20NTT%20Linux%20driver), please send any comments, suggestions, or questions to the maintainer.

## How to compile and load the driver.
Build the driver by using the make command and load the driver using the insmod command:<br/>
- Build and load driver for MSTFLint

  ```
  make
  insmode insmod ntt_driver.ko
  ```

- Build and load driver for MFT

  ```
  make
  insmod ntt_driver.ko mft_package=1
  ```
 
## Requirements

- Userspace packages versions:
  - MFT version >=4.20.x
  - MSTFlint version >=4.20.x
- The minimal GCC version should be 4.8.x.

## Access from user space<br/>
IOCTL system call will be the communication channel between userspace and kernel space.<br/>
Here is a short explanation regarding the IOCTLS codes:<br/>


| IOCTL Code                         | Explanation                                            |
| ---------------------------------- | :-----------------------------------------------------|
| `NTT_INIT`                         | Used to change bar number and map the new bar on the fly |
| `NTT_STOP`                         | Used to change configuration registers on the fly        |
| `NTT_GET_DEVICE_PARAMETERS`        | Get information about the NVIDIA® networking device      |
| `NTT_WRITE`                        | Write a block of data to PCI memory or configuration space,<br/> Size is expressed as number of unsigned integers  |
| `NTT_READ`                         | Read a block of data from PCI memory or configuration space,<br/> Size is expressed as number of unsigned integers |
| `NTT_VPD_READ`                     | Reads PCI device VPD                                     |
| `NTT_VPD_WRITE`                    | Writes PCI device VPD                                    |
| `NTT_GET_DMA_PAGES`                | Pin user space memory into the kernel space for DMA purpose,<br/> User space application should allocate buffer as number of pages |
| `NTT_RELEASE_DMA_PAGES`            | Unpin the user space memory                              |
| `NTT_READ_DWORD_FROM_CONFIG_SPACE` | Read a dword from the PCI configuation space             |
| `NTT_PCI_CONNECTX_WA`              | ConnectX ordering workaround                             |


## FAQ: Here are some questions that I sometimes get that might help.
- Q: What is the driver major number of the driver?<br/>
  - A: The driver major number allocated dynamically, no fixed number.<br/>
- Q: Which communication channel is preferred?<br>
  - A: PCI configuration access is slower and less safe than memory access.<br/>
