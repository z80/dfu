
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include( "../cmake/findlibusb-1.0.cmake" )
message( STATUS ${LIBUSB_1_DEFINITIONS} )

include_directories( "./src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
elseif ( WIN32 )

endif ( UNIX )

add_library( usbio STATIC ${src} )
target_link_libraries( usbio ${LIBUSB_1_LIBRARIES} )




