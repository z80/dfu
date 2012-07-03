
aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include( "../cmake/findlibusb-1.0.cmake" )
message( STATUS ${LIBUSB_1_DEFINITIONS} )

include_directories( "./src" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
    aux_source_directory( "./src/unix" s )
elseif ( WIN32 )
    include_directories( "${LIBUSB_INC_PATH}" 
                         "./src/libusb" )
    aux_source_directory( "./src/windows" s )
    #aux_source_directory( "./src/libusb" ss )
    #set( s ${s} ${ss} )
endif ( UNIX )
set( src ${src} ${s} )

add_library( ctrlboardio STATIC ${src} )
if ( UNIX )
    target_link_libraries( ctrlboardio ${LIBUSB_1_LIBRARIES} )
elseif ( WIN32 )
    find_library( libusb NAMES libusb-1.0.lib PATHS "${LIBUSB_LIB_PATH}" )
    target_link_libraries( ctrlboardio ${libusb} )
endif ( UNIX )




