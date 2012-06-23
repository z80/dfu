
include_directories( "./src"
                     "./src/libusb"
                     ${CMAKE_CURRENT_BINARY_DIR} )
if ( WIN32 )
    include_directories( ${LIBUSB_INC_PATH} )
endif ( WIN32 )

aux_source_directory( "./src" src )
aux_source_directory( "./src/libusb" src_libusb )
list( APPEND src ${src_libusb} )

add_library( usbio STATIC ${BLD_TYPE} ${src} )

if ( WIN32 )
    list( APPEND searchpaths ${LIBUSB_LIB_PATH} )
elseif ( UNIX )
    if ( CROSS )
        list( APPEND searchpaths ${LIB_PATH_EMB} )
    else ( CROSS )
        list( APPEND searchpaths "." "/usr/lib" $ENV{PATH} )
    endif ( CROSS )
endif ( WIN32 )
message( INFO Search paths = "${searchpaths}" )
find_library( libusb NAMES libusb.a libusb.lib PATHS ${searchpaths} )
target_link_libraries( usbio ${libusb} )





