#include"hawkInit.h"
#include<iostream>

int main() {
	owl::hawk h1(1);
  void *ptr_h, *ptr_d;
  ptr_h = h1.memAllocCPU(0, 1024);
  ptr_d = h1.memAllocGPU(0, 1024);
  void *ptr_h1 = h1.memAllocCPU(1, 1024);
  std::cout<<(uint64_t)ptr_h<<std::endl;
  std::cout<<(uint64_t)ptr_d<<std::endl;
  std::cout<<h1.memCpyCpuToGpu(ptr_d, ptr_h, 1024)<<std::endl;
  std::cout<<h1.memCpyCpuToGpu(ptr_d, ptr_h1, 1024)<<std::endl;
}
