#pragma once

#include <stdint.h>

struct MinHeapNode {

	// One of the input characters 
	int16_t data;

	// Frequency of the character 
	int_least32_t freq;

	// Left and right child of this node 
	struct MinHeapNode* left, * right;
};

// A Min Heap: Collection of 
// min-heap (or Huffman tree) nodes 
struct MinHeap {

	// Current size of min heap 
	int_least32_t size;

	// capacity of min heap 
	int_least32_t capacity;

	// Array of minheap node pointers 
	struct MinHeapNode** array;
};


struct MinHeapNode* newNode(int16_t data, int_least32_t freq);

struct MinHeap* createMinHeap(int_least32_t  capacity);

void swapMinHeapNode(struct MinHeapNode** a,
	struct MinHeapNode** b);

void minHeapify(struct MinHeap* minHeap, int_least32_t  idx);

int isSizeOne(struct MinHeap* minHeap);

struct MinHeapNode* extractMin(struct MinHeap* minHeap);

void insertMinHeap(struct MinHeap* minHeap,
	struct MinHeapNode* minHeapNode);

void buildMinHeap(struct MinHeap* minHeap);

void printArr(uint8_t arr[], int n);

int isLeaf(struct MinHeapNode* root);

struct MinHeap* createAndBuildMinHeap(int16_t data[], int_least32_t  freq[], int_least32_t  size);