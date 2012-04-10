
cmake_minimum_required(VERSION 2.8)

set( VERSION "0.0.0" )
configure_file( "./config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" )
set( GLOB_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( "./libdfu" )
add_subdirectory( "./dfu-cmd" )
#add_subdirectory( "./dfu-gui" )
