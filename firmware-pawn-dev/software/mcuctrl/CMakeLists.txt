
find_package( Qt4 )
set( QT_USE_QTCORE    True )
set( QT_USE_QTGUI     True )
include( ${QT_USE_FILE} )

set( Boost_USE_STATIC_LIBS True )
set( Boost_USE_MULTITHREADED True )
find_package( Boost COMPONENTS regex REQUIRED )

aux_source_directory( "./src" src )
message( STATUS "src: \"${src}\"" )

include( "${ROOT}/3dparty/cmake/findlibusb-1.0.cmake" )
message( STATUS ${LIBUSB_1_DEFINITIONS} )

include_directories( "./src" 
                     ${GLOB_BIN_DIR}
                     ${Boost_INCLUDE_DIRS} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
    aux_source_directory( "./src/unix" s )
elseif ( WIN32 )
    include_directories( "${LIBUSB_INC_PATH}" 
                         "./src/libusb" )
endif ( UNIX )
set( src ${src} ${s} )

add_library( mcuctrl STATIC ${src} )
if ( UNIX )
    target_link_libraries( mcuctrl ${LIBUSB_1_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES} )
elseif ( WIN32 )
    find_library( libusb NAMES libusb-1.0.lib PATHS "${LIBUSB_LIB_PATH}" ${QT_LIBRARIES} )
    target_link_libraries( mcuctrl ${libusb} ${Boost_LIBRARIES} )
endif ( UNIX )




