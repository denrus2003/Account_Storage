cmake_minimum_required(VERSION 3.5)

project(Account_Storage VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
set(OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSLv3/Win_x64")
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Account_Storage
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainwindow.h mainwindow.cpp mainwindow.ui
        record_widget.h record_widget.cpp
        record.h record.cpp
        main.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET passmanager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Account_Storage SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Account_Storage
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Account_Storage PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Account_Storage PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OPENSSL_LIBRARIES})
#target_link_libraries(Account_Storage PRIVATE /usr/lib)
set_target_properties(Account_Storage PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Account_Storage
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#set(OPENSSL_ROOT_DIR "/usr/include/openssl")
#include(FindOpenSSL)
#target_link_libraries(passmanager PRIVATE ${CMAKE_DL_LIBS} OpenSSL::SSL OpenSSL::Crypto)

target_include_directories (
    Account_Storage
    PRIVATE C:/Qt/Tools/OpenSSLv3/Win_x64/bin
)
target_link_directories (
    Account_Storage
    PRIVATE C:/Qt/Tools/OpenSSLv3/Win_x64/bin
)
target_link_libraries(
    Account_Storage
    PRIVATE
)
configure_file(records.json records.json)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Account_Storage)
endif()
