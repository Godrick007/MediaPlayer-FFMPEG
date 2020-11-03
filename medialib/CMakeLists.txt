# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


# cmakelists 设置c++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")

endif()
# cmakelists 设置c++11

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories("src/main/cpp/include")

add_library( # Sets the name of the library.
        MediaPlayer-FFMPEG

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/MediaEnter.cpp
        src/main/cpp/util/LogUtil.cpp
        src/main/cpp/Callback2Java.cpp
        src/main/cpp/decode/MediaPlayer.cpp
        src/main/cpp/decode/PlayState.cpp
        src/main/cpp/decode/Queue.cpp
        src/main/cpp/decode/Audio.cpp
        src/main/cpp/decode/Video.cpp

        )


add_library(
        soundtouch

        SHARED

        src/main/cpp/soundtouch/AAFilter.cpp
        src/main/cpp/soundtouch/BPMDetect.cpp
        src/main/cpp/soundtouch/cpu_detect_x86.cpp
        src/main/cpp/soundtouch/FIFOSampleBuffer.cpp
        src/main/cpp/soundtouch/FIRFilter.cpp
        src/main/cpp/soundtouch/InterpolateCubic.cpp
        src/main/cpp/soundtouch/InterpolateLinear.cpp
        src/main/cpp/soundtouch/InterpolateShannon.cpp
        src/main/cpp/soundtouch/mmx_optimized.cpp
        src/main/cpp/soundtouch/PeakFinder.cpp
        src/main/cpp/soundtouch/RateTransposer.cpp
        src/main/cpp/soundtouch/SoundTouch.cpp
        src/main/cpp/soundtouch/sse_optimized.cpp
        src/main/cpp/soundtouch/TDStretch.cpp

)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

set(SO_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(avcodec-58 SHARED IMPORTED)
set_target_properties(avcodec-58
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libavcodec-58.so)


add_library(avdevice-58 SHARED IMPORTED)
set_target_properties(avdevice-58
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libavdevice-58.so)


add_library(avfilter-7 SHARED IMPORTED)
set_target_properties(avfilter-7
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libavfilter-7.so)


add_library(avformat-58 SHARED IMPORTED)
set_target_properties(avformat-58
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libavformat-58.so)


add_library(avutil-56 SHARED IMPORTED)
set_target_properties(avutil-56
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libavutil-56.so)


add_library(postproc-55 SHARED IMPORTED)
set_target_properties(postproc-55
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libpostproc-55.so)


add_library(swresample-3 SHARED IMPORTED)
set_target_properties(swresample-3
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libswresample-3.so)


add_library(swscale-5 SHARED IMPORTED)
set_target_properties(swscale-5
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libswscale-5.so)


add_library(x264 SHARED IMPORTED)
set_target_properties(x264
        PROPERTIES IMPORTED_LOCATION
        ${SO_DIR}/${ANDROID_ABI}/libx264.so)


target_link_libraries( # Specifies the target library.
        MediaPlayer-FFMPEG
        soundtouch

        OpenSLES
        avcodec-58
        avdevice-58
        avfilter-7
        avformat-58
        avutil-56
        postproc-55
        swresample-3
        swscale-5
        x264

        android
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )