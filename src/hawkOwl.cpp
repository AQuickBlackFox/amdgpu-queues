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

#include "hawkOwl.h"
#include <iostream>
#include <assert.h>

static std::vector<hsa_agent_t> vAgents;
static std::vector<hsa_region_t> vKernArgRegions;
static std::vector<hsa_region_t> vFineGrainRegions;
static std::vector<hsa_amd_memory_pool_t> vMemoryPool;

static hsa_status_t get_gpu_agent(hsa_agent_t agent, void *data) {
	hsa_status_t status;
	hsa_device_type_t device_type;
	status = hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device_type);
	if(HSA_STATUS_SUCCESS == status && HSA_DEVICE_TYPE_GPU == device_type) {
		hsa_agent_t* ret = (hsa_agent_t*)data;
		vAgents.push_back(agent);
		*ret = agent;
	}
	return HSA_STATUS_SUCCESS;
}

static hsa_status_t get_cpu_agent(hsa_agent_t agent, void *data) {
	hsa_status_t status;
	hsa_device_type_t device_type;
	status = hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device_type);
	if(HSA_STATUS_SUCCESS == status && HSA_DEVICE_TYPE_CPU == device_type) {
		hsa_agent_t* ret = (hsa_agent_t*)data;
		vAgents.push_back(agent);
		*ret = agent;
	}
	return HSA_STATUS_SUCCESS;
}

static hsa_status_t get_kernarg_memory_region(hsa_region_t region, void *data) {
  hsa_region_segment_t segment;
  hsa_region_global_flag_t flags;
  hsa_region_get_info(region, HSA_REGION_INFO_SEGMENT, &segment);
  hsa_region_get_info(region, HSA_REGION_INFO_GLOBAL_FLAGS, &flags);
  if((segment == HSA_REGION_SEGMENT_GLOBAL) && (flags & HSA_REGION_GLOBAL_FLAG_KERNARG)) {
    hsa_region_t *ret = (hsa_region_t*) data;
    vKernArgRegions.push_back(region);
  }
  return HSA_STATUS_SUCCESS;
}

static hsa_status_t get_fine_grained_memory_region(hsa_region_t region, void *data) {
  uint32_t  flags;
  hsa_region_segment_t segment;
  hsa_region_get_info(region, HSA_REGION_INFO_SEGMENT, &segment);
  hsa_region_get_info(region, HSA_REGION_INFO_GLOBAL_FLAGS, &flags);
  if((segment == HSA_REGION_SEGMENT_GLOBAL) && (flags & HSA_REGION_GLOBAL_FLAG_FINE_GRAINED)) {
    hsa_region_t *ret = (hsa_region_t*) data;
    vFineGrainRegions.push_back(region);
  }
  return HSA_STATUS_SUCCESS;
}

static hsa_status_t get_memory_pool(hsa_amd_memory_pool_t pool, void *data) {
  hsa_status_t err;
  hsa_amd_segment_t segment;
  uint32_t flag;
  err = hsa_amd_memory_pool_get_info(pool, HSA_AMD_MEMORY_POOL_INFO_SEGMENT, &segment);

  if(segment == HSA_AMD_SEGMENT_GLOBAL) {
    *((hsa_amd_memory_pool_t*)data) = pool;
    vMemoryPool.push_back(pool);
  }
  return HSA_STATUS_SUCCESS;
}

owl::snowy::snowy(hsa_agent_t _agent) {
  Agent = _agent;
  hsa_region_t kernarg_region;
  kernarg_region.handle = (uint64_t) - 1;
  hsa_status_t err = hsa_agent_iterate_regions(Agent, get_kernarg_memory_region, &kernarg_region);
  KernArgRegions = vKernArgRegions;
  for(unsigned j=0;j<vKernArgRegions.size();j++) {
    std::cout<<"Kern Arg: "<<vKernArgRegions[j].handle<<" "<<KernArgRegions[j].handle<<std::endl;
  }
  vKernArgRegions.clear();

  hsa_region_t finegrain_region;
  err = hsa_agent_iterate_regions(Agent, get_fine_grained_memory_region, &finegrain_region);
  FineGrainRegions = vFineGrainRegions;
  for(unsigned j=0;j<vFineGrainRegions.size();j++) {
    std::cout<<"Fine Grain: "<<vFineGrainRegions[j].handle<<" "<<FineGrainRegions[j].handle<<std::endl;
  }
  vFineGrainRegions.clear();

  hsa_amd_memory_pool_t pool;
  pool.handle = (uint64_t) - 1;
  err = hsa_amd_agent_iterate_memory_pools(Agent, get_memory_pool, &pool);
  Pools = vMemoryPool;
  for(unsigned j=0;j<vMemoryPool.size();j++) {
    std::cout<<"Pools: "<<vMemoryPool[j].handle<<" "<<Pools[j].handle<<std::endl;
  }
  vMemoryPool.clear();
}

owl::hawk::hawk(int val) {
  hsa_init();
  FindGPUs();
  FindCPUs();
  FindPoolAccessPattern();
  NumGPUs = Gpus.size();
  NumCPUs = Cpus.size();
}

owl::hawk::~hawk() {
  hsa_shut_down();
}

hsa_status_t owl::hawk::FindGPUs() {
	hsa_agent_t agent;
	hsa_status_t err = hsa_iterate_agents(get_gpu_agent, &agent);
  for(unsigned i=0;i<vAgents.size();i++){
    owl::snowy snow(vAgents[i]);
    char name[64] = {0};
    hsa_agent_get_info(snow.Agent, HSA_AGENT_INFO_NAME, name);
    std::cout<<name<<std::endl;
    Gpus.push_back(snow);
  }
  vAgents.clear();
}


hsa_status_t owl::hawk::FindCPUs() {
  hsa_agent_t agent;
  hsa_status_t err = hsa_iterate_agents(get_cpu_agent, &agent);
  for(unsigned i=0;i<vAgents.size(); i++) {
    owl::snowy snow(vAgents[i]);
    char name[64] = {0};
    hsa_agent_get_info(snow.Agent, HSA_AGENT_INFO_NAME, name);
    std::cout<<name<<std::endl;
    Cpus.push_back(snow);
  }
  vAgents.clear();
}

hsa_status_t owl::hawk::FindAgentAccessPattern() {

}

hsa_status_t owl::hawk::FindPoolAccessPattern() {
  for(unsigned i=0;i<Cpus.size();i++) {
    for(unsigned j=0;j<Cpus[i].Pools.size();j++) {
      uint32_t ret;
      hsa_amd_agent_memory_pool_get_info(Cpus[i].Agent, Cpus[i].Pools[j], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
      std::cout<<"CPU: "<<i<<" CPU Pool: "<<j<<": "<<ret<<std::endl;
    }
    for(unsigned j=0;j<Gpus.size();j++) {
      for(unsigned k=0;k<Gpus[j].Pools.size();k++) {
        uint32_t ret;
        hsa_amd_agent_memory_pool_get_info(Cpus[i].Agent, Gpus[j].Pools[k], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
        std::cout<<"CPU: "<<i<<" GPU: "<<j<<" Pool: "<<k<<" : "<<ret<<std::endl;
      }
    }
  }

  for(unsigned i=0;i<Gpus.size();i++) {
    for(unsigned j=0;j<Gpus[i].Pools.size();j++) {
      uint32_t ret;
      hsa_amd_agent_memory_pool_get_info(Gpus[i].Agent, Gpus[i].Pools[j], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
      std::cout<<"GPU: "<<i<<" GPU Pool: "<<j<<": "<<ret<<std::endl;

    }
    for(unsigned j=0;j<Cpus.size();j++) {
      for(unsigned k=0;k<Cpus[j].Pools.size();k++) {
        uint32_t ret;
        hsa_amd_agent_memory_pool_get_info(Gpus[i].Agent, Cpus[j].Pools[k], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
        std::cout<<"GPU: "<<i<<" CPU: "<<j<<" Pool: "<<k<<" : "<<ret<<std::endl;
      }
    }
  }
}


void* owl::hawk::AllocatePoolGPU(uint32_t GpuID, size_t size) {
  if(GpuID >= NumGPUs || size == 0) {
    return nullptr;
  }
  void *ptr;
  if(Gpus[GpuID].Pools.size() > 0) {
    hsa_amd_memory_pool_allocate(Gpus[GpuID].Pools[0], size, 0, &ptr);
    MemTracker.AddPoolGpuPtr(GpuID, ptr, size);
    return ptr;
  } else {
    return nullptr;
  }
}

void* owl::hawk::AllocatePoolCPU(uint32_t CpuID, size_t size) {
	if(CpuID >= NumCPUs || size == 0) {
    return nullptr;
  }
  void *ptr;
  if(Cpus[CpuID].Pools.size() > 0) {
    if(hsa_amd_memory_pool_allocate(Cpus[CpuID].Pools[0], size, 0, &ptr) == HSA_STATUS_SUCCESS) {
    	MemTracker.AddPoolCpuPtr(CpuID, ptr, size);
    	return ptr;
		}
  }
  return nullptr;
}

bool owl::hawk::FreePoolGPU(void* Ptr){
	int32_t GpuID = MemTracker.GetGPUFromPtr(Ptr);
	if(GpuID > 0 && MemTracker.RemovePoolGPUPtr(GpuID, Ptr)) {
		if(hsa_amd_memory_pool_free(Ptr) == HSA_STATUS_SUCCESS){
			return true;
		}
	}
	return false;
}

bool owl::hawk::FreePoolCPU(void* Ptr){
	int32_t CpuID = MemTracker.GetCPUFromPtr(Ptr);
	if(CpuID > 0 && MemTracker.RemovePoolCPUPtr(CpuID, Ptr)) {
		hsa_amd_memory_pool_free(Ptr);
		return true;
	}
	return false;
}

uint32_t owl::hawk::MemCpyCpuPoolToGpuPool(void *dst, void *src, size_t size) {
  uint32_t gpuID=UINT32_MAX, cpuID=UINT32_MAX;
	gpuID = MemTracker.GetGPUFromPtr(dst);
  cpuID = MemTracker.GetCPUFromPtr(src);
  std::cout<<gpuID<<" "<<cpuID<<std::endl;
	hsa_signal_t Signal;
	hsa_signal_create(1, 0, NULL, &Signal);
  if(gpuID != UINT32_MAX && cpuID != UINT32_MAX) {
		hsa_agent_t peers[] = {Gpus[gpuID].Agent, Cpus[cpuID].Agent};
		hsa_amd_agents_allow_access(2, peers, nullptr, dst);
		hsa_amd_agents_allow_access(2, peers, nullptr, src);
		hsa_amd_memory_async_copy(dst, Gpus[gpuID].Agent,
															src, Cpus[cpuID].Agent,
															size, 0, nullptr,Signal);

		hsa_signal_value_t value = hsa_signal_wait_acquire(Signal, HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
		return 1;
  }else {
    return UINT32_MAX;
  }
}

uint32_t owl::hawk::MemCpyGpuPoolToCpuPool(void *dst, void *src, size_t size) {
  uint32_t gpuID=UINT32_MAX, cpuID=UINT32_MAX;
	cpuID = MemTracker.GetCPUFromPtr(dst);
  gpuID = MemTracker.GetGPUFromPtr(src);
  std::cout<<gpuID<<" "<<cpuID<<std::endl;
	hsa_signal_t Signal;
	hsa_signal_create(1, 0, NULL, &Signal);
  if(gpuID != UINT32_MAX && cpuID != UINT32_MAX) {
		hsa_agent_t peers[] = {Gpus[gpuID].Agent, Cpus[cpuID].Agent};
		hsa_amd_agents_allow_access(2, peers, nullptr, dst);
		hsa_amd_agents_allow_access(2, peers, nullptr, src);
		hsa_amd_memory_async_copy(dst, Cpus[cpuID].Agent,
															src, Gpus[gpuID].Agent,
															size, 0, nullptr,Signal);

		hsa_signal_value_t value = hsa_signal_wait_acquire(Signal, HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
		return 1;
  }else {
    return UINT32_MAX;
  }
}

uint32_t owl::hawk::MemCpyGpuPoolToGpuPool(void *dst, void *src, size_t size) {
  uint32_t gpuID1=UINT32_MAX, gpuID2=UINT32_MAX;
	gpuID1 = MemTracker.GetGPUFromPtr(dst);
  gpuID2 = MemTracker.GetGPUFromPtr(src);
  std::cout<<gpuID1<<" "<<gpuID2<<std::endl;
	hsa_signal_t Signal;
	hsa_signal_create(1, 0, NULL, &Signal);
  if(gpuID1 != UINT32_MAX && gpuID2 != UINT32_MAX) {
		hsa_agent_t peers[] = {Gpus[gpuID1].Agent, Gpus[gpuID2].Agent};
		std::cout<<peers[0].handle<<" "<<peers[1].handle<<std::endl;
		assert(HSA_STATUS_SUCCESS == hsa_amd_agents_allow_access(2, peers, nullptr, dst));
		assert(HSA_STATUS_SUCCESS == hsa_amd_agents_allow_access(2, peers, nullptr, src));
		hsa_amd_memory_async_copy(dst, Gpus[gpuID1].Agent,
															src, Gpus[gpuID2].Agent,
															size, 0, nullptr,Signal);

		hsa_signal_value_t value = hsa_signal_wait_acquire(Signal, HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
		return 1;
  }else {
    return UINT32_MAX;
  }
}


void *owl::hawk::GetDevicePointer(void* Ptr, uint32_t GpuID) {
	void *retPtr;
	hsa_agent_t agent = Gpus[GpuID].Agent;
	std::cout<<"GPU Agent handle: "<<agent.handle<<std::endl;
	std::cout<<"HSA Status: "<<hsa_amd_memory_lock(Ptr, MemTracker.GetCPUSizeFromPtr(Ptr), &agent, 1, &retPtr)<<std::endl;
	std::cout<<"CPU Ptr: "<<(uint64_t)Ptr<<" GPU Ptr: "<<(uint64_t)retPtr<<std::endl;
	return retPtr;
}
