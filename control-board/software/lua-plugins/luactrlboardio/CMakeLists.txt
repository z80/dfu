
find_package( Qt4 )
set( QT_USE_QT_CORE True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/3dparty-src/lua-5.1.5/src" 
                     "${ROOT}/lib-ctrlboardio/src" )

aux_source_directory( "./src" src )
add_library( luactrlboardio SHARED ${src} )
set_property( TARGET luactrlboardio PROPERTY COMPILE_DEFINITIONS CTRLBOARDIO_DLL )
target_link_libraries( luactrlboardio liblua-5.1.5 ctrlboardio )

install( TARGETS luactrlboardio DESTINATION "./" )



