
find_package(JPEG REQUIRED)    
find_package(libraw CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)
find_package(OpenMP REQUIRED)



add_executable (FluxTest 
    "Flux.cpp" 
    "Flux.h" 
    "../engine/Color/Converter.cpp" 
    "../engine/Color/Converter.h"  
    "../engine/infrastructure/FLXDefines.h" 
    "../engine/infrastructure/LUT.h" 
    "../engine/infrastructure/LUT.cpp" 
    "../engine/infrastructure/LUTf3d.h" 
    "../engine/infrastructure/LUTf3d.cpp" 
    "../engine/infrastructure/InteropTypes.h" 
    "../engine/infrastructure/MemoryUtils.cpp" 
    "../engine/infrastructure/MemoryUtils.h" 
    "../engine/infrastructure/BenchmarkHelper.h" 
    "../engine/infrastructure/BitmapHelper.h" 
    "../engine/infrastructure/BitmapHelper.cpp" 
    "../engine/io/ImageDecoder.h" 
    "../engine/io/ImageDecoder.cpp" 
    "../engine/io/JpegImageDecoder.h" 
    "../engine/io/JpegImageDecoder.cpp" 
    "../engine/io/ImageInput.h" 
    "../engine/io/ImageInput.cpp"
    "../engine/infrastructure/PixelsHelper.h" 
    "../engine/infrastructure/PixelsHelper.cpp"
    "../engine/io/Encoders/ImageEncoder.cpp" 
    "../engine/io/Encoders/ImageEncoder.h" 
    "../engine/io/Encoders/JpegImageEncoder.cpp" 
    "../engine/io/Encoders/JpegImageEncoder.h" 
    "../engine/io/RawImageDecoder.h" 
    "../engine/io/RawImageDecoder.cpp" 
    "../engine/Color/ColorLUTConverter.h" 
    "../engine/Color/ColorLUTConverter.cpp" 
    "../engine/Core/FluxImageProcessor.cpp" 
    "../engine/Core/FluxImageProcessor.h" 
)

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

     target_compile_options(FluxTest INTERFACE /arch:AVX2)
     target_compile_options(FluxTest PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>)
endif ( MSVC )

if (CMAKE_VERSION VERSION_GREATER 3.12)

  target_compile_definitions(FluxTest PUBLIC NOMINMAX)

  set_property(TARGET FluxTest PROPERTY CXX_STANDARD 20)
endif()
#set_property(TARGET FluxTest PROPERTY CXX_STANDARD 14)




target_link_libraries(FluxTest PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(FluxTest PRIVATE optimized;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/lib/turbojpeg.lib;debug;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/debug/lib/turbojpeg.lib)
target_include_directories(FluxTest PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(FluxTest PRIVATE libraw::raw_r)
target_link_libraries(FluxTest PRIVATE xsimd)




