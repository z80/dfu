
cmake_minimum_required( VERSION 2.8 )

# ѕереопределение условий компил€ции, чтобы 
# не нужно было Microsoft Redistributable 
# устанавливать.
#set(CMAKE_USER_MAKE_RULES_OVERRIDE
#    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
#set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
#    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

project( "project" )
set( INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/app" CACHE PATH "Path to install to" )
set( CMAKE_INSTALL_PREFIX ${INSTALL_PATH} )
set( ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
if ( WIN32 )
    include( "./config-win32.cmake" )
elseif ( UNIX )
    include( "./config-unix.cmake" )
endif ( WIN32 )

install( DIRECTORY "./files/" DESTINATION "." )
add_subdirectory( "./3dparty-src/qwt-6.0.1" )
add_subdirectory( "./3dparty-src/lua-5.1.5" )
add_subdirectory( "./3dparty-src/libqtlua-1.4" )
add_subdirectory( "./3dparty-src/pawn/source/compiler" )
add_subdirectory( "./3dparty-src/pawn/source/amx" )
add_subdirectory( "./lib-qtluaex" )
add_subdirectory( "./lib-widgets" )
add_subdirectory( "./plugins" )
add_subdirectory( "./lua-plugins" )
add_subdirectory( "./lib-ctrlboardio" )
add_subdirectory( "./lua-application" ) # For testing lib-qtluaex project
add_subdirectory( "./tests" )

