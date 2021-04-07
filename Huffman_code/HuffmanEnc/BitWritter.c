#pragma once

#include "BitWritter.h"

void writeBit(struct BitWritter* bw, uint8_t bit)
{
	bw->buffer |= (bit << bw->bit_idx);
	bw->bit_idx++;
	if (bw->bit_idx == 8)
	{
		fwrite(&bw->buffer, 1, 1, bw->file);
		bw->bit_idx = 0;
		bw->buffer = 0;
	}
}

void flushBitWritter(struct BitWritter* bw)
{
	fwrite(&bw->buffer, 1, 1, bw->file);
	bw->bit_idx = 0;
	bw->buffer = 0;
}


