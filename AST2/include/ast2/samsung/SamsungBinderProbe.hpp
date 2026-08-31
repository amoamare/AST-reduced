#pragma once

#include "ast2/Capability.hpp"
#include "ast2/platform/BinderRuntime.hpp"

#include <vector>

namespace ast2::samsung {

class SamsungBinderProbe final {
public:
    explicit SamsungBinderProbe(platform::BinderRuntime& binderRuntime) noexcept;

    [[nodiscard]] std::vector<Capability> Probe() const;

private:
    platform::BinderRuntime& binderRuntime_;
};

} // namespace ast2::samsung
