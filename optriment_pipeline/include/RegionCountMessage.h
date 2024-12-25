#pragma once

#include <unordered_map>

#include "Serializable.h"

namespace optriment {
class RegionCountMessage : public Serializable
{
public:
    char* toBytes() const override;
    void fromBytes(char* ptr, size_t len) override;
    void add(const std::string& zone, uint32_t count);

private:
    std::unordered_map<std::string, uint32_t> zoneCounts;
};
} // namespace optriment