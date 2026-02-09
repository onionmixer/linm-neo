include_directories(${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2 )

########### next target ###############

if( __LINM_SAMBA_USE__ AND SMBCLIENT_FOUND )
    SET(linmpluginsmb_LIB_INCLUDES
            SMBReader.h
            )
    SET(linmpluginsmb_LIB_SRCS
       SMBReader.cpp
    )
    add_library(linmpluginsmb STATIC ${linmpluginsmb_LIB_SRCS} ${linmpluginsmb_LIB_INCLUDES})
    if ( SMBCLIENT_INCLUDE_DIRS )
        target_include_directories(linmpluginsmb PRIVATE ${SMBCLIENT_INCLUDE_DIRS})
    endif()
endif()
