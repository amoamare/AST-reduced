#include "ast2/samsung/SamsungBinderProbe.hpp"

#include <array>
#include <sstream>
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

    std::vector<Capability> capabilities;
    capabilities.reserve(services.size());

    for (const auto service : services) {
        Capability capability;
        capability.name = std::string("binder.service.") + std::string(service);

        AIBinder* binder = binderRuntime_.CheckService(service.data());
        if (binder == nullptr) {
            capability.state = CapabilityState::Unavailable;
            capability.detail = "service handle not available to current process";
            capabilities.push_back(std::move(capability));
            continue;
        }

        const std::int32_t ping = binderRuntime_.Ping(binder);
        const bool remote = binderRuntime_.IsRemote(binder);
        binderRuntime_.Release(binder);

        if (ping == 0) {
            capability.state = CapabilityState::Available;
            std::ostringstream detail;
            detail << "handle=yes, remote=" << (remote ? "yes" : "no")
                   << ", ping=0";
            capability.detail = detail.str();
        } else {
            capability.state = CapabilityState::Denied;
            capability.detail = "service handle acquired but Binder ping failed: " +
                                std::to_string(ping);
        }

        capabilities.push_back(std::move(capability));
    }

    return capabilities;
}

} // namespace ast2::samsung
