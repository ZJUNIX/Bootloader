#ifndef _IO_H_
#define _IO_H_

#include "inttypes.h"

uint8_t iportb(uint32_t port);
void oportb(uint32_t port, uint8_t data);

#endif // _IO_H_