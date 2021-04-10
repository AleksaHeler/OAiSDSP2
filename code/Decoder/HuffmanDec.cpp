#define _CRT_SECURE_NO_WARNINGS

// C program for Huffman Coding 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "HuffmanDec.h"
#include "BitReader.h"

#define MAX_TREE_HT 10000 

// A utility function to print an array of size n 
void printArr(uint8_t arr[], int n)
{
	int i;
	for (i = 0; i < n; ++i)
		printf("%d", arr[i]);

	printf("\n");
}

int isLeaf(struct HuffmanpNode* root)
{
	return !(root->left) && !(root->right);
}

struct HuffmanpNode* readDict(QFile& in, ushort dict_size, ushort* bytes_read)
{
	uint8_t byte;

	if (*bytes_read == dict_size)
		return NULL;
	else
	{
		in.getChar((char*)&byte);
		(*bytes_read)++;
	}

	struct HuffmanpNode* node = (struct HuffmanpNode*)malloc(sizeof(struct HuffmanpNode));
	if (node == NULL)
	{
		fputs("Memory error", stderr); return NULL;
	}

	node->left = node->right = NULL;


	if (byte == 2)
	{
		in.getChar(((char*)&node->data) + 1);
		in.getChar((char*)&node->data);
		(*bytes_read) += 2;
		return node;
	}
	else 
	{ 
	
		if (byte != 0)
		{
			printf("Error, expected '0' but %u is found! \n", byte);
		}
		
		node->left = readDict(in, dict_size, bytes_read);

		if (*bytes_read == dict_size)
		{
			return node;
		}
		else
		{
			in.getChar((char*)&byte);
			(*bytes_read)++;
		}

		if(byte != 1)
		{
			printf("Error, expected '1' but %u is found! \n", byte);
		}
		
		node->right = readDict(in, dict_size, bytes_read);

		return node;

	}	

	return NULL;
}


void calculateCodes(struct HuffmanpNode* root, uint8_t  arr[], int_least32_t  top, uint8_t** codes, int16_t* code_len)
{
	// Assign 0 to left edge and recur 
	if (root->left) {

		arr[top] = 0;
		calculateCodes(root->left, arr, top + 1, codes, code_len);
	}

	// Assign 1 to right edge and recur 
	if (root->right) {

		arr[top] = 1;
		calculateCodes(root->right, arr, top + 1, codes, code_len);
	}

	if (isLeaf(root)) {

		code_len[root->data + 32768] = top;
		codes[root->data + 32768] = (uint8_t*) malloc(top * sizeof(uint8_t));
		if (codes[root->data + 32768] == NULL)
		{
			fputs("Memory error", stderr); return;
		}


		for (int i = 0; i < top; i++)
		{
			codes[root->data + 32768][i] = arr[i];
		}
	}
}

void testDictrionary(struct HuffmanpNode* root)
{
	uint8_t* arr = (uint8_t*)malloc(MAX_TREE_HT * sizeof(uint8_t));
	if (arr == NULL)
	{
		fputs("Memory error", stderr); return;
	}

	uint8_t** codes = (uint8_t**)malloc(65536 * sizeof(uint8_t*));
	if (codes == NULL)
	{
		fputs("Memory error", stderr); return;
	}

	int16_t* code_len = (int16_t*)malloc(65536 * sizeof(int16_t));
	if (code_len == NULL)
	{
		fputs("Memory error", stderr); return;
	}

	printf("########################################\n");

	calculateCodes(root, arr, 0, codes, code_len);

	for (int i = 0; i < 65536; i++)
	
	if (code_len[i] > 0)
	{
		printf("%d: ", i-32768);
		printArr(codes[i], code_len[i]);
	}

	printf("########################################\n\n");

	free(arr);
	free(codes);
	free(code_len);
}

bool decodeHuffmanData(QFile& inputFile, std::vector<short>& v_Y, std::vector<short>& v_U, std::vector<short>& v_V)
{
	qCritical() << "HuffmanDecoding:";

	ushort dict_size;
	if (inputFile.getChar(((char*)&dict_size) + 1) == false)
		return false;
	if (inputFile.getChar((char*)&dict_size) == false)
		return false;
	qCritical() << "	Dictionary size:" << dict_size;

	qCritical() << "	Reading dictionary...";
	ushort bytes_read = 0;
	struct HuffmanpNode* root = readDict(inputFile, dict_size, &bytes_read);

	struct BitReader br = { inputFile, 0, 0 };

	uint8_t bit = readBit(&br);

	qCritical() << "	Decoding data...";
	std::vector<short> vReader;
	struct HuffmanpNode* curr = root;
	while (bit != 0xFF)
	{
		if (bit == 0)
			curr = curr->left;
		else
			curr = curr->right;

		// reached leaf node 
		if (isLeaf(curr))
		{
			vReader.push_back(curr->data);

			curr = root;
		}

		bit = readBit(&br);
	}

	qCritical() << "	Reading Y component...";
	int i = 0;
	v_Y.push_back(vReader[i]);
	v_Y.push_back(vReader[i + 1]);
	while (vReader[i] != 0 || vReader[i + 1] != 0) //eob 
	{
		i += 2;
		v_Y.push_back(vReader[i]);
		v_Y.push_back(vReader[i + 1]);
	}
	i += 2;

	qCritical() << "	Reading U component...";
	v_U.push_back(vReader[i]);
	v_U.push_back(vReader[i + 1]);
	while (vReader[i] != 0 || vReader[i + 1] != 0) //eob 
	{
		i += 2;
		v_U.push_back(vReader[i]);
		v_U.push_back(vReader[i + 1]);
	}
	i += 2;

	qCritical() << "	Reading V component...";
	v_V.push_back(vReader[i]);
	v_V.push_back(vReader[i + 1]);
	while (vReader[i] != 0 || vReader[i + 1] != 0) //eob 
	{
		i += 2;
		v_V.push_back(vReader[i]);
		v_V.push_back(vReader[i + 1]);
	}
	i += 2;

	return true;
}