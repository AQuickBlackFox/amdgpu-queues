CC=g++
HAWK_INC=/home/aditya/rocm/amdgpu-queues/include/detail
HAWK_SRC=/home/aditya/rocm/amdgpu-queues/src
HSA_DIR=/opt/rocm/hsa
ROCM_DIR=/opt/rocm
SOURCES=$(HAWK_SRC)/burrowingOwl.cpp $(HAWK_SRC)/hawkOwl.cpp $(HAWK_SRC)/hornedOwl.cpp $(HAWK_SRC)/screechOwl.cpp

all: test1 test2

test1:
	$(CC) -I$(HAWK_INC) -I$(HSA_DIR)/include -g -std=c++11 $(SOURCES) test1.cpp -L $(ROCM_DIR)/lib -lhsa-runtime64 -o test1

test2:
	$(CC) -I$(HAWK_INC) -I$(HSA_DIR)/include -g -std=c++11 $(SOURCES) test2.cpp -L $(ROCM_DIR)/lib -lhsa-runtime64 -o test2


clean:
	rm -rf test1 test2
