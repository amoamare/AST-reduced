#include "ast2/Capability.hpp"
#include "ast2/DeviceProfile.hpp"
#include "ast2/platform/BinderRuntime.hpp"
#include "ast2/samsung/SamsungBinderProbe.hpp"
#include "ast2/samsung/SamsungTransactionProfile.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::string EscapeJson(std::string_view value) {
    std::string output;
    output.reserve(value.size() + 8);

    for (const char ch : value) {
        switch (ch) {
            case '\\': output += "\\\\"; break;
            case '"': output += "\\\""; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += ch; break;
        }
    }

    return output;
}

void PrintText(
    const ast2::DeviceProfile& device,
    const std::vector<ast2::Capability>& capabilities) {

    std::cout
        << "AST2 capability probe\n"
        << "=====================\n"
        << "uid            : " << device.uid << '\n'
        << "manufacturer   : " << device.manufacturer << '\n'
        << "model          : " << device.model << '\n'
        << "device         : " << device.device << '\n'
        << "android        : " << device.androidRelease << '\n'
        << "sdk            : " << device.sdk << '\n'
        << "security patch : " << device.securityPatch << '\n'
        << "hardware       : " << device.hardware << '\n'
        << "board platform : " << device.boardPlatform << "\n\n";

    for (const auto& capability : capabilities) {
        std::cout << capability.name << '\n'
                  << "  state  : " << ast2::ToString(capability.state) << '\n'
                  << "  detail : " << capability.detail << "\n\n";
    }

    if (device.model == "SM-N975U" && device.androidRelease == "12") {
        const auto profile =
            ast2::samsung::SamsungTransactionProfile::N975UAndroid12();

        std::cout
            << "matched firmware profile\n"
            << "  name       : " << profile.profileName << '\n'
            << "  service    : " << profile.serviceName << '\n'
            << "  descriptor : " << profile.descriptor << '\n';

        if (profile.sendRequestToRil.has_value()) {
            std::cout << "  sendRequestToRIL transaction : "
                      << *profile.sendRequestToRil << '\n';
        }

        if (profile.invokeOemRilRequestRawForSubscriber.has_value()) {
            std::cout << "  invokeOemRilRequestRawForSubscriber transaction : "
                      << *profile.invokeOemRilRequestRawForSubscriber << '\n';
        }

        std::cout << '\n';
    }

    std::cout << "No modem/vendor operation transactions were issued.\n";
}

void PrintJson(
    const ast2::DeviceProfile& device,
    const std::vector<ast2::Capability>& capabilities) {

    std::cout
        << "{\n"
        << "  \"device\": {\n"
        << "    \"uid\": " << device.uid << ",\n"
        << "    \"manufacturer\": \"" << EscapeJson(device.manufacturer) << "\",\n"
        << "    \"model\": \"" << EscapeJson(device.model) << "\",\n"
        << "    \"device\": \"" << EscapeJson(device.device) << "\",\n"
        << "    \"androidRelease\": \"" << EscapeJson(device.androidRelease) << "\",\n"
        << "    \"sdk\": \"" << EscapeJson(device.sdk) << "\",\n"
        << "    \"securityPatch\": \"" << EscapeJson(device.securityPatch) << "\",\n"
        << "    \"fingerprint\": \"" << EscapeJson(device.fingerprint) << "\",\n"
        << "    \"hardware\": \"" << EscapeJson(device.hardware) << "\",\n"
        << "    \"boardPlatform\": \"" << EscapeJson(device.boardPlatform) << "\"\n"
        << "  },\n"
        << "  \"capabilities\": [\n";

    for (std::size_t index = 0; index < capabilities.size(); ++index) {
        const auto& capability = capabilities[index];
        std::cout
            << "    {\"name\": \"" << EscapeJson(capability.name)
            << "\", \"state\": \"" << ast2::ToString(capability.state)
            << "\", \"detail\": \"" << EscapeJson(capability.detail) << "\"}";

        if (index + 1 < capabilities.size()) {
            std::cout << ',';
        }
        std::cout << '\n';
    }

    std::cout << "  ]\n}\n";
}

} // namespace

int main(int argc, char** argv) {
    const bool json =
        argc == 2 && std::string_view(argv[1]) == "--json";

    if (argc > 2 || (argc == 2 && !json)) {
        std::cerr << "Usage: ast2-probe [--json]\n";
        return 1;
    }

    const ast2::DeviceProfile device = ast2::DeviceProfile::Detect();

    ast2::platform::BinderRuntime binderRuntime;
    if (!binderRuntime.Load()) {
        std::cerr << "Binder runtime unavailable: "
                  << binderRuntime.LastError() << '\n';
        return 2;
    }

    ast2::samsung::SamsungBinderProbe samsungProbe(binderRuntime);
    const auto capabilities = samsungProbe.Probe();

    if (json) {
        PrintJson(device, capabilities);
    } else {
        PrintText(device, capabilities);
    }

    return 0;
}
