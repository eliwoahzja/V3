#pragma once

bool (*orig_bypass)(void *ins);
bool hook_bypass(void *ins) {
    return false;
}

#if defined(__aarch64__)

inline const char *armFalse = "00 00 80 D2 C0 03 5F D6";

#endif

struct range {
    uintptr_t Irt, Ind;
    struct Iter {
      uintptr_t val;
      bool operator!=(const Iter& it) const {
        return val <= it.val; 
      }
      uintptr_t operator*() const {
        return val; 
      }
      void operator++() {
        val += 4; 
      }
    };
    Iter begin() const { 
      return {Irt}; 
    }
    Iter end() const {
      return {Ind}; 
    }
};

inline void InitializeProtection() {
MemoryPatch::createWithHex("libanogs.so", 0x204218, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x2D2A70, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x30B87C, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x438154, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x44A714, "00 00 80 D2 C0 03 5F D6").Modify();
    for (auto offs : range{0x1, 0x1000}) {
        MemoryPatch::createWithHex("libanogs.so", offs, armFalse).Modify();
    }
    
}
    
    
