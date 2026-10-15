cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "tp1-pong")
project(${PROJECT_NAME})

find_package(OpenGL REQUIRED)

find_package(SDL2 2.32.2 EXACT REQUIRED)

find_package(GLEW QUIET)

if (NOT TARGET GLEW::GLEW)

    find_path(GLEW_INCLUDE_DIR REQUIRED
            NAMES GL/glew.h
            HINTS "C:/Arquivos de Programas/GLEW/include"
    )

    find_library(GLEW_LIBRARY REQUIRED
            NAMES glew32
            HINTS "C:/Arquivos de Programas/GLEW/lib/Release/x64"
    )

    add_library(GLEW::GLEW UNKNOWN IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
            IMPORTED_LOCATION "${GLEW_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
    )

endif()

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Game.cpp
        Source/Game.h
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Actors/Paddle.cpp
        Source/Actors/Paddle.h
        Source/Actors/Ball.cpp
        Source/Actors/Ball.h)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW SDL2::SDL2 SDL2::SDL2main OpenGL::GL)