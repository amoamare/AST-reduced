#pragma once

#include <string>

namespace ast2 {

struct DeviceProfile {
    std::string manufacturer;
    std::string model;
    std::string device;
    std::string androidRelease;
    std::string sdk;
    std::string securityPatch;
    std::string fingerprint;
    std::string hardware;
    std::string boardPlatform;
    unsigned int uid{};

    [[nodiscard]] static DeviceProfile Detect();
};

} // namespace ast2
