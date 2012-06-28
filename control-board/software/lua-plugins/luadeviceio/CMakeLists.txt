
find_package( Qt4 )
set( QT_USE_QT_CORE True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "../lib_bayspec/src" 
                     "${ROOT_PATH}/lua-5.1.4/src" 
                     "../../aistdata/src" 
                     "../optic/spgrctrl/src"
                     "../andor_tuning/src" 
                      ${BOOST_PATH} )

aux_source_directory( "./src" src )
add_library( luabayspec SHARED ${src} )
set_property( TARGET luabayspec PROPERTY COMPILE_DEFINITIONS AISTDATA_DLL )
target_link_libraries( luabayspec ${QT_LIBRARIES} libbayspec andor_tuning spgrctrl aistdata lua51 )

install( TARGETS luabayspec DESTINATION "modules_bin" )
if ( USE_AIST )
    install( TARGETS luabayspec DESTINATION "aist/modules_bin" )
endif ( USE_AIST )



