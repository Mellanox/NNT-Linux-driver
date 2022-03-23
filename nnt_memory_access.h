#ifndef MST_MEMORY_ACCESS_H
#define MST_MEMORY_ACCESS_H

int read_memory(struct mst_device* mst_device, struct rw_operation* read_operation);
int write_memory(struct mst_device* mst_device, struct rw_operation* write_operation);
int init_memory(unsigned int command, void* user_buffer,
                struct mst_device* mst_device);
#endif
