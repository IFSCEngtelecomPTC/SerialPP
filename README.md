# How to use this library using cmake

Your cmake project need to fetch this library, and then use it when compiling your programs:

```
include(FetchContent)
FetchContent_Declare(
        serialpp
        URL https://github.com/IFSCEngtelecomPTC/SerialPP/archive/refs/tags/v1.0.0.tar.gz
)
FetchContent_MakeAvailable(serialpp)
include_directories(${serialpp_SOURCE_DIR} .)

# OBS: this is just a demo to show you how to link this library to your executable

# The following line is usually generated by CLion (or by yourself, if using cmake manually)
# Replace it accordingly to your project
add_executable(demo main.cpp)

# This command links socketpp library to your executable
# Rename test_app to yout executable name
target_link_libraries(demo SerialPP)
```