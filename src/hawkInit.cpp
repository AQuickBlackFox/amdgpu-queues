#include "hawkInit.h"
#include <iostream>

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
  agent = _agent;
  hsa_region_t kernarg_region;
  kernarg_region.handle = (uint64_t) - 1;
  hsa_status_t err = hsa_agent_iterate_regions(agent, get_kernarg_memory_region, &kernarg_region);
  kern_arg = vKernArgRegions;
  for(unsigned j=0;j<vKernArgRegions.size();j++) {
    std::cout<<vKernArgRegions[j].handle<<" "<<kern_arg[j].handle<<std::endl;
  }
  vKernArgRegions.clear();

  hsa_region_t finegrain_region;
  err = hsa_agent_iterate_regions(agent, get_fine_grained_memory_region, &finegrain_region);
  fine_grain = vFineGrainRegions;
  for(unsigned j=0;j<vFineGrainRegions.size();j++) {
    std::cout<<vFineGrainRegions[j].handle<<" "<<fine_grain[j].handle<<std::endl;
  }
  vFineGrainRegions.clear();

  hsa_amd_memory_pool_t pool;
  pool.handle = (uint64_t) - 1;
  err = hsa_amd_agent_iterate_memory_pools(agent, get_memory_pool, &pool);
  pools = vMemoryPool;
  for(unsigned j=0;j<vMemoryPool.size();j++) {
    std::cout<<vMemoryPool[j].handle<<" "<<pools[j].handle<<std::endl;
  }
  vMemoryPool.clear();
}

owl::hawk::hawk(int val) {
  hsa_init();
  findGPUs();
  findCPUs();
  findPoolAccessPattern();
  numGPUs = gpus.size();
  numCPUs = cpus.size();
}

owl::hawk::~hawk() {
  hsa_shut_down();
}

hsa_status_t owl::hawk::findGPUs() {
	hsa_agent_t agent;
	hsa_status_t err = hsa_iterate_agents(get_gpu_agent, &agent);
  for(unsigned i=0;i<vAgents.size();i++){
    owl::snowy snow(vAgents[i]);
    char name[64] = {0};
    hsa_agent_get_info(snow.agent, HSA_AGENT_INFO_NAME, name);
    std::cout<<name<<std::endl;
    gpus.push_back(snow);
  }
  vAgents.clear();
}


hsa_status_t owl::hawk::findCPUs() {
  hsa_agent_t agent;
  hsa_status_t err = hsa_iterate_agents(get_cpu_agent, &agent);
  for(unsigned i=0;i<vAgents.size(); i++) {
    owl::snowy snow(vAgents[i]);
    char name[64] = {0};
    hsa_agent_get_info(snow.agent, HSA_AGENT_INFO_NAME, name);
    std::cout<<name<<std::endl;
    cpus.push_back(snow);
  }
  vAgents.clear();
}

hsa_status_t owl::hawk::findAgentAccessPattern() {
  
}

hsa_status_t owl::hawk::findPoolAccessPattern() {
  for(unsigned i=0;i<cpus.size();i++) {
    for(unsigned j=0;j<cpus[i].pools.size();j++) {
      uint32_t ret;
      hsa_amd_agent_memory_pool_get_info(cpus[i].agent, cpus[i].pools[j], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
      std::cout<<"CPU: "<<i<<" CPU Pool: "<<j<<": "<<ret<<std::endl;
    }
    for(unsigned j=0;j<gpus.size();j++) {
      for(unsigned k=0;k<gpus[j].pools.size();k++) {
        uint32_t ret;
        hsa_amd_agent_memory_pool_get_info(cpus[i].agent, gpus[j].pools[k], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
        std::cout<<"CPU: "<<i<<" GPU: "<<j<<" Pool: "<<k<<" : "<<ret<<std::endl;
      }
    }
  }

  for(unsigned i=0;i<gpus.size();i++) {
    for(unsigned j=0;j<gpus[i].pools.size();j++) {
      uint32_t ret;
      hsa_amd_agent_memory_pool_get_info(gpus[i].agent, gpus[i].pools[j], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
      std::cout<<"GPU: "<<i<<" GPU Pool: "<<j<<": "<<ret<<std::endl;

    }
    for(unsigned j=0;j<cpus.size();j++) {
      for(unsigned k=0;k<cpus[j].pools.size();k++) {
        uint32_t ret;
        hsa_amd_agent_memory_pool_get_info(gpus[i].agent, cpus[j].pools[k], HSA_AMD_AGENT_MEMORY_POOL_INFO_ACCESS, &ret);
        std::cout<<"GPU: "<<i<<" CPU: "<<j<<" Pool: "<<k<<" : "<<ret<<std::endl;
      }
    }
  }
}

void* owl::hawk::memAllocGPU(uint32_t gpuID, size_t size) {
  if(gpuID >= numGPUs || size == 0) {
    return nullptr;
  }
  void *ptr;
  if(gpus[gpuID].pools.size() > 0) {
    hsa_amd_memory_pool_allocate(gpus[gpuID].pools[0], size, 0, &ptr);
    memTracker.addGpuPtr(gpuID, ptr, size);
//    memTracker.printGpuPtr();
    return ptr;
  } else {
    return nullptr;
  }
}

void* owl::hawk::memAllocCPU(uint32_t cpuID, size_t size) {
  void *ptr;
  if(cpus[cpuID].pools.size() > 0) {
    hsa_amd_memory_pool_allocate(cpus[cpuID].pools[0], size, 0, &ptr);
    memTracker.addCpuPtr(cpuID, ptr, size);
//    memTracker.printCpuPtr();
    return ptr;
  } else {
    return nullptr;
  }
}


uint32_t owl::hawk::memCpyCpuToGpu(void *dst, void *src, size_t size) {
  uint32_t gpuID=UINT32_MAX, cpuID=UINT32_MAX;
  for(unsigned i=0;i<memTracker.gpuPtrMap.size();i++){
    if(std::find(memTracker.gpuPtrMap[i].begin(), memTracker.gpuPtrMap[i].end(), dst) != memTracker.gpuPtrMap[i].end()) {
      gpuID = i;
      break;
    }
  }
  for(unsigned i=0;i<memTracker.cpuPtrMap.size();i++){
    if(std::find(memTracker.cpuPtrMap[i].begin(), memTracker.cpuPtrMap[i].end(), src) != memTracker.cpuPtrMap[i].end()) {
      cpuID = i;
      break;
    }
  }
  if(gpuID != UINT32_MAX && cpuID != UINT32_MAX) {
  }else {
    return UINT32_MAX;
  }
  std::cout<<gpuID<<" "<<cpuID<<std::endl;
}

void owl::horned::addCpuPtr(uint32_t cpuID, void* cpuPtr, size_t size) {
  cpuPtrMap[cpuID].push_back(cpuPtr);
  cpuSize[cpuID]+=size;
}

void owl::horned::addGpuPtr(uint32_t gpuID, void* gpuPtr, size_t size) {
  gpuPtrMap[gpuID].push_back(gpuPtr);
  gpuSize[gpuID]+=size;
}

void owl::horned::printCpuPtr() {
  for(unsigned i=0;i<cpuPtrMap.size();i++) {
    for(void* n : cpuPtrMap[i]) {
      std::cout<<(uint64_t)n<<std::endl;
    }
  }
}

void owl::horned::printGpuPtr() {
  for(unsigned i=0;i<gpuPtrMap.size();i++) {
    for(void* n : gpuPtrMap[i]) {
      std::cout<<(uint64_t)n<<std::endl;
    }
  }
}

owl::horned::~horned() {
  for(unsigned i=0;i<cpuPtrMap.size();i++) {
    for(void* n : cpuPtrMap[i]) {
      hsa_amd_memory_pool_free(n);
    }
  }
  for(unsigned j=0;j<gpuPtrMap.size();j++) {
    for(void* n : gpuPtrMap[j]) {
      hsa_amd_memory_pool_free(n);
    }
  }
}

