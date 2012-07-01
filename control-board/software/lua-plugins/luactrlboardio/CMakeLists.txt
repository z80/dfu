
find_package( Qt4 )
set( QT_USE_QT_CORE True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/3dparty-src/lua-5.1.5/src" 
                     "${ROOT}/lib-ctrlboardio/src" )

aux_source_directory( "./src" src )
add_library( luactrlboardio SHARED ${src} )
if ( WIN32 )
   list( APPEND defs DECLSPEC=__declspec(dllexport) )
endif ( WIN32 )
set_property( TARGET luactrlboardio PROPERTY COMPILE_DEFINITIONS ${defs} )
set_property( TARGET luactrlboardio PROPERTY OUTPUT_NAME "luactrlboardio" )
set_target_properties( luactrlboardio PROPERTIES PREFIX "" )
target_link_libraries( luactrlboardio liblua-5.1.5 ctrlboardio )

install( TARGETS luactrlboardio DESTINATION "./" )



