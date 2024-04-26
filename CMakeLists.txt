CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project ("cripter")

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/exe")

add_executable (cripter "PureWare.cpp" "PureWare.hpp" "more.hpp" "Socket.cpp" "SSLConnection.cpp")

find_package(spdlog CONFIG REQUIRED)
    target_link_libraries(cripter PRIVATE spdlog::spdlog_header_only)

find_package(fmt CONFIG REQUIRED)
    target_link_libraries(cripter PRIVATE fmt::fmt-header-only)


target_link_options(cripter PRIVATE
    "/level='requireAdministrator'"
)
