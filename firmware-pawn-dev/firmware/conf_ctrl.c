
#include "conf_ctrl.h"
#include "hal.h"

#include "stm32f10x_flash.h"

#define CONF_PAGE_INDEX  20
#define CONF_PAGE_SIZE   1024
#define CONF_FLASH_START 0x08000000

static uint8_t crc8( uint8_t * data, int cnt );

uint8_t saveConf( uint32_t * datad, int cnt )
{
    uint32_t * flash = ( uint32_t * )( CONF_FLASH_START + CONF_PAGE_INDEX * CONF_PAGE_SIZE );
    // Comparing byte-by-byte.
    int i;
    uint8_t theSame = 1;
    for ( i=0; i<cnt; i++ )
    {
        if ( flash[i] != datad[i] )
        {
            theSame = 0;
            break;
        }
    }
    // If data is the same just return.
    if ( theSame )
        return 0;
    // If different write data.
    FLASH_Unlock();
    uint32_t flashD = CONF_FLASH_START + CONF_PAGE_INDEX * CONF_PAGE_SIZE;
    FLASH_Status st = FLASH_ErasePage( flashD );
    if ( st != FLASH_COMPLETE )
    {
        FLASH_Lock();
        return 1;
    }
    for ( i=0; i<cnt; i++ )
    {
        st = FLASH_ProgramWord( flashD+4*i, datad[i] );
        if ( st != FLASH_COMPLETE )
        {
            FLASH_Lock();
            return 2;
        }
    }
    uint32_t crc = (uint32_t)crc8( (uint8_t *)datad, 4*cnt );
    st = FLASH_ProgramWord( flashD+4*i, crc );
    FLASH_Lock();
    if ( st != FLASH_COMPLETE )
        return 3;
    return 0;
}

uint32_t * loadConf( int cnt )
{
    uint32_t * flash = ( uint32_t * )( CONF_FLASH_START + CONF_PAGE_INDEX * CONF_PAGE_SIZE );
    uint8_t dataCrc = crc8( (uint8_t *)flash, cnt * 4 );
    uint8_t readCrc = (uint8_t)flash[cnt];
    if ( dataCrc != readCrc )
        return 0;
    return flash;
}

static uint8_t crc8( uint8_t * data, int cnt )
{
    const uint8_t poly = 0x8C;
    int i, j;
    uint8_t val = 0;
    for ( i=0; i<cnt; i++ )
    {
        val ^= data[i];
        for ( j=0; j<8; j++ )
        {
            if ( val & 0x01 )
                val = ( val >> 1 ) ^ poly;
            else
                val = ( val >> 1 );
        }
    }
    return val;
}






