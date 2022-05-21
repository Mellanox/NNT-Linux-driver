#ifndef NNT_DEVICE_LIST_H
#define NNT_DEVICE_LIST_H

#define NNT_NVIDIA_PCI_VENDOR       0x15b3

static struct pci_device_id pciconf_devices[] = {
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4099) },  /* ConnectX-3     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4103) },  /* ConnectX-3Pro  */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4113) },  /* ConnectX-IB    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4115) },  /* ConnectX-4     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4117) },  /* ConnectX-4Lx   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4119) },  /* ConnectX-5     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4121) },  /* ConnectX-5EX   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4123) },  /* ConnectX-6     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4125) },  /* ConnectX-6DX   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4127) },  /* ConnectX-6LX   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4129) },  /* ConnectX-7     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 41682) }, /* BlueField      */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 41686) }, /* BlueField 2    */
        { 0, }
};


static struct pci_device_id livefish_pci_devices[] = {
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01f6) }, /* ConnectX-3 Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01f8) }, /* ConnectX-3 Pro Flash Recovery */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01ff) }, /* ConnectX-IB Flash Recovery    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0209) }, /* ConnectX-4 Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020b) }, /* ConnectX-4Lx Flash Recovery   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020d) }, /* ConnectX-5 Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020f) }, /* ConnectX-6 Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0212) }, /* ConnectX-6DX Flash Recovery   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0216) }, /* ConnectX-6LX Flash Recovery   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0218) }, /* ConnectX-7 Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0211) }, /* BlueField Flash Recovery      */
        { 0, }
};


static struct pci_device_id bar_pci_devices[] = {
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4099) }, /* ConnectX-3    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4103) }, /* ConnectX-3Pro */
        { 0, }
};

#endif
