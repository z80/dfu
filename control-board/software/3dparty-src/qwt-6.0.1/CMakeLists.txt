
find_package( Qt4 REQUIRED )
set( QT_USE_QTGUI true )
set( QT_USE_QTSVG true )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     ${CMAKE_BINARY_DIR} )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

qt4_wrap_cpp( moc_src ${headers} )
set( src ${src} ${moc_src} )

add_library( qwt-6.0.1 STATIC ${src} )
target_link_libraries( qwt-6.0.1 ${QT_LIBRARIES} )









