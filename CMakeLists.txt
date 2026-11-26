cmake_minimum_required(VERSION 3.16)

project(BeaconModules VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appBeaconModules
    main.cpp
)

qt_add_qml_module(appBeaconModules
    URI BeaconModules
    QML_FILES
        Main.qml
        SOURCES src/modules/notemanager.h
        SOURCES src/modules/notemanager.cpp
        SOURCES src/modules/notelistmodel.h
        SOURCES src/modules/notelistmodel.cpp
        SOURCES src/modules/noterepository.h src/modules/noterepository.cpp
        SOURCES src/modules/notetypes.h
        SOURCES src/modules/attachmentlistmodel.h src/modules/attachmentlistmodel.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appBeaconModules PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appBeaconModules
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appBeaconModules
    PRIVATE Qt6::Quick Qt6::Sql
)

target_include_directories(appBeaconModules PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules
)

install(TARGETS appBeaconModules
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_compile_options(appBeaconModules PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(appBeaconModules PRIVATE -fsanitize=address,undefined)
    target_link_options(appBeaconModules PRIVATE -fsanitize=address,undefined)
endif()
