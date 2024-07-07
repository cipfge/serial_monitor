set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

find_package(QT
    NAMES Qt6 Qt5
    COMPONENTS Widgets SerialPort
    REQUIRED
    )

find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS Widgets SerialPort
    REQUIRED
    )

configure_file(
    version.hpp.in
    ${CMAKE_BINARY_DIR}/src/version.hpp
    )

set(SM_SOURCE_FILES
    serial_monitor.cpp
    main_window.cpp
    main_window.hpp
    main_window.ui
    settings_dialog.cpp
    settings_dialog.hpp
    settings_dialog.ui
    status_widget.cpp
    status_widget.hpp
    status_widget.ui
    )

set(SM_RESOURCE_FILES
    serial_monitor.qrc
    )

if(WIN32)
    set(SM_GUI_TYPE WIN32)
    list(APPEND SM_RESOURCE_FILES "win32/serial_monitor.rc")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(serial_monitor
    	${SM_GUI_TYPE}
        ${SM_SOURCE_FILES}
        ${SM_RESOURCE_FILES}
    )
else()
    add_executable(serial_monitor
        ${SM_GUI_TYPE}
        ${SM_SOURCE_FILES}
        ${SM_RESOURCE_FILES}
    )
endif()

target_link_libraries(serial_monitor
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::SerialPort
    )

set_target_properties(serial_monitor
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET serial_monitor POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}"
            "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:serial_monitor>"
        COMMENT "Running windeployqt..."
        )
endif()
