#pragma once

#include <stdint.h>

struct HuffmanpNode
{
	int16_t data;
	struct HuffmanpNode* left, * right;
};
