
#include "io.h"
#include "inttypes.h"

uint8_t iportb(uint32_t port)
{
	uint8_t *addr = (uint8_t *)(0xb4000000 | port);
	return (uint8_t)(*addr);
}
uint16_t iportw(uint32_t port)
{
	uint16_t *addr = (uint16_t *)(0xb4000000 | port);
	return (uint16_t)(*addr);
}

void oportb(uint32_t port, uint8_t data)
{
	uint8_t *addr = (uint8_t *)(0xb4000000 | port);
	*addr = data;
}

void oportw(uint32_t port, uint16_t data)
{
	uint16_t *addr = (uint16_t *)(0xb4000000 | port);
	*addr = data;
}