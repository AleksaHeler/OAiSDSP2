#pragma once
#include "MinHeapNode.h"

// A Min Heap: Collection of 
// min-heap (or Huffman tree) nodes 
struct MinHeap 
{
	// Current size of min heap 
	int_least32_t size;

	// capacity of min heap 
	int_least32_t capacity;

	// Array of minheap node pointers 
	struct MinHeapNode** array;
};
