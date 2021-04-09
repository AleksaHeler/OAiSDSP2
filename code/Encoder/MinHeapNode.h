#pragma once
#include <stdint.h>

struct MinHeapNode {

	// One of the input characters 
	int16_t data;

	// Frequency of the character 
	int_least32_t freq;

	// Left and right child of this node 
	struct MinHeapNode* left, *right;

};
