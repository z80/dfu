
find_package( Qt4 )
set( QT_USE_QT_CORE True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/lua-5.1.5/src" 
                     "${ROOT}/lib-deviceio/src" 
                     "${ROOT}/lib-usbio/src" )

aux_source_directory( "./src" src )
add_library( luadeviceio SHARED ${src} )
set_property( TARGET luadeviceio PROPERTY COMPILE_DEFINITIONS DEVICEIO_DLL )
target_link_libraries( luadeviceio lua-5.1.5 deviceio )

install( TARGETS luadeviceio DESTINATION "./" )



