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

#include"hawkOwl.h"
#include<iostream>
#include<assert.h>

int main() {
	owl::hawk h1;//(1);
  void *In_h1, *Out_h1, *Out_h2, *Ptr_d1, *Ptr_d2;

  In_h1 = h1.AllocatePoolCPU(0, 1024);
	Out_h1 = h1.AllocatePoolCPU(1, 1024);
	Out_h2 = h1.AllocatePoolCPU(1, 1024);
  Ptr_d1 = h1.AllocatePoolGPU(0, 1024);
	Ptr_d2 = h1.AllocatePoolGPU(1, 1024);

  int *Host1Ptr1 = (int*)In_h1;
	int *Host2Ptr2 = (int*)Out_h2;
	int *Host1Ptr2 = (int*)Out_h1;

	for(unsigned i=0;i<256;i++){
		Host1Ptr1[i] = 11;
		Host2Ptr2[i] = 0;
		Host1Ptr2[i] = 0;
	}
  h1.MemCpyCpuPoolToGpuPool(Ptr_d1, In_h1, 1024);
	h1.MemCpyGpuPoolToCpuPool(Out_h1, Ptr_d1, 1024);
	h1.MemCpyGpuPoolToGpuPool(Ptr_d2, Ptr_d1, 1024);
  h1.MemCpyGpuPoolToCpuPool(Out_h2, Ptr_d2, 1024);

	for(unsigned i=0;i<256;i++){
		std::cout<<Host1Ptr2[i]<<std::endl;
		assert(Host1Ptr1[i] == Host2Ptr2[i]);
	}

}
