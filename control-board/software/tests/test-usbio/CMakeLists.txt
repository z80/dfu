
include_directories( "./src"
                     "${ROOT}/lib-usbio/src" 
                     "${ROOT}/lib-deviceio/src" 
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
message( INFO "SRC = ${src}" )
message( INFO "HEADERS = ${headers}" )

add_executable( test-usbio ${src} )
target_link_libraries( test-usbio deviceio )
install( TARGETS test-usbio DESTINATION "." )





