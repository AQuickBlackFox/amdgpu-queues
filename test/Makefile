CC=g++
HAWK_INC=/home/aditya/rocm/amdgpu-queues/include/detail
HAWK_SRC=/home/aditya/rocm/amdgpu-queues/src
HSA_DIR=/opt/rocm/hsa
ROCM_DIR=/opt/rocm

all: test1

test1:
	$(CC) -I$(HAWK_INC) -I$(HSA_DIR)/include -std=c++11 $(HAWK_SRC)/hawkInit.cpp test1.cpp -L $(ROCM_DIR)/lib -lhsa-runtime64 -o test1

clean:
	rm test1
