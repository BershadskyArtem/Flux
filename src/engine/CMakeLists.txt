# CMakeList.txt : CMake project for Flux, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package(JPEG REQUIRED)    
find_package(libraw CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)

set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

add_library(FLXCore SHARED 
    Color/Test.h Color/Test.cpp 
    "Exports.h" 
    "Exports.cpp" 
    "infrastructure/FLXDefines.h" "Color/Converter.h" "Color/Converter.cpp" "infrastructure/LUT.h" "infrastructure/LUT.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FLXCore PROPERTY CXX_STANDARD 20)
endif()



if ( MSVC )
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})

    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE}_INIT")
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
    set(CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})

    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

    target_compile_options(FLXCore INTERFACE /arch:AVX2)

    target_compile_options(FLXCore PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>)
endif ( MSVC )

if (CMAKE_VERSION VERSION_GREATER 3.12)

  target_compile_definitions(FLXCore PUBLIC NOMINMAX)

  set_property(TARGET FLXCore PROPERTY CXX_STANDARD 20)
endif()


target_link_libraries(FLXCore PRIVATE optimized;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/lib/turbojpeg.lib;debug;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/debug/lib/turbojpeg.lib)
target_include_directories(FLXCore PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(FLXCore PRIVATE libraw::raw_r)
target_link_libraries(FLXCore PRIVATE xsimd)



# TODO: Add tests and install targets if needed.
