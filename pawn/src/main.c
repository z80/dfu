
#include "amx.h"
#include "pawn_loader.h"
#include <stdio.h>

static cell printi( AMX * amx,  const cell * params );
static cell print( AMX * amx,  const cell * params );

int main(int argc,char *argv[])
{
    Pawn * p;
    int res;
    static AMX_NATIVE_INFO decls[] = 
    {
        { "printi", printi }, 
        { "print",  print }, 
        { 0,       0 }, 
    };
    if ( argc < 2 )
    {
        printf( "Script name was expected, actually...\n" );
        return 1;
    }
    p = pawn();
    res = pawnLoad( p, argv[1] );
    res = amx_Register( pawnAmx(), decls, -1 );
    res = pawnExec( p );
    return res;
}

static cell printi( AMX * amx,  const cell * params )
{
    int val = (int)params[1];
    printf( "val = %i\n", val );
    return 0;
}

static cell print( AMX * amx,  const cell * params )
{
    char stri[128];
    int l;
    cell * cstri;
    amx_GetAddr( amx, params[1], &cstri );
    amx_StrLen( cstri, &l );
    if ( l > 0 )
    {
        amx_GetString( stri, cstri, 0, l+1 );
        printf( "%s", stri );
    }
    return 0;
}
