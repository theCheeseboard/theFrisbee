cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus)
find_package(libcontemporary)

set(SOURCES
        DriveObjects/blockinterface.cpp
        DriveObjects/diskinterface.cpp
        DriveObjects/diskobject.cpp
        DriveObjects/driveinterface.cpp
        DriveObjects/encryptedinterface.cpp
        DriveObjects/filesysteminterface.cpp
        DriveObjects/loopinterface.cpp
        DriveObjects/partitioninterface.cpp
        DriveObjects/partitiontableinterface.cpp
        components/partitionvisualisation.cpp
        components/sizeeditbox.cpp
        diskmodel.cpp
        diskoperationmanager.cpp
        driveobjectmanager.cpp
        jobs/editpartitionjob.cpp
        jobs/erasecdrwjob.cpp
        jobs/progress/editpartitionjobprogress.cpp
        jobs/progress/erasecdrwjobprogress.cpp
        jobs/progress/restorediskjobprogress.cpp
        jobs/progress/restoreopticaljobprogress.cpp
        jobs/restorediskjob.cpp
        jobs/restorejob.cpp
        jobs/restoreopticaljob.cpp
        operations/eraseopticalpopover.cpp
        operations/erasepartitionpopover.cpp
        operations/erasepartitiontablepopover.cpp
        operations/imagepopover.cpp
        operations/partitionpopover.cpp
        operations/restoreopticalpopover.cpp
        optical/opticalerrortracker.cpp
        partitioninformation.cpp)

file(GLOB ROOT_HEADERS *.h)
file(GLOB DRIVEOBJECT_HEADERS DriveObjects/*.h)

set(HEADERS ${ROOT_HEADERS} ${DRIVEOBJECT_HEADERS})

add_library(libthefrisbee SHARED ${SOURCES})
cntp_init(libthefrisbee 17)
cntp_target_public_headers(libthefrisbee
    DESTINATION_DIRECTORY libthefrisbee
    HEADERS ${HEADERS})
set_target_properties(libthefrisbee PROPERTIES
        OUTPUT_NAME thefrisbee
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthefrisbee
        VERSION 1.0.0)

target_link_libraries(libthefrisbee Qt6::Widgets Qt6::DBus libcontemporary)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/libthefrisbee.framework/Resources/CMake)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libthefrisbee)
ENDIF()

configure_package_config_file(libthefrisbeeConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libthefrisbeeConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libthefrisbeeConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT libthefrisbeeTargets
        FILE libthefrisbeeTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(FILES com.vicr123.trigger-uevent.policy
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions)
install(FILES trigger-uevent.sh
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/libthefrisbee)

install(TARGETS libthefrisbee
        EXPORT libthefrisbeeTargets
        INCLUDES DESTINATION ${libthefrisbee_INCLUDE_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})
