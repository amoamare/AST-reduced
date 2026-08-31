#pragma once

#include <string>

namespace ast2 {

enum class CapabilityState {
    Unknown,
    Available,
    Unavailable,
    Denied,
};

struct Capability {
    std::string name;
    CapabilityState state{CapabilityState::Unknown};
    std::string detail;
};

inline const char* ToString(CapabilityState state) noexcept {
    switch (state) {
        case CapabilityState::Unknown: return "unknown";
        case CapabilityState::Available: return "available";
        case CapabilityState::Unavailable: return "unavailable";
        case CapabilityState::Denied: return "denied";
    }
    return "unknown";
}

} // namespace ast2
