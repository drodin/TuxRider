cmake_minimum_required(VERSION 3.9.2)

# For Android Studio we will force generated toolchain
if(ANDROID_STUDIO)
  set(gen_toolchain "${CMAKE_CURRENT_BINARY_DIR}/generated/toolchain.cmake")
  configure_file(
      "${CMAKE_CURRENT_LIST_DIR}/cmake/template/toolchain.cmake.in"
      "${gen_toolchain}"
      @ONLY
  )
  set(CMAKE_TOOLCHAIN_FILE "${gen_toolchain}" CACHE PATH "" FORCE)
endif()

# Example of stepping into external sources.
# USE CAREFULLY, please check documentation:
# * https://docs.hunter.sh/en/latest/reference/user-variables.html#hunter-keep-package-sources
option(HUNTER_KEEP_PACKAGE_SOURCES "Keep third party sources" ON)

option(HUNTER_STATUS_DEBUG "Print debug info" ON)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.12.tar.gz"
    SHA1 "f6a2b596beb8bbd7d81d01a0fcf417f97d779fbf"
)

project(tuxrider)

option(CMAKE_VERBOSE_MAKEFILE "Verbose makefile" ON)

file(GLOB SOURCES
        src/main/cpp/android/*.c
        src/main/cpp/src/*.c
        src/main/cpp/src/*.cpp
)

set(C_FLAGS "-D__APPLE__ -DTUXRACER_NO_ASSERT -DLIB_CLASS=\\\"com/drodin/tuxrider/NativeLib\\\" --include android.h")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")

add_library(tuxrider SHARED ${SOURCES})

# If CMAKE_DEBUG_POSTFIX is emtpy Hunter will initialize with 'd'.
# We have to explicitly set DEBUG_POSTFIX to simplify library
# load from Java code.
set_target_properties(tuxrider PROPERTIES DEBUG_POSTFIX "")

target_include_directories(tuxrider PRIVATE
        src/main/cpp/android/
        src/main/cpp/src/
)

hunter_add_package(tcl)
find_package(tcl REQUIRED)

target_link_libraries(tuxrider log dl c m GLESv1_CM tcl::tcl)
