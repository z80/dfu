
find_package( Qt4 )
set( QT_USE_QTCORE    True )
set( QT_USE_QTGUI     True )
set( QT_USE_QTUITOOLS True )
if ( WIN32 )
    set( QT_USE_QTMAIN True )
endif ( WIN32 )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/lib-qtluaex/src" 
                     "${ROOT}/lib-widgets/src" 
                     "${ROOT}/3dparty-src/lua-5.1.5/src" 
                     "${ROOT}/3dparty-src/libqtlua-1.4/src" 
                     "${ROOT}/3dparty-src/qwt-6.0.1/src" 
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.cpp" )
file( GLOB headers "./src/*.h" )
file( GLOB ui      "./src/*.ui" )
file( GLOB qrc     "./src/*.qrc" )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

Qt4_WRAP_CPP( mocSrc ${headers} )
set( src ${src} ${mocSrc} )

QT4_WRAP_UI( uiSrc ${ui} )
set( src ${src} ${uiSrc} )

QT4_ADD_RESOURCES( qrcSrc ${qrc} )
set( src ${src} ${qrcSrc} )

add_executable( application ${src} )
target_link_libraries( application qtluaex widgets ${QT_LIBRARIES} )
install( TARGETS application DESTINATION "." )
