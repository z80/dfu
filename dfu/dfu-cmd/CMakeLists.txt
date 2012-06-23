
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include( "../cmake/findlibusb-1.0.cmake" )
message( STATUS ${LIBUSB_1_DEFINITIONS} )

include_directories( "./src" 
                     "../libdfu/src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
elseif ( WIN32 )

endif ( UNIX )

add_executable( dfu-cmd ${src} )
target_link_libraries( dfu-cmd dfu ${LIBUSB_1_LIBRARIES} )




