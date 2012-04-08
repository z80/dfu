
cmake_minimum_required(VERSION 2.8)

include_directories( "./src" )

#file( GLOB src "./src" "*.c" )
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )
add_library( dfu ${src} )

find_library( libusb NAMES libusb.a PATHS $ENV{PATH} ${LIBUSB_PATH} )
target_link_libraries( dfu ${libusb} )

