#include "ast2/platform/BinderServiceProbe.hpp"

#include <sstream>
#include <string>

namespace ast2::platform {

BinderServiceProbe::BinderServiceProbe(BinderRuntime& runtime) noexcept
    : runtime_(runtime) {
}

Capability BinderServiceProbe::Probe(std::string_view serviceName) const {
    Capability capability;
    capability.name = std::string("binder.service.") + std::string(serviceName);

    AIBinder* binder = runtime_.CheckService(serviceName.data());
    if (binder == nullptr) {
        capability.state = CapabilityState::Unavailable;
        capability.detail = "service handle not available to current process";
        return capability;
    }

    const std::int32_t ping = runtime_.Ping(binder);
    const bool remote = runtime_.IsRemote(binder);
    runtime_.Release(binder);

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

    return capability;
}

} // namespace ast2::platform
