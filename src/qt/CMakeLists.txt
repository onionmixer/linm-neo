find_package(Qt5 COMPONENTS Widgets QUIET)
if(NOT Qt5_FOUND)
    message(STATUS "Qt5 not found - skipping Qt GUI build")
    return()
endif()

message(STATUS "Qt5 found - building Qt GUI (linm-qt)")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LINM_QT_SRCS
    qtpanel/qt_panel.cpp
    qtpanel/qtlinm_main.cpp
    qtpanel/qt_dialog.cpp
    qtpanel/qt_paneltooltip.cpp
    qtpanel/qt_panelfilebox.cpp
    qtpanel/qt_mainwindow.cpp
    qtpanel/qt_statusbar.cpp
    qtpanel/qt_menu.cpp
    qtpanel/qt_command.cpp
    qtpanel/qt_tabpanel.cpp
    qtpanel/qt_mcd.cpp
    qtpanel/qt_funcbar.cpp
)

add_executable(linm-qt ${LINM_QT_SRCS})

target_include_directories(linm-qt PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/panel
    ${CMAKE_SOURCE_DIR}/panel/common
    ${CMAKE_SOURCE_DIR}/panel/vfs
    ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
    ${CMAKE_SOURCE_DIR}/panel/vfs/archive
    ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2
    ${CMAKE_SOURCE_DIR}/editor
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/qtpanel
)

# Link order matters for static libraries: dependents before dependencies
target_link_libraries(linm-qt
    Qt5::Widgets
    mlscommon
    vfsplugin
    editor
    panelcommon
    linmpluginarc
    linmpluginftp
    mlscommon
    ${CMAKE_THREAD_LIBS_INIT}
    z
)

if (LIBICONV AND LIBINTL)
    target_link_libraries(linm-qt ${LIBICONV} ${LIBINTL})
endif()

if ( OPENSSL_FOUND AND LIBSSH2_LIBRARIES )
    add_dependencies(linm-qt linmpluginsftp)
    target_link_libraries(linm-qt linmpluginsftp ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${LIBSSH2_LIBRARY})
endif()

if ( SMBCLIENT_FOUND AND __LINM_SAMBA_USE__ )
    add_dependencies(linm-qt linmpluginsmb)
    target_link_libraries(linm-qt linmpluginsmb ${SMBCLIENT_LIBRARIES})
endif()

install(TARGETS linm-qt DESTINATION ${CMAKE_INSTALL_BINDIR})
