#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace ast2::samsung {

struct SamsungTransactionProfile {
    std::string profileName;
    std::string serviceName;
    std::string descriptor;
    std::optional<std::uint32_t> sendRequestToRil;
    std::optional<std::uint32_t> invokeOemRilRequestRawForSubscriber;

    [[nodiscard]] static SamsungTransactionProfile N975UAndroid12();
};

} // namespace ast2::samsung
