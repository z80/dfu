
find_package( Qt4 )
set( QT_USE_QTCORE True )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     ${BOOST_INC_PATH} 
                     "${ROOT}/3dparty-src/lua-5.1.5/src" 
                     "${ROOT}/3dparty-src/libqtlua-1.4/src" )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

add_library( qtluaex STATIC ${src} )
target_link_libraries( qtluaex liblua-5.1.5 qtlua ${QT_LIBRARIES} )

