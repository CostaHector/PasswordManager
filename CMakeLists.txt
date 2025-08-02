cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME PasswordManager)
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
)

file(GLOB SRV_SRC_CPP
    source/*.cpp
)

file(GLOB SRV_TOOLS_SRC_CPP
    tools/*.cpp
)

set(SRV_HEADER_PATH
    include
    tools
)

file(GLOB SRV_HEADER_FILES
    include/*.h
)

file(GLOB SRV_TOOLS_HEADER_FILES
    tools/*.h
)

list(LENGTH SRV_SRC_CPP src_cpp_count)
message("\n> ${src_cpp_count} file(s) in SRV_SRC_CPP, there are:")
foreach(file ${SRV_SRC_CPP})
    message("${file}")
endforeach()

list(LENGTH SRV_HEADER_PATH header_count)
message("\n> ${header_count} folder(s) in SRV_HEADER_PATH, there are:")
foreach(folder ${SRV_HEADER_PATH})
    message("${folder}")
endforeach()

list(LENGTH SRV_HEADER_FILES include_h_count)
message("\n> ${include_h_count} file(s) in SRV_HEADER_FILES, there are:")
foreach(file ${SRV_HEADER_FILES})
    message("${file}")
endforeach()

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${SRV_SRC_CPP}
    ${SRV_TOOLS_SRC_CPP}
    materials/ResourceCollectFiles.qrc
)

# 添加头文件
target_sources(${PROJECT_NAME} PUBLIC
    ${SRV_HEADER_FILES}
    ${SRV_TOOLS_HEADER_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRV_HEADER_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# OpenSSL configuration for Windows
if(WIN32)
    # Set OpenSSL paths
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")

    # Include directories
    include_directories("${OPENSSL_ROOT_DIR}/include")

    # Library paths
    link_directories("${OPENSSL_ROOT_DIR}/lib")
    link_directories("${OPENSSL_ROOT_DIR}/lib/VC/x64")

    # Debug vs Release library selection
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libssl.lib"
            "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libcrypto.lib"
        )
        message(STATUS "Linking with OpenSSL Debug libraries")
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libssl.lib"
            "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libcrypto.lib"
        )
        message(STATUS "Linking with OpenSSL Release libraries")
    endif()
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
