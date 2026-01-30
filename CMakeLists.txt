cmake_minimum_required(VERSION 3.16)
project(iot_builder LANGUAGES C CXX)

option(BUILD_HOST_TOOLS "Build host-only tools" ON)

if(NOT BUILD_HOST_TOOLS)
  message(STATUS "iot_builder: host build disabled")
  return()
endif()

# Skip building when cross-compiling for Zephyr
if(CMAKE_CROSSCOMPILING)
  message(STATUS "iot_builder: skipping host build during cross-compilation")
  return()
endif()

# Source files (relative to this directory)
set(IOT_BUILDER_SRCS
  src/builder/main.cpp
  src/builder/builder.cpp
  src/builder/CSpline.cpp
  src/builder/Interpolator.cpp
  lib/json/JsonValue.cpp
  lib/json/JsonObject.cpp
  lib/json/JsonArray.cpp
  lib/json/JsonFactory.cpp
)

add_executable(iot_builder ${IOT_BUILDER_SRCS})

target_include_directories(iot_builder PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/builder
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

target_compile_features(iot_builder PRIVATE cxx_std_17)
if(NOT MSVC)
  target_compile_options(iot_builder PRIVATE -Wall -Wextra -Wpedantic)
endif()

# The platform adapter header now defines/exports fixed-width integer types
# for the build; avoid forcing includes or compile definitions here to keep the
# build flags minimal and portable.
# Provide an alias target for easier linkage/importing
# Do not create a library alias for an executable target

# Generated output directory
set(IOT_BUILDER_OUT_DIR ${CMAKE_BINARY_DIR}/generated/)
file(MAKE_DIRECTORY ${IOT_BUILDER_OUT_DIR})
set(IOT_GENERATED ${IOT_BUILDER_OUT_DIR}/config.c ${IOT_BUILDER_OUT_DIR}/config.h)

# Run the built tool to produce generated files
add_custom_command(
  OUTPUT ${IOT_GENERATED}
  COMMAND $<TARGET_FILE:iot_builder> ${CMAKE_CURRENT_SOURCE_DIR}/src/builder/sample_config.json ${IOT_BUILDER_OUT_DIR}
  DEPENDS iot_builder
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running iot_builder to produce generated files"
  VERBATIM
)

add_custom_target(iot_builder_gen DEPENDS ${IOT_GENERATED})
add_custom_target(build_and_run DEPENDS iot_builder iot_builder_gen)

install(TARGETS iot_builder RUNTIME DESTINATION bin)
