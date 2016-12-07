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


// 1 means available 0 means being-used
owl::screech::screech(uint32_t NumSignals) : TotalSignals(NumSignals) {
  hsa_signal_t signal;
  for(unsigned i=0;i<TotalSignals;i++) {
    hsa_signal_create(1, 0, NULL, &signal);
    Signals.push_back(signal);
    Tracker.push_back(1);
  }
}

owl::screech::screech() {
  hsa_signal_t signal;
  uint32_t TotalSignal = 1024;
  for(unsigned i=0;i<TotalSignals;i++) {
    hsa_signal_create(1, 0, NULL, &signal);
    Signals.push_back(signal);
    Tracker.push_back(1);
  }
}

hsa_signal_t owl::screech::AcquireSignal() {
  std::lock_guard<std::mutex> lock(Mutex);
  for(unsigned i=0;i<TotalSignals;i++) {
    if(Tracker[i] == 1){
      Tracker[i] = 0;
      return Signals[i];
    }
  }
}

bool owl::screech::ReleaseSignal(hsa_signal_t signal) {
  std::lock_guard<std::mutex> lock(Mutex);
  for(unsigned i=0;i<TotalSignals;i++){
    if(Tracker[i] == 0){
      Tracker[i] = 1;
      hsa_signal_wait_acquire(Signals[i], HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
      hsa_signal_store_relaxed(Signals[i],1);
      return true;
    }
  }
  return false;
}

void owl::screech::WaitOnSignal(hsa_signal_t signal) {
  std::lock_guard<std::mutex> lock(Mutex);
  hsa_signal_wait_acquire(signal, HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
  hsa_signal_store_relaxed(signal,1);
  for(uint32_t i=0;i<TotalSignals;i++) {
    if(signal.handle == Signals[i].handle){
      Tracker[i] = 1;
    }
  }
}

void owl::screech::WaitOnAllSignals(){
  std::lock_guard<std::mutex> lock(Mutex);
  for(uint32_t i=0;i<TotalSignals;i++){
    hsa_signal_wait_acquire(Signals[i], HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
    hsa_signal_store_relaxed(Signals[i],1);
    Tracker[i] = 1;
  }
}

owl::screech::~screech(){
  for(uint32_t i=0;i<TotalSignals;i++){
    if(Tracker[i] == 0) {
      hsa_signal_wait_acquire(Signals[i], HSA_SIGNAL_CONDITION_LT, 1, UINT64_MAX, HSA_WAIT_STATE_BLOCKED);
    }
    hsa_signal_destroy(Signals[i]);
  }
}
