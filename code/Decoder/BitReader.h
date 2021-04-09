#pragma once

#include <stdint.h>
#include <stdio.h>

#include <QDebug>
#include <QFile>

struct BitReader
{
	QFile& file;
	uint8_t buffer;
	int_fast8_t bit_idx;
};

uint8_t readBit(struct BitReader* br);