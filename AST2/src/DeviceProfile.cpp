#include "ast2/DeviceProfile.hpp"

#include <sys/system_properties.h>
#include <unistd.h>

#include <string>

namespace ast2 {

namespace {

std::string GetProperty(const char* name) {
    char value[PROP_VALUE_MAX]{};
    const int length = __system_property_get(name, value);
    return length > 0 ? std::string(value, static_cast<std::size_t>(length))
                      : std::string{};
}

} // namespace

DeviceProfile DeviceProfile::Detect() {
    DeviceProfile profile;
    profile.manufacturer = GetProperty("ro.product.manufacturer");
    profile.model = GetProperty("ro.product.model");
    profile.device = GetProperty("ro.product.device");
    profile.androidRelease = GetProperty("ro.build.version.release");
    profile.sdk = GetProperty("ro.build.version.sdk");
    profile.securityPatch = GetProperty("ro.build.version.security_patch");
    profile.fingerprint = GetProperty("ro.build.fingerprint");
    profile.hardware = GetProperty("ro.hardware");
    profile.boardPlatform = GetProperty("ro.board.platform");
    profile.uid = static_cast<unsigned int>(getuid());
    return profile;
}

} // namespace ast2
