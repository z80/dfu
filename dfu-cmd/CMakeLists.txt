
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include_directories( "./src" 
                     "../libdfu/src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "/usr/include/libusb-1.0" )
elseif ( WIN32 )

endif ( UNIX )


add_executable( dfu-cmd ${src} )
target_link_libraries( dfu-cmd dfu )



