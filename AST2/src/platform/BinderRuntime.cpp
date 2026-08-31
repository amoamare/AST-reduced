#include "ast2/platform/BinderRuntime.hpp"

#include <dlfcn.h>

namespace ast2::platform {

namespace {

template <typename T>
T LoadSymbol(void* handle, const char* name) noexcept {
    return reinterpret_cast<T>(dlsym(handle, name));
}

} // namespace

BinderRuntime::~BinderRuntime() {
    if (handle_ != nullptr) {
        dlclose(handle_);
    }
}

bool BinderRuntime::Load() {
    if (handle_ != nullptr) {
        return true;
    }

    handle_ = dlopen("libbinder_ndk.so", RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr) {
        const char* error = dlerror();
        lastError_ = error != nullptr ? error : "dlopen(libbinder_ndk.so) failed";
        return false;
    }

    checkService_ = LoadSymbol<CheckServiceFn>(handle_, "AServiceManager_checkService");
    isDeclared_ = LoadSymbol<IsDeclaredFn>(handle_, "AServiceManager_isDeclared");
    ping_ = LoadSymbol<PingFn>(handle_, "AIBinder_ping");
    isRemote_ = LoadSymbol<IsRemoteFn>(handle_, "AIBinder_isRemote");
    decStrong_ = LoadSymbol<DecStrongFn>(handle_, "AIBinder_decStrong");

    if (checkService_ == nullptr || ping_ == nullptr ||
        isRemote_ == nullptr || decStrong_ == nullptr) {
        lastError_ = "libbinder_ndk is missing one or more required runtime symbols";
        dlclose(handle_);
        handle_ = nullptr;
        return false;
    }

    lastError_.clear();
    return true;
}

bool BinderRuntime::IsLoaded() const noexcept {
    return handle_ != nullptr;
}

AIBinder* BinderRuntime::CheckService(const char* name) const noexcept {
    return checkService_ != nullptr ? checkService_(name) : nullptr;
}

bool BinderRuntime::IsDeclared(const char* name) const noexcept {
    return isDeclared_ != nullptr && isDeclared_(name);
}

std::int32_t BinderRuntime::Ping(AIBinder* binder) const noexcept {
    return ping_ != nullptr && binder != nullptr ? ping_(binder) : -1;
}

bool BinderRuntime::IsRemote(const AIBinder* binder) const noexcept {
    return isRemote_ != nullptr && binder != nullptr && isRemote_(binder);
}

void BinderRuntime::Release(AIBinder* binder) const noexcept {
    if (decStrong_ != nullptr && binder != nullptr) {
        decStrong_(binder);
    }
}

std::string BinderRuntime::LastError() const {
    return lastError_;
}

} // namespace ast2::platform
