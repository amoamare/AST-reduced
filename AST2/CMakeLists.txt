cmake_minimum_required(VERSION 3.22.1)
project(AST2 LANGUAGES CXX)

add_executable(ast2-probe
    src/main.cpp
    src/DeviceProfile.cpp
    src/platform/BinderRuntime.cpp
    src/platform/BinderServiceProbe.cpp
    src/samsung/SamsungBinderProbe.cpp
    src/samsung/SamsungTransactionProfile.cpp
)

target_include_directories(ast2-probe PRIVATE include)
target_compile_features(ast2-probe PRIVATE cxx_std_20)
target_compile_options(ast2-probe PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)
target_link_libraries(ast2-probe PRIVATE dl)
