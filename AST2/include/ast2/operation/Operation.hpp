#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace ast2::operation {

enum class OperationState {
    Created,
    Validating,
    OpeningTransport,
    Executing,
    Verifying,
    Completed,
    Failed,
};

struct OperationContext {
    std::chrono::milliseconds timeout{30000};
};

struct OperationResult {
    OperationState state{OperationState::Created};
    bool success{};
    std::string detail;
};

class IOperation {
public:
    virtual ~IOperation() = default;

    [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
    [[nodiscard]] virtual OperationResult Run(const OperationContext& context) = 0;
};

} // namespace ast2::operation
