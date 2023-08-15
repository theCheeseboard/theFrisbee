cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets DBus)
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
        DriveObjects/udisksinterface.h DriveObjects/udisksinterface.cpp
        DriveObjects/atadriveinterface.h DriveObjects/atadriveinterface.cpp
        DriveObjects/blocklvm2interface.h DriveObjects/blocklvm2interface.cpp
        DriveObjects/physicalvolumeinterface.h DriveObjects/physicalvolumeinterface.cpp

        DriveObjects/volumegroup.h DriveObjects/volumegroup.cpp
        DriveObjects/logicalvolume.h DriveObjects/logicalvolume.cpp

        components/partitionvisualisation.cpp
        components/sizeeditbox.cpp
        diskmodel.cpp
        diskoperationmanager.cpp
        driveobjectmanager.cpp
        jobs/editpartitionjob.cpp
        jobs/erasecdrwjob.cpp
        jobs/progress/editpartitionjobprogress.cpp jobs/progress/editpartitionjobprogress.ui
        jobs/progress/erasecdrwjobprogress.cpp jobs/progress/erasecdrwjobprogress.ui
        jobs/progress/restorediskjobprogress.cpp jobs/progress/restorediskjobprogress.ui
        jobs/progress/restoreopticaljobprogress.cpp jobs/progress/restoreopticaljobprogress.ui
        jobs/progress/imageblockjobprogress.h jobs/progress/imageblockjobprogress.cpp jobs/progress/imageblockjobprogress.ui
        jobs/restorediskjob.h jobs/restorediskjob.cpp
        jobs/restorejob.h jobs/restorejob.cpp
        jobs/restoreopticaljob.h jobs/restoreopticaljob.cpp
        jobs/imageblockjob.h jobs/imageblockjob.cpp
        operations/eraseopticalpopover.cpp operations/eraseopticalpopover.ui
        operations/erasepartitionpopover.cpp operations/erasepartitionpopover.ui
        operations/erasepartitiontablepopover.cpp operations/erasepartitiontablepopover.ui
        operations/imagepopover.cpp operations/imagepopover.ui
        operations/partitionpopover.cpp operations/partitionpopover.ui
        operations/restoreopticalpopover.cpp operations/restoreopticalpopover.ui
        optical/opticalerrortracker.cpp
        partitioninformation.cpp
        frisbeeexception.cpp
        structures/private/smartattribute.h structures/private/smartattribute.cpp
        structures/formattedsmartattribute.cpp
        volumegroupmodel.h volumegroupmodel.cpp
        volumegrouplvmodel.h volumegrouplvmodel.cpp
    )

set(STRUCTURES_HEADERS
    structures/formattedsmartattribute.h
)

file(GLOB ROOT_HEADERS *.h)
file(GLOB DRIVEOBJECT_HEADERS DriveObjects/*.h)

set(HEADERS ${ROOT_HEADERS} ${DRIVEOBJECT_HEADERS} ${STRUCTURES_HEADERS})

add_library(libthefrisbee SHARED ${SOURCES} ${HEADERS})
set_target_properties(libthefrisbee PROPERTIES
        CNTP_DATA_SUBDIR thefrisbee/libthefrisbee)
cntp_init(libthefrisbee 20)
cntp_translate(libthefrisbee)
cntp_target_public_headers(libthefrisbee
    DESTINATION_DIRECTORY libthefrisbee
    HEADERS ${HEADERS})
set_target_properties(libthefrisbee PROPERTIES
        OUTPUT_NAME thefrisbee
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthefrisbee
        VERSION 1.0.0)

target_link_libraries(libthefrisbee Qt::Widgets Qt::DBus libcontemporary)

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
