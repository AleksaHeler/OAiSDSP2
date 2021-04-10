#pragma once

#include <QDebug>
#include <QFile>

#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "BitWritter.h"
#include "MinHeap.h"
#include "MinHeapNode.h"

#define MAX_TREE_HT 10000 

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


struct MinHeapNode* buildHuffmanTree(int16_t data[], int_least32_t  freq[], int_least32_t  size);

void calculateCodes(struct MinHeapNode* root, uint8_t  arr[], int_least32_t  top, uint8_t** codes, int16_t* code_len);

void writeDict(QFile& out, struct MinHeapNode* root);

void getDictSize(struct MinHeapNode* root, ushort* ret_val);

bool doHuffmanEncoding(QFile& outputFile, std::vector<short> &v_Y, std::vector<short> &v_U, std::vector<short> &v_V);