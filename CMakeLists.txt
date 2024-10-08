cmake_minimum_required(VERSION 3.20)


# ------------------------- #
# - PROJECT SETTINGS
# ------------------------- #

project(Bretema VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ------------------------- #
# - OPTIONS
# ------------------------- #

option(OPT_TESTS "Compile tests instead of main app" OFF)


# ------------------------- #
# - FUNCTIONS
# ------------------------- #

function(bmCopyDir src dest)
    message(STATUS "Copying directory ${src} to ${dest}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${src} ${dest})
endfunction()

function(bmCopyToBuild pathName)
    bmCopyDir(${CMAKE_SOURCE_DIR}/${pathName} ${CMAKE_BINARY_DIR}/${pathName})
endfunction()


# ------------------------- #
# - THIRD PARTY
# ------------------------- #

# Modern Printf
set(FMT_DOC OFF)
add_subdirectory(Modules/fmt)
list(APPEND BM_LIBS fmt)

# Glsl-Style Maths
add_subdirectory(Modules/glm)
list(APPEND BM_LIBS glm)

# Cross-Platform Window Management
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC  ON  CACHE BOOL "" FORCE)
add_subdirectory(Modules/glfw)
list(APPEND BM_LIBS glfw)

# Vulkan
find_package(Vulkan REQUIRED )
find_program(glslcExe NAMES glslc HINTS Vulkan::glslc)
list(APPEND BM_INCLUDES ${GLFW_DIR}/deps)
list(APPEND BM_LIBS Vulkan::Vulkan)

# Vulkan Bootstrap
set(VK_BOOTSTRAP_DISABLE_WARNINGS ON CACHE BOOL "" FORCE)
add_subdirectory(Modules/vk-bootstrap)
list(APPEND BM_LIBS vk-bootstrap)

# ImGui: Platform agnostic GUI
set(IMGUI_DIR Modules/ImGui)
list(APPEND BM_INCLUDES ${IMGUI_DIR})
list(APPEND BM_SOURCES_EXTRA
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)
list(APPEND BM_HEADERS_EXTRA
    ${IMGUI_DIR}/imgui_impl_glfw.h
    ${IMGUI_DIR}/imgui_impl_vulkan.h
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
)


# ------------------------- #
# - SHADERS COMPILE AND COPY (@review: improve vars naming)
# ------------------------- #

set(BM_SHADERS_DIR "Assets/Shaders")

file(GLOB BM_SHADERS_LIST ${BM_SHADERS_DIR}/*)

add_custom_command(COMMAND
    ${CMAKE_COMMAND} -E remove_directory ${BM_SHADERS_DIR}
    ${CMAKE_COMMAND} -E make_directory   ${BM_SHADERS_DIR}
    OUTPUT ${BM_SHADERS_DIR}
)

MESSAGE(STATUS "[ Compiling SHADERS ]")
foreach(shaderSrc IN LISTS BM_SHADERS_LIST)
    get_filename_component(shaderName ${shaderSrc} NAME)
    set(shaderDst ${BM_SHADERS_DIR}/${shaderName}.spv)
    add_custom_command(COMMAND ${glslcExe} -o ${shaderDst} ${shaderSrc} OUTPUT ${shaderDst})
    list(APPEND EXTRA_SHADERS ${BM_SHADERS_DIR}/${shaderName}.spv)
    MESSAGE(STATUS "  * ${shaderName}")
endforeach()

add_custom_target(shaders ALL DEPENDS ${EXTRA_SHADERS}) # Seems like unneeded


# ------------------------- #
# - COPY ASSETS
# ------------------------- #

bmCopyToBuild(Assets/Geometry)
bmCopyToBuild(Assets/Icons)


# ------------------------- #
# - PROJECT
# ------------------------- #

set(SRC_DIR "Bretema")

file(GLOB BM_HEADERS_CORE ${SRC_DIR}/Core/*.hpp)
file(GLOB BM_HEADERS_ENGINE ${SRC_DIR}/bm/*.hpp)
file(GLOB BM_HEADERS_PLATFORM_VK ${SRC_DIR}/vk/*.hpp)

list(APPEND BM_HEADERS
    ${BM_HEADERS_CORE}
    ${BM_HEADERS_ENGINE}
    ${BM_HEADERS_PLATFORM_VK}

    # ${SRC_DIR}/bm/base.hpp
    # ${SRC_DIR}/bm/utils.hpp
    # ${SRC_DIR}/bm/userInput.hpp
    # ${SRC_DIR}/bm/app.hpp
    # ${SRC_DIR}/bm/window.hpp
    # ${SRC_DIR}/bm/renderer.hpp
    # ${SRC_DIR}/bm/transform.hpp
    # ${SRC_DIR}/bm/camera.hpp

    # ${SRC_DIR}/vk/base.hpp
    # ${SRC_DIR}/vk/renderer.hpp
    # ${SRC_DIR}/vk/str.hpp
    # ${SRC_DIR}/vk/init.hpp
    # ${SRC_DIR}/vk/types.hpp
)

file(GLOB BM_SOURCES_CORE ${SRC_DIR}/Core/*.cpp)
file(GLOB BM_SOURCES_ENGINE ${SRC_DIR}/bm/*.cpp)
file(GLOB BM_SOURCES_PLATFORM_VK ${SRC_DIR}/vk/*.cpp)

list(APPEND BM_SOURCES
    ${BM_SOURCES_CORE}
    ${BM_SOURCES_ENGINE}
    ${BM_SOURCES_PLATFORM_VK}

    # ${SRC_DIR}/bm/app.cpp
    # ${SRC_DIR}/bm/window.cpp
    # ${SRC_DIR}/bm/renderer.cpp

    # ${SRC_DIR}/vk/base.cpp
    # ${SRC_DIR}/vk/renderer.cpp
)

add_library(${PROJECT_NAME}
    STATIC
        ${BM_SHADERS}
        ${BM_HEADERS}
        ${BM_SOURCES}
        ${BM_HEADERS_EXTRA} # @review
        ${BM_SOURCES_EXTRA} # @review
)

# Standard
target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_20
)
set_property(TARGET ${PROJECT_NAME}
    PROPERTY
        CXX_STANDARD 20
)

# Defines
target_compile_definitions(${PROJECT_NAME}
    PRIVATE

        BM_VERSION_MAJOR=1
        BM_VERSION_MINOR=0

        ImTextureID=ImU64
)



target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        Modules
    PRIVATE
        ${BM_INCLUDES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${BM_LIBS}
)


# ------------------------- #
# - EXE and TESTING
# ------------------------- #

function(bmAddExe exeName exeSources)
    add_executable(${exeName} ${exeSources} ${RES_FILES})
    target_include_directories(${exeName} PRIVATE ${BM_INCLUDES})
    target_link_libraries(${exeName} PRIVATE ${BM_LIBS} ${PROJECT_NAME})
endfunction()

function(bmAddTest testName testSources)
    set(testName test_${testName})
    bmAddExe(${testName} ${testSources})
    catch_discover_tests(${testName})
endfunction()


if (OPT_TESTS)
bmAddTest(sum2 Tests/Sum2.cpp)
endif()

bmAddExe(ImGuiDemo Tests/ImGuiDemo.cpp)
bmAddExe(main Tests/main.cpp)
