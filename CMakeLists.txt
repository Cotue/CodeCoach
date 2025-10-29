cmake_minimum_required(VERSION 3.25)
project(MiInterfazQt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOUIC_SEARCH_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/Interfaz/forms
)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Habilitar MOC y UIC automáticos
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Archivos de código
set(SOURCES
        main.cpp
        Interfaz/src/mainwindow.cpp
        Interfaz/src/CppHighlighter.cpp
        Interfaz/src/CodeEditor.cpp
)

set(HEADERS
        Interfaz/src/mainwindow.h
        Interfaz/src/CppHighlighter.h
        Interfaz/src/CodeEditor.h
)

set(UIS
        Interfaz/forms/mainwindow.ui
        Interfaz/src/CppHighlighter.h
)

# Crear ejecutable
add_executable(MiInterfazQt ${SOURCES} ${HEADERS} ${UIS})

target_include_directories(MiInterfazQt PUBLIC
        Interfaz/src
)

target_link_libraries(MiInterfazQt PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)



