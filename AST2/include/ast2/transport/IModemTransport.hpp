#pragma once

#include "ast2/Capability.hpp"

#include <string_view>
#include <vector>

namespace ast2::transport {

class IModemTransport {
public:
    virtual ~IModemTransport() = default;

    [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
    [[nodiscard]] virtual std::vector<Capability> ProbeCapabilities() const = 0;
};

} // namespace ast2::transport
