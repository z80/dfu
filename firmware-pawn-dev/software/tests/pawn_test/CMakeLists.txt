
include_directories( "./src" 
                      )
if ( UNIX )
    include_directories( "${ROOT}/3dparty/pawn/linux" )
endif ( UNIX )

file( GLOB src     "./src/*.cpp" )
file( GLOB csrc     "./src/*.c" )
set( src ${src} ${csrc} )
file( GLOB headers "./src/*.h" )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )
IF (UNIX)
  SET( src ${src} ${ROOT}/3dparty/pawn/linux/binreloc.c)
  IF(NOT HAVE_CURSES_H)
    SET( src ${src} ${ROOT}/3dparty/pawn/linux/getch.c)
  ENDIF(NOT HAVE_CURSES_H)
ENDIF (UNIX)

message( STATUS ${defs} )
add_executable( pawn_test ${src} )
IF (UNIX)
  IF(HAVE_CURSES_H)
   SET_TARGET_PROPERTIES( pawn_test PROPERTIES COMPILE_FLAGS -DUSE_CURSES )
    TARGET_LINK_LIBRARIES(pawn_test dl curses)
  ELSE(HAVE_CURSES_H)
    TARGET_LINK_LIBRARIES(pawn_test dl)
  ENDIF(HAVE_CURSES_H)
ENDIF (UNIX)
set( defs   )
add_definitions( -DAMX_DONT_RELOCATE )
add_definitions( -DAMX_TOKENTHREADING )
add_definitions( -DNDEBUG )
add_definitions( -DAMX_NATIVETABLE=g_nativetable )

install( TARGETS pawn_test DESTINATION "./pawn" )
install( DIRECTORY ./pawn_src/ DESTINATION "./pawn" )




