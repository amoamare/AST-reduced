#pragma once

#include "ast2/Capability.hpp"
#include "ast2/platform/BinderRuntime.hpp"

#include <string_view>

namespace ast2::platform {

class BinderServiceProbe final {
public:
    explicit BinderServiceProbe(BinderRuntime& runtime) noexcept;

    [[nodiscard]] Capability Probe(std::string_view serviceName) const;

private:
    BinderRuntime& runtime_;
};

} // namespace ast2::platform
