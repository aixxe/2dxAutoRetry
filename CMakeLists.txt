cmake_minimum_required(VERSION 3.26)
project(2dxAutoRetry VERSION 0.1.0.0 LANGUAGES CXX)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME              safetyhook
    GIT_TAG           e0ea3166464737557a416df2e77cda71b3763715
    GITHUB_REPOSITORY cursey/safetyhook
    OPTIONS           "SAFETYHOOK_FETCH_ZYDIS ON"
)

CPMAddPackage(
    NAME              spdlog
    GIT_TAG           v1.12.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS           "SPDLOG_BUILD_SHARED OFF"
)

add_library(${PROJECT_NAME} SHARED
    src/init.cc
    src/offsets.cc
    src/util/module.cc
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} safetyhook spdlog)