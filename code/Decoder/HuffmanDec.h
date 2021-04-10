#pragma once

// C program for Huffman Coding 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "HuffmanDec.h"
#include "BitReader.h"

#include <QDebug>
#include <QFile>

#define _CRT_SECURE_NO_WARNINGS
#define MAX_TREE_HT 10000 

struct HuffmanpNode
{
	int16_t data;
	struct HuffmanpNode* left, * right;
};

void printArr(uint8_t arr[], int n);

int isLeaf(struct HuffmanpNode* root);

struct HuffmanpNode* readDict(QFile& in, ushort dict_size, ushort* bytes_read);

void calculateCodes(struct HuffmanpNode* root, uint8_t  arr[], int_least32_t  top, uint8_t** codes, int16_t* code_len);

void testDictrionary(struct HuffmanpNode* root);

bool decodeHuffmanData(QFile& inputFile, std::vector<short>& v_Y, std::vector<short>& v_U, std::vector<short>& v_V);