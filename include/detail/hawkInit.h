#pragma once

#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <hsa/hsa.h>
#include <hsa/hsa_ext_amd.h>

namespace owl {

  class snowy {
    public:
    hsa_agent_t agent;
    std::vector<hsa_region_t> kern_arg;
    std::vector<hsa_region_t> fine_grain;
    std::vector<hsa_amd_memory_pool_t> pools;
    snowy(hsa_agent_t _agent);
    ~snowy() {}
  };

  class horned {
    public:
    std::map<uint32_t, std::list<void*>> cpuPtrMap;
    std::map<uint32_t, std::list<void*>> gpuPtrMap;
    std::map<uint32_t,size_t> gpuSize;
    std::map<uint32_t,size_t> cpuSize;
    void addCpuPtr(uint32_t cpuID, void* cpuPtr, size_t size);
    void addGpuPtr(uint32_t gpuID, void* gpuPtr, size_t size);
    void printCpuPtr();
    void printGpuPtr();
    horned() {}
    ~horned();
  };

  class screech {
    
  };

  class hawk {
    private:
      std::vector<owl::snowy> gpus;
      std::vector<owl::snowy> cpus;
      uint32_t numGPUs;
      uint32_t numCPUs;
      owl::horned memTracker;
      hsa_status_t findGPUs();
      hsa_status_t findCPUs();
      hsa_status_t findAgentAccessPattern();
      hsa_status_t findPoolAccessPattern();
    public:
      hawk(int);
      void *memAllocGPU(uint32_t gpuID, size_t size);
      void *memAllocCPU(uint32_t cpuID, size_t size);
      uint32_t memCpyCpuToGpu(void *dst, void *src, size_t size);
      uint32_t getGpuIdByPtr(void *ptr);
      uint32_t getCpuIdByPtr(void *ptr);
      ~hawk() ;
  };
}
