#include "hawkInit.h"
#include <iostream>

static std::vector<hsa_agent_t> vAgents;

hsa_status_t get_gpu_agent(hsa_agent_t agent, void *data) {
	hsa_status_t status;
	hsa_device_type_t device_type;
	status = hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device_type);
	if(HSA_STATUS_SUCCESS == status && HSA_DEVICE_TYPE_GPU == device_type) {
		hsa_agent_t* ret = (hsa_agent_t*)data;
    char name[64] = {0};
		hsa_agent_get_info(agent, HSA_AGENT_INFO_NAME, name);
		vAgents.push_back(agent);
		*ret = agent;
	}
	return HSA_STATUS_SUCCESS;
}

owl::hawk::hawk(int val) {
	std::cout<<val<<std::endl;
	hsa_init();
	std::cout<<"In init"<<std::endl;
	getGPUs();
}

owl::hawk::~hawk() {}

hsa_status_t owl::hawk::getGPUs() {
	std::cout<<"In getGPUs"<<std::endl;
	hsa_agent_t agent;
	hsa_status_t err = hsa_iterate_agents(get_gpu_agent, &agent);
	if(err == HSA_STATUS_INFO_BREAK) {
		err = HSA_STATUS_SUCCESS;
	} else {
		err = HSA_STATUS_ERROR;
	}
	char name[64] = {0};
	std::cout<<vAgents.size()<<std::endl;
	for(unsigned i =0 ; i<vAgents.size();i++) {
	err = hsa_agent_get_info(vAgents[i], HSA_AGENT_INFO_NAME, name);
	std::cout<<name<<std::endl;
	}
}
