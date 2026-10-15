cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "tp2-asteroids")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(OpenGL REQUIRED)

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
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/DrawComponent.cpp
        Source/Components/DrawComponent.h
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Actors/Ship.cpp
        Source/Actors/Ship.h
        Source/Actors/Asteroid.cpp
        Source/Actors/Asteroid.h
        Source/Components/CircleColliderComponent.cpp
        Source/Components/CircleColliderComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW SDL2::SDL2 SDL2::SDL2main OpenGL::GL)