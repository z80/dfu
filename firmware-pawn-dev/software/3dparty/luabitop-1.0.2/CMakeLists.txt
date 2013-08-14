
include_directories( "./" 
                     "${ROOT}/3dparty/lua-5.1.5/src" )

file( GLOB src     "./*.c" )
message( STATUS "Project \"bit\"" )
message( STATUS "SRC = ${src}" )

if ( WIN32 )
    list( APPEND defs "DECLSPEC=__declspec(dllexport)" )
elseif ( UNIX )
    list( APPEND defs "DECLSPEC=" )
endif ( WIN32 )

add_library( bit SHARED ${src} )
set_property( TARGET bit PROPERTY COMPILE_DEFINITIONS ${defs} )
set_property( TARGET bit PROPERTY OUTPUT_NAME "bit" )
set_target_properties( bit PROPERTIES PREFIX "" )
target_link_libraries( bit liblua-5.1.5 )

install( TARGETS bit DESTINATION "./" )


