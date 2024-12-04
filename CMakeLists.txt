# CMakeList.txt : CMake project for 3DEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("3DEngine")
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Windows/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Windows/${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Windows/${CMAKE_BUILD_TYPE})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Linux/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Linux/${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/build/Linux/${CMAKE_BUILD_TYPE})

  if (CMAKE_GENERATOR STREQUAL "Unix Makefiles" OR CMAKE_GENERATOR STREQUAL "Ninja")
  add_custom_target(build_log ALL
    COMMAND ${CMAKE_COMMAND} -- build ${CMAKE_BINARY_DIR} --config $<CONFIG>
    > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/build_log.txt 2>&1
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating build log..."
    VERBATIM)
endif()
endif()

# Add GLFW
add_subdirectory(deps/glfw)
include_directories(deps/glfw/include)

# Add GLAD
add_library(glad deps/glad/src/glad.c "src/engine/ecs/include/Entity.h" "src/engine/ecs/include/Component.h" "src/engine/ecs/include/System.h" "src/engine/ecs/include/ECSManager.h" "src/engine/ecs/src/Entity.cpp" "src/engine/ecs/src/Component.cpp" "src/engine/ecs/src/System.cpp" "src/engine/ecs/src/ECSManager.cpp" "src/engine/renderer/include/Renderer.h" "src/engine/renderer/include/OpenGLRenderer.h" "src/engine/renderer/src/Renderer.cpp" "src/engine/renderer/src/OpenGLRenderer.cpp" "src/engine/core/include/Window.h" "src/engine/core/src/Window.cpp" "src/engine/renderer/include/MeshRenderer.h" "src/engine/renderer/src/MeshRenderer.cpp" "src/engine/renderer/include/RenderSystem.h" "src/engine/renderer/include/Shader.h" "src/engine/renderer/src/Shader.cpp" "src/engine/renderer/include/ShaderComponent.h" "src/engine/renderer/include/MeshRendererComponent.h")
target_include_directories(glad PUBLIC deps/glad/include glm)

# Fetch FLM using CMake FetchContent
# FetchContent_Declare(
#   glm
#   GIT_REPOSITORY https://github.com/g-truc/glm.git
#   GIT_TAG 0.9.9.8 # or use a specific version, e.g., 0.9.9.8
# )
# FetchContent_MakeAvailable(glm)

# Add GLM manually
#add_subdirectory(deps/glm)
include_directories(deps/glm)

# Add source to this project's executable.
add_executable(3DEngine src/main.cpp "src/engine/ecs/include/Entity.h" "src/engine/ecs/include/Component.h" "src/engine/ecs/include/System.h" "src/engine/ecs/include/ECSManager.h" "src/engine/ecs/src/Entity.cpp" "src/engine/ecs/src/Component.cpp" "src/engine/ecs/src/System.cpp" "src/engine/ecs/src/ECSManager.cpp" "src/engine/renderer/include/Renderer.h" "src/engine/renderer/include/OpenGLRenderer.h" "src/engine/renderer/src/Renderer.cpp" "src/engine/renderer/src/OpenGLRenderer.cpp" "src/engine/core/include/Window.h" "src/engine/core/src/Window.cpp" "src/engine/renderer/include/MeshRenderer.h" "src/engine/renderer/src/MeshRenderer.cpp" "src/engine/renderer/include/RenderSystem.h" "src/engine/renderer/include/Shader.h" "src/engine/renderer/src/Shader.cpp" "src/engine/renderer/include/ShaderComponent.h" "src/engine/renderer/include/MeshRendererComponent.h")
target_link_libraries(3DEngine glfw glad)

# Notify build start
#add_custom_target(
#  notify_start ALL
#  COMMAND python ${CMAKE_SOURCE_DIR}/scripts/build-sys/notify_build.py --status=start
#)

# Notify build success or failure
#add_custom_target(
#    notify_finish ALL
#    COMMAND ${CMAKE_COMMAND} -E time python ${CMAKE_SOURCE_DIR}/scripts/notify_build.py --status=success --duration="15" --warnings="warr" --errors="err"
#    DEPENDS 3DEngine
#)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 3DEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
