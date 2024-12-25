#include <cstring>
#include <sstream>
#include <string>

#include "RegionCountMessage.h"

using namespace optriment;

char* RegionCountMessage::toBytes() const
{
    std::stringstream ss;
    ss << "{" << std::endl;
    for (const auto& zoneCount : zoneCounts)
    {
        ss << "\"" << zoneCount.first << "_count: " << "\": " << zoneCount.second << "," << std::endl;
    }
    ss << "}" << std::endl;
    std::string str = ss.str();
    char* result = new char[str.size() + 1];
    std::strcpy(result, str.c_str());
    return result;
}

void RegionCountMessage::fromBytes(char* ptr, size_t len)
{
    std::string str(ptr, len);
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        std::string zone;
        uint32_t count;
        std::stringstream tokenStream(token);
        std::getline(tokenStream, zone, ':');
        tokenStream >> count;
        zoneCounts[zone] = count;
    }
}

void RegionCountMessage::add(const std::string& zone, uint32_t count)
{
    zoneCounts[zone] = count;
}