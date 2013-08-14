
find_package( Qt4 )
set( QT_USE_QTCORE    True )
set( QT_USE_QTGUI     True )
set( QT_USE_QTUITOOLS True )
if ( WIN32 )
    set( QT_USE_QTMAIN True )
endif ( WIN32 )
include( ${QT_USE_FILE} )

include_directories( "./src" 
                     "${ROOT}/3dparty/lua-5.1.5/src" 
                     "${ROOT}/3dparty/libqtlua-1.4/src" 
                     "${ROOT}/luamcuctrl/src" 
                     ${Boost_INCLUDE_DIRS} 
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

if ( WIN32 )
    set( BUILD_TYPE WIN32 )
else ( WIN32 )
    set( BUILD_TYPE "" )
endif ( WIN32 )

add_executable( terminal ${BUILD_TYPE} ${src} )
target_link_libraries( terminal liblua-5.1.5 luaftdi luamcuctrl qtlua ${QT_LIBRARIES} )
install( TARGETS terminal DESTINATION "." )
install( DIRECTORY "./src_lua/" DESTINATION "." )

