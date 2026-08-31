#include "ast2/samsung/SamsungBinderProbe.hpp"
#include "ast2/platform/BinderServiceProbe.hpp"

#include <array>
#include <string_view>

namespace ast2::samsung {

SamsungBinderProbe::SamsungBinderProbe(
    platform::BinderRuntime& binderRuntime) noexcept
    : binderRuntime_(binderRuntime) {
}

std::vector<Capability> SamsungBinderProbe::Probe() const {
    constexpr std::array<std::string_view, 2> services{
        "phone",
        "isemtelephony",
    };

    platform::BinderServiceProbe probe(binderRuntime_);

    std::vector<Capability> capabilities;
    capabilities.reserve(services.size());

    for (const auto service : services) {
        capabilities.push_back(probe.Probe(service));
    }

    return capabilities;
}

} // namespace ast2::samsung
