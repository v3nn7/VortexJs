cmake_minimum_required(VERSION 3.17)
project(vortexjs)

# ✅ Wymuszenie C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ✅ Ścieżki do zależności
set(GODOT_CPP_PATH "${CMAKE_SOURCE_DIR}/../godot-cpp")
set(QUICKJS_PATH "${CMAKE_SOURCE_DIR}/quickjs")

# ✅ Pliki C++ Twojego rozszerzenia
file(GLOB_RECURSE VORTEXJS_SOURCES
    ${CMAKE_SOURCE_DIR}/*.cpp
    ${CMAKE_SOURCE_DIR}/*.h
)

# ✅ Tworzenie biblioteki GDExtension
add_library(vortexjs SHARED ${VORTEXJS_SOURCES})
set_target_properties(vortexjs PROPERTIES OUTPUT_NAME "vortexjs")

# ✅ Ścieżki do nagłówków
target_include_directories(vortexjs PRIVATE
    ${QUICKJS_PATH}/libquickjs       # quickjs.h i inne nagłówki
    ${GODOT_CPP_PATH}/include
    ${GODOT_CPP_PATH}/gen/include
)

# ✅ Linkowanie z godot-cpp i gotową libquickjs.lib
add_subdirectory(${GODOT_CPP_PATH} godot-cpp-build)

target_link_libraries(vortexjs
    godot-cpp
    ${QUICKJS_PATH}/x64/Release/libquickjs.lib
)

# ✅ MSVC: dodatkowe opcje
if (MSVC)
    target_compile_options(vortexjs PRIVATE /std:c++20 /MP)
    target_compile_definitions(vortexjs PRIVATE CONFIG_VERSION=1)
endif()

# ✅ Kopiowanie DLL do addons/vortexjs/bin/
add_custom_command(TARGET vortexjs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/../addons/vortexjs/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:vortexjs>"
        "${CMAKE_SOURCE_DIR}/../addons/vortexjs/bin/$<TARGET_FILE_NAME:vortexjs>"
)
