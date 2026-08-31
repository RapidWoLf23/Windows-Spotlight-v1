cmake_minimum_required(VERSION 3.16)

project(WindowsSpotlight VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

qt_add_executable(WindowsSpotlight
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/Indexer.cpp
    src/Indexer.h
)

target_link_libraries(WindowsSpotlight PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    target_link_libraries(WindowsSpotlight PRIVATE
        dwmapi
        shell32
    )

    set_target_properties(WindowsSpotlight PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(MINGW)
    target_compile_options(WindowsSpotlight PRIVATE
        -O2
        -pipe
    )
endif()

set_target_properties(WindowsSpotlight PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)