#pragma once

#include <stdio.h>

namespace optriment {
class Serializable {
   public:
    virtual char* toBytes() const = 0;
    virtual void fromBytes(char* ptr, size_t len) = 0;
};
}  // namespace optriment