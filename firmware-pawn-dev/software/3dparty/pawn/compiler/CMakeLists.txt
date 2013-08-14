
IF(UNIX)
  ADD_DEFINITIONS(-DENABLE_BINRELOC -D_GNU_SOURCE)
  INCLUDE_DIRECTORIES( ../linux)
  LINK_LIBRARIES(pthread m) #CMake adds the "lib" prefix, so "pthread" becomes "libpthread"
ENDIF(UNIX)
IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

# The Pawn compiler
SET( PAWNCC_SRCS sc1.c sc2.c sc3.c sc4.c sc5.c sc6.c sc7.c
     scexpand.c sci18n.c sclist.c scmemfil.c scstate.c scvars.c
     lstring.c memfile.c
     ${CMAKE_CURRENT_SOURCE_DIR}/../amx/keeloq.c )

IF(UNIX)
  SET(PAWNCC_SRCS ${PAWNCC_SRCS} ../linux/binreloc.c)
ENDIF(UNIX)
add_executable( pawncc ${PAWNCC_SRCS} )
install( TARGETS pawncc DESTINATION "./pawn" )


# Simple Pawn disassembler
set( PAWNDISASM_SRCS pawndisasm.c )
add_executable(pawndisasm ${PAWNDISASM_SRCS})
install( TARGETS pawndisasm DESTINATION "./pawn" )
