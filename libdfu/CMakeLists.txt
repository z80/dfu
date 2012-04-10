
cmake_minimum_required(VERSION 2.8)

include_directories( "./src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "/usr/include/libusb-1.0" )
elseif ( WIN32 )

endif ( UNIX )

#file( GLOB src "./src" "*.c" )
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )
add_library( dfu ${src} )

find_library( libusb NAMES libusb-1.0.a PATHS $ENV{PATH} ${LIBUSB_PATH} )
find_library( libpthread NAMES libpthread.a PATHS $ENV{PATH} )
find_library( libc NAMES libc.a PATHS $ENV{PATH} )

target_link_libraries( dfu ${libusb} ${libpthread} ${libc} )

