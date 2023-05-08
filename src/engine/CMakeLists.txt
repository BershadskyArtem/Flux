# CMakeList.txt : CMake project for Flux, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package(JPEG REQUIRED)    
find_package(libraw CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)
find_package(OpenMP REQUIRED)




set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

add_library(FLXCore SHARED 
    Color/Test.h Color/Test.cpp 
    "Exports.h" 
    "Exports.cpp" 
    "Color/Converter.h" 
    "Color/Converter.cpp" 
    "infrastructure/FLXDefines.h" 
    "infrastructure/LUT.h" 
    "infrastructure/LUT.cpp" 
    "infrastructure/LUTf3d.h" 
    "infrastructure/LUTf3d.cpp" 
    "infrastructure/InteropTypes.h" 
    "infrastructure/MemoryUtils.cpp" 
    "infrastructure/MemoryUtils.h" 
    "io/ImageDecoder.h" 
    "io/ImageDecoder.cpp" 
    "io/JpegImageDecoder.h" 
    "io/JpegImageDecoder.cpp" 
    "io/ImageInput.h" 
    "io/ImageInput.cpp" 
    "infrastructure/PixelsHelper.h" 
    "infrastructure/PixelsHelper.cpp"
    "io/Encoders/ImageEncoder.h" 
    "io/Encoders/ImageEncoder.cpp" 
    "io/Encoders/JpegImageEncoder.h" 
    "io/Encoders/JpegImageEncoder.cpp" 
    "infrastructure/BenchmarkHelper.h" 
    "infrastructure/BenchmarkHelper.cpp" 
    "Pipeline/FluxPipeline.h" 
    "io/RawImageDecoder.h" 
    "io/RawImageDecoder.cpp"
    "io/BitmapStructureDecoder.h" 
    "infrastructure/BitmapHelper.h" 
    "infrastructure/BitmapHelper.cpp" 
    "Color/ColorLUTConverter.h" 
    "Color/ColorLUTConverter.cpp" 
    "Core/FluxImageProcessor.h" 
    "Core/FluxImageProcessor.cpp" 
    "Core/FluxWaveletDenoising.h" 
    "Core/FluxWaveletDenoising.cpp" 
    "Core/WaveletImage.h" 
    "Core/WaveletImage.cpp" 
    "Core/Wavelets/Daubechies4.h" 
    "Core/Wavelets/Daubechies4.cpp"
    "infrastructure/Matrix.h"
    "infrastructure/Matrix.cpp"
 "Core/imageOperations/BaseImageOperation.h" "Core/ImageOperations/BaseImageOperation.cpp")

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

target_include_directories(FLXCore PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(FLXCore PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(FLXCore PRIVATE optimized;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/lib/turbojpeg.lib;debug;${CMAKE_SOURCE_DIR}/out/build/x64-Debug/vcpkg_installed/x64-windows-static/debug/lib/turbojpeg.lib)
target_link_libraries(FLXCore PRIVATE libraw::raw_r)
target_link_libraries(FLXCore PRIVATE xsimd)




# TODO: Add tests and install targets if needed.
