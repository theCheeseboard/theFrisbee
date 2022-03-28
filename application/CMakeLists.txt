cmake_minimum_required(VERSION 3.21.0)

project(thefrisbee VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(GNUInstallDirs)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary REQUIRED)
#cntp_generate_translations(${CMAKE_CURRENT_LIST_DIR})

add_compile_definitions(SYSTEM_LIBRARY_DIRECTORY="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

set(SOURCES
        diskPanes/diskpanecomponent.cpp
        diskPanes/overviewdiskpane.cpp
        diskpane.cpp
        main.cpp
        mainwindow.cpp
        operations/creatediskimagepopover.cpp

        diskPanes/overviewdiskpane.ui
        diskpane.ui
        mainwindow.ui
        operations/creatediskimagepopover.ui

        resources.qrc)

set(HEADERS
        diskPanes/diskpanecomponent.h
        diskPanes/overviewdiskpane.h
        diskpane.h
        mainwindow.h
        operations/creatediskimagepopover.h)

add_executable(thefrisbee ${SOURCES} ${HEADERS})
set_target_properties(thefrisbee PROPERTIES
        MACOSX_BUNDLE TRUE
        CNTP_DATA_SUBDIR thefrisbee)

cntp_translate(thefrisbee)

target_link_libraries(thefrisbee Qt6::Widgets Qt6::DBus libcontemporary libthefrisbee)
target_include_directories(thefrisbee PUBLIC ../libthefrisbee/)

install(TARGETS thefrisbee
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)