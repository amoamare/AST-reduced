#pragma once

#include <android/binder_ibinder.h>

#include <cstdint>
#include <string>

namespace ast2::platform {

class BinderRuntime final {
public:
    BinderRuntime() = default;
    BinderRuntime(const BinderRuntime&) = delete;
    BinderRuntime& operator=(const BinderRuntime&) = delete;
    BinderRuntime(BinderRuntime&&) = delete;
    BinderRuntime& operator=(BinderRuntime&&) = delete;
    ~BinderRuntime();

    bool Load();
    [[nodiscard]] bool IsLoaded() const noexcept;

    [[nodiscard]] AIBinder* CheckService(const char* name) const noexcept;
    [[nodiscard]] bool IsDeclared(const char* name) const noexcept;
    [[nodiscard]] std::int32_t Ping(AIBinder* binder) const noexcept;
    [[nodiscard]] bool IsRemote(const AIBinder* binder) const noexcept;
    void Release(AIBinder* binder) const noexcept;

    [[nodiscard]] std::string LastError() const;

private:
    using CheckServiceFn = AIBinder* (*)(const char*);
    using IsDeclaredFn = bool (*)(const char*);
    using PingFn = std::int32_t (*)(AIBinder*);
    using IsRemoteFn = bool (*)(const AIBinder*);
    using DecStrongFn = void (*)(AIBinder*);

    void* handle_{};
    CheckServiceFn checkService_{};
    IsDeclaredFn isDeclared_{};
    PingFn ping_{};
    IsRemoteFn isRemote_{};
    DecStrongFn decStrong_{};
    std::string lastError_;
};

} // namespace ast2::platform
