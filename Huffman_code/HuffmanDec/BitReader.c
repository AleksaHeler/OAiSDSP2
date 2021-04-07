#include "BitReader.h"


uint8_t readBit(struct BitReader* br)
{
	if (br->bit_idx == 0)
	{
		if (fread(&br->buffer, 1, 1, br->file) == 0)
		{
			return 0xFF;
		}
	}

	uint8_t ret_val = (br->buffer & (1 << br->bit_idx)) != 0;

	br->bit_idx++;
	br->bit_idx = br->bit_idx % 8;

	return ret_val;
}
