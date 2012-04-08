
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include_directories( "./src" 
                     "../libdfu/src" )

add_executable( dfu-cmd ${src} )
target_link_libraries( dfu-cmd dfu )



