
cmake_minimum_required(VERSION 2.8)

include( "../cmake/findlibusb-1.0.cmake" )
message( STATUS ${LIBUSB_1_DEFINITIONS} )

include_directories( "./src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
elseif ( WIN32 )

endif ( UNIX )

#file( GLOB src "./src" "*.c" )
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )
add_library( dfu ${src} )

target_link_libraries( dfu ${lLIBUSB_1_LIBRARIES} )

