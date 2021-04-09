#pragma once

#include <QDebug>
#include <QFile>

#include <stdio.h> 
#include <stdint.h>

struct BitWritter
{
	QFile* file;
	uint8_t buffer;
	int_fast8_t bit_idx;
};

void writeBit(struct BitWritter* bw, uint8_t bit);

void flushBitWritter(struct BitWritter* bw);



