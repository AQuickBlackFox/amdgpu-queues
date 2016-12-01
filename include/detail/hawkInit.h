#pragma once

#include <vector>
#include <hsa/hsa.h>

namespace owl {
  class hawk {
    private:
      hsa_status_t getGPUs();
    public:
      hawk(int);
      ~hawk();
  };
}
