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
#include <mutex>

#include <hsa/hsa.h>
#include <hsa/hsa_ext_amd.h>

namespace owl {

/**
Region and pool tracker
*/
  class snowy {
    public:
    hsa_agent_t Agent;
    std::vector<hsa_region_t> KernArgRegions;
    std::vector<hsa_region_t> FineGrainRegions;
    std::vector<hsa_amd_memory_pool_t> Pools;
    std::vector<uint32_t> PeerListGPU;
    std::vector<uint32_t> PeerListCPU;
    snowy(hsa_agent_t agent);
    ~snowy() {}
  };

/**
Memory tracker
*/
  class horned {
  private:
    bool FindGPUPtr(void* Ptr, uint32_t gpuID);
    bool FindCPUPtr(void* Ptr, uint32_t cpuID);
  public:
    std::map<uint32_t, std::map<void*,size_t>> CpuPoolPtrMap;
    std::map<uint32_t, std::map<void*,size_t>> GpuPoolPtrMap;
    std::map<uint32_t, size_t> GpuUsedPoolSize;
    std::map<uint32_t, size_t> CpuUsedPoolSize;
    void AddPoolGpuPtr(uint32_t GpuID, void* GpuPtr, size_t Size);
    void AddPoolCpuPtr(uint32_t CpuID, void* CpuPtr, size_t Size);
    int32_t GetGPUFromPtr(void* Ptr);
    int32_t GetCPUFromPtr(void* Ptr);
    size_t GetGPUSizeFromPtr(void* Ptr);
    size_t GetCPUSizeFromPtr(void* Ptr);
    bool RemovePoolGPUPtr(uint32_t GpuID, void* Ptr);
    bool RemovePoolCPUPtr(uint32_t CpuID, void* Ptr);
    void PrintCpuPtr();
    void PrintGpuPtr();
    horned() {}
    ~horned();
  };

  /**
  Signal tracker
  */
  class screech {
  private:
    std::mutex Mutex;
    uint32_t TotalSignals;
    std::vector<hsa_signal_t> Signals;
    std::vector<uint32_t> Tracker;
  public:
    hsa_signal_t AcquireSignal();
    bool ReleaseSignal(hsa_signal_t);
    void WaitOnSignal(hsa_signal_t);
    void WaitOnAllSignals();
    screech(uint32_t numSignals);
    screech();
    ~screech();
  };

static screech SignalTracker(1024);

/**
Copy Engine + Kernel Execution
*/
  class burrowing {
  private:
    std::mutex Mutex;
    hsa_queue_t* Queue;
  public:
    burrowing();
    bool LaunchKernel();
    ~burrowing();
  };



  class hawk {
    private:
      std::vector<owl::snowy> Gpus;
      std::vector<owl::snowy> Cpus;
      uint32_t NumGPUs;
      uint32_t NumCPUs;
      owl::horned MemTracker;
      hsa_status_t FindGPUs();
      hsa_status_t FindCPUs();
      hsa_status_t FindAgentAccessPattern();
      hsa_status_t FindPoolAccessPattern();
    public:
      hawk(int);
      hawk(){}
      void *AllocatePoolGPU(uint32_t GpuID, size_t size);
      void *AllocatePoolCPU(uint32_t CpuID, size_t size);
      bool FreePoolGPU(void* Ptr);
      bool FreePoolCPU(void* Ptr);
      bool AllowPeerAccess(uint32_t gpu1, uint32_t gpu2);
      uint32_t MemCpyCpuPoolToGpuPool(void *dst, void *src, size_t size);
      uint32_t MemCpyGpuPoolToCpuPool(void *dst, void *src, size_t size);
      uint32_t MemCpyGpuPoolToGpuPool(void *dst, void *src, size_t size);
      void *GetDevicePointer(void* Ptr, uint32_t GpuID);
      ~hawk() ;
  };
}
