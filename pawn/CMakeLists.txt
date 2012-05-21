
cmake_minimum_required( VERSION 2.6 )
project( "pawnrun" )

set( STACK_SIZE 1024 CACHE STRING "Array for stack and heap size in bytes. MUST be multiple of 4." )
set( POOL_SIZE  1024 CACHE STRING "Array for overlays size in bytes" )
configure_file( "./src/pawn_cfg.in.h" "${CMAKE_CURRENT_BINARY_DIR}/pawn_cfg.h" )

include_directories( "./src"
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.c" )
file( GLOB headers "./src/*.h" )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

add_executable( pawnrun ${src} )
install( TARGETS pawnrun DESTINATION "./" )
