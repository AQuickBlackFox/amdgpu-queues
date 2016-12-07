# amdgpu-queues
Hybrid Queues on top of HSA on AMD GPUs

## Coding convention
```
namespace owl {
  class hawk {
  private:
    std::vector<uint32_t> MemTracker;
  public:
    std::vector<uint32_t> GetMemTracker();
    bool UpdateMemTracker(std::vector<uint32_t> NewMemTracker);
    hawk() {}
    ~hawk() {}
  };
}
```
