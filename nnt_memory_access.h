#ifndef NNT_MEMORY_ACCESS_H
#define NNT_MEMORY_ACCESS_H

int read_memory(struct nnt_device* nnt_device, struct rw_operation* read_operation);
int write_memory(struct nnt_device* nnt_device, struct rw_operation* write_operation);
int init_memory(unsigned int command, void* user_buffer,
                struct nnt_device* nnt_device);
#endif
