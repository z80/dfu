
find_package( Qt4 )
set( QT_USE_QTCORE     True )
set( QT_USE_QTGUI      True )
set( QT_USE_QTNETWORK  True )
set( QT_USE_QTXML      True )
include( ${QT_USE_FILE} )

include_directories( "./src/base" 
                     "./src/client" 
                     "./src/server" 
                     ${CMAKE_CURRENT_BINARY_DIR} )

set( src ./src/base/QXmppArchiveIq.cpp 
         ./src/base/QXmppBindIq.cpp 
         ./src/base/QXmppBookmarkSet.cpp 
         ./src/base/QXmppByteStreamIq.cpp 
         ./src/base/QXmppCodec.cpp 
         ./src/base/QXmppConstants.cpp 
         ./src/base/QXmppDataForm.cpp 
         ./src/base/QXmppDiscoveryIq.cpp 
         ./src/base/QXmppElement.cpp 
         ./src/base/QXmppEntityTimeIq.cpp 
         ./src/base/QXmppGlobal.cpp 
         ./src/base/QXmppIbbIq.cpp 
         ./src/base/QXmppIq.cpp 
         ./src/base/QXmppJingleIq.cpp 
         ./src/base/QXmppLogger.cpp 
         ./src/base/QXmppMessage.cpp 
         ./src/base/QXmppMucIq.cpp 
         ./src/base/QXmppNonSASLAuth.cpp 
         ./src/base/QXmppPingIq.cpp 
         ./src/base/QXmppPresence.cpp 
         ./src/base/QXmppPubSubIq.cpp 
         ./src/base/QXmppRegisterIq.cpp 
         ./src/base/QXmppResultSet.cpp 
         ./src/base/QXmppRosterIq.cpp 
         ./src/base/QXmppRpcIq.cpp 
         ./src/base/QXmppRtpChannel.cpp 
         ./src/base/QXmppSasl.cpp 
         ./src/base/QXmppSessionIq.cpp 
         ./src/base/QXmppSocks.cpp 
         ./src/base/QXmppStanza.cpp 
         ./src/base/QXmppStream.cpp 
         ./src/base/QXmppStreamFeatures.cpp 
         ./src/base/QXmppStreamInitiationIq.cpp 
         ./src/base/QXmppStun.cpp 
         ./src/base/QXmppUtils.cpp 
         ./src/base/QXmppVCardIq.cpp 
         ./src/base/QXmppVersionIq.cpp 
         ./src/base/qdnslookup.cpp )


if ( UNIX )
    set( src ${src} ./src/base/qdnslookup_unix.cpp )
elseif ( WIN32 )
    set( src ${src} ./src/base/qdnslookup_win.cpp )
endif ( UNIX )         

file( GLOB csrc "./src/client/*.cpp" )
file( GLOB ssrc "./src/server/*.cpp" )
set( src ${src} ${csrc} ${ssrc} )





set( headers ./src/base/QXmppArchiveIq.h 
             ./src/base/QXmppBindIq.h 
             ./src/base/QXmppBookmarkSet.h 
             ./src/base/QXmppByteStreamIq.h 
             ./src/base/QXmppConstants.h 
             ./src/base/QXmppDataForm.h 
             ./src/base/QXmppDiscoveryIq.h 
             ./src/base/QXmppElement.h 
             ./src/base/QXmppEntityTimeIq.h 
             ./src/base/QXmppGlobal.h 
             ./src/base/QXmppIbbIq.h 
             ./src/base/QXmppIq.h 
             ./src/base/QXmppJingleIq.h 
             ./src/base/QXmppLogger.h 
             ./src/base/QXmppMessage.h 
             ./src/base/QXmppMucIq.h 
             ./src/base/QXmppNonSASLAuth.h 
             ./src/base/QXmppPingIq.h 
             ./src/base/QXmppPresence.h 
             ./src/base/QXmppPubSubIq.h 
             ./src/base/QXmppRegisterIq.h 
             ./src/base/QXmppResultSet.h 
             ./src/base/QXmppRosterIq.h 
             ./src/base/QXmppRpcIq.h 
             ./src/base/QXmppRtpChannel.h 
             ./src/base/QXmppSessionIq.h 
             ./src/base/QXmppSocks.h 
             ./src/base/QXmppStanza.h 
             ./src/base/QXmppStream.h 
             ./src/base/QXmppStreamFeatures.h 
             ./src/base/QXmppStreamInitiationIq_p.h 
             ./src/base/QXmppStun.h 
             ./src/base/QXmppUtils.h 
             ./src/base/QXmppVCardIq.h 
             ./src/base/QXmppVersionIq.h 
             ./src/base/QXmppCodec_p.h 
             ./src/base/QXmppSasl_p.h 
             ./src/base/qdnslookup.h
             ./src/base/qdnslookup_p.h )

file( GLOB cheaders "./src/client/*.h" )
file( GLOB sheaders "./src/server/*.h" )
set( headers ${headers} ${cheaders} ${sheaders} )

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

add_library( qxmpp STATIC ${src} )
set( libs ${QT_LIBRARIES} )
if ( WIN32 )
    set( libs ${libs} dnsapi.lib ws2_32.lib )
endif ( WIN32 )
target_link_libraries( qxmpp ${libs} )
set_property( TARGET qxmpp PROPERTY COMPILE_DEFINITIONS QXMPP_BUILD )




