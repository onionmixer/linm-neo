if (UNIX)
    add_custom_command(OUTPUT linm.sh
            COMMAND sed "s%@bindir@%${__LINM_BINPATH__}%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.sh )
    add_custom_command(OUTPUT linm.csh
            COMMAND sed "s%@bindir@%${__LINM_BINPATH__}%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.csh )
    add_custom_command(OUTPUT linm_alias.sh
            COMMAND sed "s%@bindir@%${__LINM_BINPATH__}%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh )
    add_custom_command(OUTPUT linm_alias.csh
            COMMAND sed "s%@bindir@%${__LINM_BINPATH__}%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh )

    add_custom_command(OUTPUT default.cfg
            COMMAND sed "s%@LINM_VERSION@%${VERSION}%" ${CMAKE_CURRENT_SOURCE_DIR}/default.cfg.in > ${CMAKE_CURRENT_BINARY_DIR}/default.cfg )
    add_custom_command(OUTPUT colorset.cfg
            COMMAND sed "s%@LINM_VERSION@%${VERSION}%" ${CMAKE_CURRENT_SOURCE_DIR}/colorset.cfg.in > ${CMAKE_CURRENT_BINARY_DIR}/colorset.cfg )
    add_custom_command(OUTPUT keyset.cfg
            COMMAND sed "s%@LINM_VERSION@%${VERSION}%" ${CMAKE_CURRENT_SOURCE_DIR}/keyset.cfg.in > ${CMAKE_CURRENT_BINARY_DIR}/keyset.cfg )
    add_custom_command(OUTPUT syntexset.cfg
            COMMAND sed "s%@LINM_VERSION@%${VERSION}%" ${CMAKE_CURRENT_SOURCE_DIR}/syntexset.cfg.in > ${CMAKE_CURRENT_BINARY_DIR}/syntexset.cfg )

    add_custom_command(OUTPUT qtkeyset.cfg
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qtkeyset.cfg ${CMAKE_CURRENT_BINARY_DIR}/qtkeyset.cfg
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/qtkeyset.cfg)

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/default_config.h
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_default_config.sh
                    ${CMAKE_CURRENT_BINARY_DIR}/default.cfg
                    ${CMAKE_CURRENT_BINARY_DIR}/colorset.cfg
                    ${CMAKE_CURRENT_BINARY_DIR}/keyset.cfg
                    ${CMAKE_CURRENT_BINARY_DIR}/syntexset.cfg
                    ${CMAKE_CURRENT_BINARY_DIR}/qtkeyset.cfg
                    ${CMAKE_BINARY_DIR}/default_config.h
            DEPENDS default.cfg colorset.cfg keyset.cfg syntexset.cfg qtkeyset.cfg
                    ${CMAKE_CURRENT_SOURCE_DIR}/generate_default_config.sh)

    add_custom_target(linmScript DEPENDS linm.sh linm.csh linm_alias.csh linm_alias.sh)
    add_custom_target(linmConfig DEPENDS default.cfg colorset.cfg keyset.cfg syntexset.cfg qtkeyset.cfg
                      ${CMAKE_BINARY_DIR}/default_config.h)

    install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/colorset.cfg
                ${CMAKE_CURRENT_BINARY_DIR}/default.cfg
                ${CMAKE_CURRENT_BINARY_DIR}/keyset.cfg
                ${CMAKE_CURRENT_BINARY_DIR}/syntexset.cfg
                ${CMAKE_CURRENT_BINARY_DIR}/qtkeyset.cfg
            DESTINATION ${__LINM_CFGPATH__} )

    install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/linm.sh
                ${CMAKE_CURRENT_BINARY_DIR}/linm.csh
                ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh
                ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh
            DESTINATION bin )
endif (UNIX)

add_dependencies(linm linmScript linmConfig)
