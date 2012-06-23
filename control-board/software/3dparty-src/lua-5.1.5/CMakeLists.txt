
set( BUILD_SHARED_LIBS ON )
add_library( liblua-5.1.5  STATIC 
                       src/lapi.c src/lcode.c src/ldebug.c src/ldo.c 
                       src/ldump.c src/lfunc.c src/lgc.c src/llex.c
                       src/lmem.c src/lobject.c src/lopcodes.c 
                       src/lparser.c src/lstate.c src/lstring.c
                       src/ltable.c src/ltm.c src/lundump.c src/lvm.c 
                       src/lzio.c src/lauxlib.c src/lbaselib.c 
                       src/ldblib.c src/liolib.c src/lmathlib.c 
                       src/loslib.c src/ltablib.c src/lstrlib.c 
                       src/loadlib.c src/linit.c )
if ( UNIX )
    target_link_libraries( liblua-5.1.5 m )
endif ( UNIX )

add_executable( lua-5.1.5 src/lua.c )
target_link_libraries( lua-5.1.5 liblua-5.1.5 )

add_executable( luac-5.1.5 src/lapi.c src/lcode.c src/ldebug.c src/ldo.c 
                    src/ldump.c src/lfunc.c src/lgc.c src/llex.c
                    src/lmem.c src/lobject.c src/lopcodes.c 
                    src/lparser.c src/lstate.c src/lstring.c
                    src/ltable.c src/ltm.c src/lundump.c src/lvm.c 
                    src/lzio.c src/lauxlib.c src/lbaselib.c 
                    src/ldblib.c src/liolib.c src/lmathlib.c 
                    src/loslib.c src/ltablib.c src/lstrlib.c 
                    src/loadlib.c src/linit.c src/luac.c 
                    src/print.c )
target_link_libraries( luac-5.1.5 liblua-5.1.5 )


