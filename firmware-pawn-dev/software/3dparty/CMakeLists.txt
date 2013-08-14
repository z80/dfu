add_subdirectory( "./lua-5.1.5" )
add_subdirectory( "./qtlua-1.4" )
add_subdirectory( "./qxmpp-0.7.4" )
add_subdirectory( "./qwt-6.0.1" )
add_subdirectory( "./luabitop-1.0.2" )
add_subdirectory( "./widgets" )
add_subdirectory( "./pawn" )
if ( UNIX )
    add_subdirectory( "./qextserialport" )
    #add_subdirectory( "./libftdi1-1.0" )
endif ( UNIX )






