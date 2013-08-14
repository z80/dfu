
find_package( Qt4 )
set( QT_USE_QT_CORE True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/3dparty/lua-5.1.5/src" 
                     "${ROOT}/mcuctrl/src" )

aux_source_directory( "./src" src )
#add_library( luamcuctrl SHARED ${src} )
#if ( WIN32 )
#   list( APPEND defs "DECLSPEC=__declspec(dllexport)" )
#endif ( WIN32 )
add_library( luamcuctrl STATIC ${src} )
set_property( TARGET luamcuctrl PROPERTY COMPILE_DEFINITIONS ${defs} )
set_property( TARGET luamcuctrl PROPERTY OUTPUT_NAME "luamcuctrl" )
set_target_properties( luamcuctrl PROPERTIES PREFIX "" )
target_link_libraries( luamcuctrl liblua-5.1.5 mcuctrl )

#install( TARGETS luamcuctrl DESTINATION "./" )



