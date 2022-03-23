#ifndef MST_IOCTL_H
#define MST_IOCTL_H

#include "mst_ioctl_defs.h"
#include "mst_device_defs.h"

int map_dma_pages(struct page_info* page_info, struct mst_device* mst_device);
int release_dma_pages(struct page_info* page_info, struct mst_device* mst_device);

#endif
