cmake_minimum_required(VERSION 3.25)
project(MiInterfazQt)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(mongocxx_DIR "C:/vcpkg/installed/x64-windows/share/mongocxx")
set(CMAKE_CXX_STANDARD 23)


set(CMAKE_AUTOUIC_SEARCH_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/Interfaz/forms
)
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)


# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)


# Habilitar MOC y UIC automáticos
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Archivos de código
set(SOURCES
        main.cpp
        Interfaz/src/mainwindow.cpp
        Interfaz/src/CppHighlighter.cpp
        Interfaz/src/CodeEditor.cpp
        Interfaz/src/LLMservice.h
)

set(HEADERS
        Interfaz/src/mainwindow.h
        Interfaz/src/CppHighlighter.h
        Interfaz/src/CodeEditor.h
        Interfaz/src/LLMservice.h
)

set(UIS
        Interfaz/forms/mainwindow.ui
        Interfaz/src/CppHighlighter.h
        Interfaz/src/Compiler.cpp
        Interfaz/src/Compiler.h
        Interfaz/src/LLMservice.cpp
        Interfaz/src/LLMservice.h
)

# Crear ejecutable
add_executable(MiInterfazQt ${SOURCES} ${HEADERS} ${UIS}
        Interfaz/src/LLMservice.h)

target_include_directories(MiInterfazQt PUBLIC
        Interfaz/src
)

target_link_libraries(MiInterfazQt PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)



