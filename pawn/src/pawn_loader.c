
#define AMXOVL

#include "pawn_loader.h"
#include "pawn_cfg.h"
#include "amx.h"
#include "amxpool.h"

#include <stdio.h>

typedef struct SPawnLoader
{
    AMX amx;

    FILE * fp;
    unsigned char stack[ STACK_SZ ];
    unsigned char pool[ POOL_SZ ];
};

Pawn g_p;
FILE * g_fp = 0;

static int AMXAPI prun_Overlay( AMX * amx, int index )
{
    AMX_HEADER * hdr;
    AMX_OVERLAYINFO * tbl;

    hdr = (AMX_HEADER*)amx->base;
    tbl = (AMX_OVERLAYINFO*)(amx->base + hdr->overlays) + index;
    amx->codesize = tbl->size;
    amx->code     = amx_poolfind( index );
    if ( amx->code == 0 )
    {
        amx->code = amx_poolalloc( tbl->size, index );
        fseek( g_p.fp, (int)hdr->cod + tbl->offset, SEEK_SET );
        fread( amx->code, 1, tbl->size, g_p.fp );
    }
    return AMX_ERR_NONE;
}

Pawn * pawn( void )
{
    return &g_p;
}

AMX  * pawnAmx( void )
{
    return &g_p.amx;
}

int pawnLoad( Pawn * p, const char * fileName )
{
  /*FILE *fp;
  AMX_HEADER hdr;
  int result, i;
  int32_t size;
  unsigned char * datablock;
  #define OVLPOOLSIZE 1024

  // open the file, read and check the header
  if ((fp = fopen(fileName, "rb")) == NULL)
    return AMX_ERR_NOTFOUND;
  p->fp = fp;

  fread(&hdr, sizeof hdr, 1, fp);
  amx_Align16(&hdr.magic);
  amx_Align16((uint16_t *)&hdr.flags);
  amx_Align32((uint32_t *)&hdr.size);
  amx_Align32((uint32_t *)&hdr.cod);
  amx_Align32((uint32_t *)&hdr.dat);
  amx_Align32((uint32_t *)&hdr.hea);
  amx_Align32((uint32_t *)&hdr.stp);
  if (hdr.magic != AMX_MAGIC) {
    fclose(fp);
    return AMX_ERR_FORMAT;
  } // if

  if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
    // allocate the block for the data + stack/heap, plus the complete file
    // header, plus the overlay pool
    
    #if defined AMXOVL
      size = (hdr.stp - hdr.dat) + hdr.cod + OVLPOOLSIZE;
    #else
      return AMX_ERR_OVERLAY;
    #endif
  } else {
    size = hdr.stp;
  } // if
  if ((datablock = malloc(size)) == NULL) {
    fclose(fp);
    return AMX_ERR_MEMORY;
  } // if

  // save the filename, for optionally reading the debug information (we could
  // also have read it here immediately); for reading overlays, we also need
  // the filename (and in this case, note that amx_Init() already browses
  // through all overlays)
  
  // read in the file, in two parts; first the header and then the data section
  rewind(fp);
  if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
    #if defined AMXOVL
      // read the entire header
      fread(datablock, 1, hdr.cod, fp);
      // read the data section, put it behind the header in the block
      fseek(fp, hdr.dat, SEEK_SET);
      fread(datablock + hdr.cod, 1, hdr.hea - hdr.dat, fp);
      // initialize the overlay pool
      amx_poolinit(datablock + (hdr.stp - hdr.dat) + hdr.cod, OVLPOOLSIZE);
    #endif
  } else {
    fread(datablock, 1, (size_t)hdr.size, fp);
  } // if
  //fclose(fp);

  // initialize the abstract machine
  for ( i=0; i<sizeof(p->amx); i++ )
      ((unsigned char *)&p->amx)[i] = 0;
  #if defined AMXOVL
    if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
      p->amx.data = datablock + hdr.cod;
      p->amx.overlay = prun_Overlay;
    } // if
  #endif
  result = amx_Init( &p->amx, datablock);

  // free the memory block on error, if it was allocated here
  if (result != AMX_ERR_NONE) {
    free(datablock);
    p->amx.base = NULL;                   // avoid a double free
  } // if

  return result;*/

    AMX_HEADER hdr;
    int i;
    if ( !(p->fp = fopen( fileName, "rb" ) ) )
        return PAWN_ERR_FILE_OPEN;
    g_fp = p->fp;
    fread( &hdr, sizeof(hdr), 1, p->fp );
    amx_Align16( &hdr.magic );
    amx_Align16((uint16_t *)&hdr.flags);
    amx_Align32((uint32_t *)&hdr.size);
    amx_Align32((uint32_t *)&hdr.cod);
    amx_Align32((uint32_t *)&hdr.dat);
    amx_Align32((uint32_t *)&hdr.hea);
    amx_Align32((uint32_t *)&hdr.stp);
    if ( hdr.magic != AMX_MAGIC )
    {
        fclose( p->fp );
        return PAWN_ERR_FORMAT;
    }

    rewind( p->fp );
    fread( p->stack, 1, hdr.cod, p->fp );
    fseek( p->fp, hdr.dat, SEEK_SET );
    fread( p->stack + hdr.cod, 1, hdr.hea - hdr.dat, p->fp );

    amx_poolinit( p->pool, POOL_SZ );

    for ( i=0; i<sizeof(p->amx); i++ )
        ((unsigned char *)&p->amx)[i] = 0;
    p->amx.data    = p->stack + hdr.cod;
    p->amx.overlay = prun_Overlay;
    i = amx_Init( &p->amx, p->stack );
    return ( i == AMX_ERR_NONE ) ? PAWN_OK : PAWN_ERR_INIT;
}

/*int AMXAPI aux_LoadProgram(AMX *amx, char *filename)
{
  FILE *fp;
  AMX_HEADER hdr;
  int result;
  int32_t size;
  unsigned char *datablock;
  #define OVLPOOLSIZE 4096

  // open the file, read and check the header
  if ((fp = fopen(filename, "rb")) == NULL)
    return AMX_ERR_NOTFOUND;
  fread(&hdr, sizeof hdr, 1, fp);
  amx_Align16(&hdr.magic);
  amx_Align16((uint16_t *)&hdr.flags);
  amx_Align32((uint32_t *)&hdr.size);
  amx_Align32((uint32_t *)&hdr.cod);
  amx_Align32((uint32_t *)&hdr.dat);
  amx_Align32((uint32_t *)&hdr.hea);
  amx_Align32((uint32_t *)&hdr.stp);
  if (hdr.magic != AMX_MAGIC) {
    fclose(fp);
    return AMX_ERR_FORMAT;
  } // if

  if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
    // allocate the block for the data + stack/heap, plus the complete file
    // header, plus the overlay pool
    
    #if defined AMXOVL
      size = (hdr.stp - hdr.dat) + hdr.cod + OVLPOOLSIZE;
    #else
      return AMX_ERR_OVERLAY;
    #endif
  } else {
    size = hdr.stp;
  } // if
  if ((datablock = malloc(size)) == NULL) {
    fclose(fp);
    return AMX_ERR_MEMORY;
  } // if

  // save the filename, for optionally reading the debug information (we could
  // also have read it here immediately); for reading overlays, we also need
  // the filename (and in this case, note that amx_Init() already browses
  // through all overlays)
  
  // read in the file, in two parts; first the header and then the data section
  rewind(fp);
  if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
    #if defined AMXOVL
      // read the entire header
      fread(datablock, 1, hdr.cod, fp);
      // read the data section, put it behind the header in the block
      fseek(fp, hdr.dat, SEEK_SET);
      fread(datablock + hdr.cod, 1, hdr.hea - hdr.dat, fp);
      // initialize the overlay pool
      amx_poolinit(datablock + (hdr.stp - hdr.dat) + hdr.cod, OVLPOOLSIZE);
    #endif
  } else {
    fread(datablock, 1, (size_t)hdr.size, fp);
  } // if
  fclose(fp);

  // initialize the abstract machine
  memset(amx, 0, sizeof *amx);
  #if defined AMXOVL
    if ((hdr.flags & AMX_FLAG_OVERLAY) != 0) {
      amx->data = datablock + hdr.cod;
      amx->overlay = prun_Overlay;
    } // if
  #endif
  result = amx_Init(amx, datablock);

  // free the memory block on error, if it was allocated here
  if (result != AMX_ERR_NONE) {
    free(datablock);
    amx->base = NULL;                   // avoid a double free
  } // if

  return result;
}*/


int pawnExec( Pawn * p )
{
    int err;
    cell retVal;
    err = amx_Exec( &p->amx, &retVal, AMX_EXEC_MAIN );
    return (err == AMX_ERR_NONE) ? PAWN_OK : PAWN_ERR_EXEC;
}

