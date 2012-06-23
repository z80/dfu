
include_directories( "./src"
                      "${ROOT}/lib-usbio/src" 
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
message( INFO "SRC = ${src}" )
message( INFO "HEADERS = ${headers}" )

add_library( deviceio STATIC ${src} )
target_link_libraries( deviceio usbio )
install( TARGETS deviceio DESTINATION "." )
