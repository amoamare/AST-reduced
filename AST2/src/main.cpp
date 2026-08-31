#include "ast2/Capability.hpp"
#include "ast2/platform/BinderRuntime.hpp"
#include "ast2/samsung/SamsungBinderProbe.hpp"

#include <sys/system_properties.h>
#include <unistd.h>

#include <iostream>
#include <string>

namespace {

std::string GetProperty(const char* name) {
    char value[PROP_VALUE_MAX]{};
    const int length = __system_property_get(name, value);
    return length > 0 ? std::string(value, static_cast<std::size_t>(length))
                      : "<unset>";
}

} // namespace

int main() {
    std::cout
        << "AST2 capability probe\n"
        << "=====================\n"
        << "uid          : " << getuid() << '\n'
        << "manufacturer : " << GetProperty("ro.product.manufacturer") << '\n'
        << "model        : " << GetProperty("ro.product.model") << '\n'
        << "android      : " << GetProperty("ro.build.version.release") << '\n'
        << "sdk          : " << GetProperty("ro.build.version.sdk") << '\n'
        << "hardware     : " << GetProperty("ro.hardware") << "\n\n";

    ast2::platform::BinderRuntime binderRuntime;
    if (!binderRuntime.Load()) {
        std::cerr << "Binder runtime unavailable: "
                  << binderRuntime.LastError() << '\n';
        return 2;
    }

    ast2::samsung::SamsungBinderProbe samsungProbe(binderRuntime);

    for (const auto& capability : samsungProbe.Probe()) {
        std::cout << capability.name << '\n'
                  << "  state  : " << ast2::ToString(capability.state) << '\n'
                  << "  detail : " << capability.detail << "\n\n";
    }

    std::cout << "No modem/vendor operation transactions were issued.\n";
    return 0;
}
