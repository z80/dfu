
#include "amx.h"
#include "pawn_loader.h"
#include <stdio.h>

static cell printi( AMX * amx,  const cell * params );
static cell print( AMX * amx,  const cell * params );
static AMX_NATIVE_INFO decls[] = 
{
    { "printi", printi }, 
    { "print",  print }, 
    { 0,       0 }, 
};

int main(int argc,char *argv[])
{
    Pawn * p;
    int res;
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
    int cnt = params[0] / sizeof(cell);
    int val;
    int i;
    for ( i=1; i<=cnt; i++ )
    {
        val = (int)params[i];
        printf( "%i%s", val, (i<cnt) ? " " : "" );
    }
    printf( "\n" );
    return 0;
}

static cell print( AMX * amx,  const cell * params )
{
    char stri[128];
    int l, i;
    //cell * cstri;
    int cnt = params[0] / sizeof(cell);
    for ( i=1; i<=cnt; i++ )
    {
        //amx_GetAddr( amx, params[i], &cstri );
        amx_StrLen( (cell *)params[i], &l );
        if ( l > 0 )
        {
            amx_GetString( stri, (cell *)params[i], 0, l+1 );
            printf( "%s", stri );
        }
    }
    return 0;
}
