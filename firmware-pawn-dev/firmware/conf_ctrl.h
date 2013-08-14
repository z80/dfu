
#ifndef __CONF_CTRL_H_
#define __CONF_CTRL_H_

#include "ch.h"

// These are low level config save utilities.
uint8_t saveConf( uint32_t * datad, int cnt );
uint32_t * loadConf( int cnt );


#endif




