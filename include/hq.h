#ifndef AMDGPU_HQ_H
#define AMDGPU_HQ_H

typedef unsigned hqError;
typedef struct __hqQueue *hqQueue;

hqError hqInit();

hqError hqCreateQueue(hqQueue*);

hqError hqDestroyQueue(hqQueue);

hqError hqMemoryCopy3(hqQueue, hqDevice, hqDevice, hqDevice, void*, void*, size_t);

hqError hqMemoryCopy2(hqDevice, hqDevice, void*, void*, size_t);

hqError hqLaunchKernel(hqKernel, unsigned, unsigned, unsigned,
                        unsigned, unsigned, unsigned,
                        size_t, void**, size_t);

hqError hqClose();

#endif
