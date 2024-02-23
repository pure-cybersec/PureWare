CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project ("cripter")

set(CMAKE_CXX_STANDARD 23)

add_executable (cripter "PureWare.cpp" "PureWare.hpp")

target_link_options(cripter PRIVATE
    "/level='requireAdministrator'"
)
