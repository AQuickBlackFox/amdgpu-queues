/*
Copyright (c) 2016 Aditya Atluri. All rights reserved.
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <hsa/hsa.h>
#include <hsa/hsa_ext_amd.h>

namespace owl {

/**
Region and pool tracker
*/
  class snowy {
    public:
    hsa_agent_t agent;
    std::vector<hsa_region_t> kern_arg;
    std::vector<hsa_region_t> fine_grain;
    std::vector<hsa_amd_memory_pool_t> pools;
    snowy(hsa_agent_t _agent);
    ~snowy() {}
  };

/**
Memory tracker
*/
  class horned {
    public:
    std::map<uint32_t, std::list<void*>> cpuPtrMap;
    std::map<uint32_t, std::list<void*>> gpuPtrMap;
    std::map<uint32_t, size_t> gpuSize;
    std::map<uint32_t, size_t> cpuSize;
    void addCpuPtr(uint32_t cpuID, void* cpuPtr, size_t size);
    void addGpuPtr(uint32_t gpuID, void* gpuPtr, size_t size);
    void printCpuPtr();
    void printGpuPtr();
    horned() {}
    ~horned();
  };

/**
Signal tracker
*/
  class screech {
  private:
    uint32_t totalSignal;
    std::vector<hsa_signal_t> signals;
  public:
    screech(uin32_t);
    screech() {}
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
