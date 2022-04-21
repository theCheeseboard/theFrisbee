cmake_minimum_required(VERSION 3.21.0)

project(thefrisbee VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary REQUIRED)

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

cntp_init(thefrisbee 17)
cntp_translate(thefrisbee)
cntp_target_name(thefrisbee "theFrisbee")
cntp_target_desktop_file(thefrisbee
    DESKTOP_FILE com.vicr123.thefrisbee.desktop
    BLUEPRINT_DESKTOP_FILE com.vicr123.thefrisbee_blueprint.desktop)

target_link_libraries(thefrisbee Qt6::Widgets Qt6::DBus libcontemporary libthefrisbee)
target_include_directories(thefrisbee PUBLIC ../libthefrisbee/)

install(TARGETS thefrisbee
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)
