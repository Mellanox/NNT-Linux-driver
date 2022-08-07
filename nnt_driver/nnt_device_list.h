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
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 41692) }, /* BlueField 3    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 41694) }, /* BlueField 4    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 53120) }, /* Spectrum 4     */
        { 0, }
};


static struct pci_device_id livefish_pci_devices[] = {
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01f6) }, /* ConnectX-3 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01f8) }, /* ConnectX-3 Pro Flash Recovery         */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x01ff) }, /* ConnectX-IB Flash Recovery            */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0209) }, /* ConnectX-4 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020b) }, /* ConnectX-4Lx Flash Recovery           */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020d) }, /* ConnectX-5 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x020f) }, /* ConnectX-6 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0210) }, /* ConnectX-6 Secure Flash Recovery      */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0211) }, /* BlueField Flash Recovery              */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0212) }, /* ConnectX-6 Dx Flash Recovery          */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0213) }, /* ConnectX-6 Dx Secure Flash Recovery   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0214) }, /* BlueField-2 SoC Flash Recovery        */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0215) }, /* BlueField-2 Secure Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0216) }, /* ConnectX-6LX Flash Recovery           */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0217) }, /* ConnectX-6 Lx Secure Flash Recovery   */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0218) }, /* ConnectX-7 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0219) }, /* ConnectX-7 Secure Flash Recovery      */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021a) }, /* BlueField-3 Lx SoC Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021b) }, /* BlueField-3 Lx Secure Flash Recovery  */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021c) }, /* BlueField-3 SoC Flash Recovery        */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021d) }, /* BlueField-3 Secure Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021e) }, /* ConnectX-8 Flash Recovery             */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021e) }, /* ConnectX-8 Secure Flash Recovery      */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x0220) }, /* BlueField-4 SoC Flash Recovery        */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x021e) }, /* BlueField-4 Secure Flash Recovery     */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 0x024e) }, /* Spectrum-4 Secure Flash Recovery     */
        { 0, }
};


static struct pci_device_id bar_pci_devices[] = {
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4099) }, /* ConnectX-3    */
        { PCI_DEVICE(NNT_NVIDIA_PCI_VENDOR, 4103) }, /* ConnectX-3Pro */
        { 0, }
};

#endif
