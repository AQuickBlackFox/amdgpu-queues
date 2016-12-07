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

owl::horned::~horned() {
	for(uint32_t i=0;i<CpuPoolPtrMap.size();i++){
		for(auto iter = CpuPoolPtrMap[i].begin(); iter != CpuPoolPtrMap[i].end(); ++iter) {
			hsa_amd_memory_pool_free(iter->first);
		}
	}

	for(uint32_t i=0;i<GpuPoolPtrMap.size();i++){
		for(auto iter = GpuPoolPtrMap[i].begin(); iter != GpuPoolPtrMap[i].end(); ++iter) {
			hsa_amd_memory_pool_free(iter->first);
		}
	}
}


bool owl::horned::FindGPUPtr(void* Ptr, uint32_t gpuID) {
	for(auto iter = GpuPoolPtrMap[gpuID].begin(); iter != GpuPoolPtrMap[gpuID].end(); ++iter) {
		if(iter->first == Ptr){
			return true;
		}
	}
	return false;
}

bool owl::horned::FindCPUPtr(void* Ptr, uint32_t cpuID) {
	for(auto iter = CpuPoolPtrMap[cpuID].begin(); iter != CpuPoolPtrMap[cpuID].end(); ++iter) {
		if(iter->first == Ptr){
			return true;
		}
	}
	return false;
}

void owl::horned::AddPoolGpuPtr(uint32_t GpuID, void *GpuPtr, size_t Size) {
	GpuPoolPtrMap[GpuID].insert(std::pair<void*,size_t>(GpuPtr, Size));
	GpuUsedPoolSize[GpuID] += Size;
}

void owl::horned::AddPoolCpuPtr(uint32_t CpuID, void *CpuPtr, size_t Size) {
	CpuPoolPtrMap[CpuID].insert(std::pair<void*,size_t>(CpuPtr, Size));
	CpuUsedPoolSize[CpuID] += Size;
}

int32_t owl::horned::GetGPUFromPtr(void *Ptr) {
	for(uint32_t i=0;i< GpuPoolPtrMap.size(); i++){
		for(auto iter = GpuPoolPtrMap[i].begin(); iter != GpuPoolPtrMap[i].end(); ++iter) {
			if(iter->first == Ptr) {
				return i;
			}
		}
	}
	return -1;
}

int32_t owl::horned::GetCPUFromPtr(void *Ptr) {
	for(uint32_t i=0;i< CpuPoolPtrMap.size(); i++){
		for(auto iter = CpuPoolPtrMap[i].begin(); iter != CpuPoolPtrMap[i].end(); ++iter) {
			if(iter->first == Ptr) {
				return i;
			}
		}
	}
	return -1;
}

size_t owl::horned::GetGPUSizeFromPtr(void* Ptr){
	for(uint32_t i=0;i< GpuPoolPtrMap.size(); i++){
		for(auto iter = GpuPoolPtrMap[i].begin(); iter != GpuPoolPtrMap[i].end(); ++iter) {
			if(iter->first == Ptr) {
				return iter->second;
			}
		}
	}
	return 0;
}


size_t owl::horned::GetCPUSizeFromPtr(void* Ptr){
	for(uint32_t i=0;i< CpuPoolPtrMap.size(); i++){
		for(auto iter = CpuPoolPtrMap[i].begin(); iter != CpuPoolPtrMap[i].end(); ++iter) {
			if(iter->first == Ptr) {
				return iter->second;
			}
		}
	}
	return 0;
}

bool owl::horned::RemovePoolGPUPtr(uint32_t GpuID, void* Ptr){
	for(auto iter = GpuPoolPtrMap[GpuID].begin(); iter != GpuPoolPtrMap[GpuID].end(); ++iter) {
		if(iter->first == Ptr) {
			GpuUsedPoolSize[GpuID] -= iter->second;
			GpuPoolPtrMap[GpuID].erase(Ptr);
			return true;
		}
	}
	return false;
}

bool owl::horned::RemovePoolCPUPtr(uint32_t CpuID, void* Ptr){
	for(auto iter = CpuPoolPtrMap[CpuID].begin(); iter != CpuPoolPtrMap[CpuID].end(); ++iter) {
		if(iter->first == Ptr) {
			CpuUsedPoolSize[CpuID] -= iter->second;
			CpuPoolPtrMap[CpuID].erase(Ptr);
			return true;
		}
	}
	return false;
}
