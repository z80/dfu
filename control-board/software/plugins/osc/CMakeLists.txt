
find_package( Qt4 )
set( QT_USE_QTCORE     True )
set( QT_USE_QTGUI      True )
set( QT_USE_QTDESIGNER True )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/lib-widgets/src"
                     "${ROOT}/3dparty-src/qwt-6.0.1/src" 
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
file( GLOB ui      "./src/*.ui" )
file( GLOB qrc     "./src/*.qrc" )
message( INFO "SRC = ${src}" )
message( INFO "HEADERS = ${headers}" )

Qt4_WRAP_CPP( mocSrc ${headers} )
set( src ${src} ${mocSrc} )

QT4_WRAP_UI( uiSrc ${ui} )
set( src ${src} ${uiSrc} )

QT4_ADD_RESOURCES( qrcSrc ${qrc} )
set( src ${src} ${qrcSrc} )

add_library( osc SHARED ${src} )
target_link_libraries( osc widgets ${QT_LIBRARIES} )
install( TARGETS osc DESTINATION "./designer" )
