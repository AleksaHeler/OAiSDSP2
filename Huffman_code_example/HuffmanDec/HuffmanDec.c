#define _CRT_SECURE_NO_WARNINGS

// C program for Huffman Coding 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "Huffman.h"
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

struct HuffmanpNode* readDict(FILE* in)
{
	uint8_t byte;

	if(fread(&byte, 1, 1, in) == 0)
	{
		return NULL;
	}

	struct HuffmanpNode* node = (struct HuffmanpNode*)malloc(sizeof(struct HuffmanpNode));
	if (node == NULL)
	{
		fputs("Memory error", stderr); return NULL;
	}

	node->left = node->right = NULL;


	if (byte == 2)
	{
		fread(&node->data, 2, 1, in);
		return node;
	}
	else 
	{ 
	
		if (byte != 0)
		{
			printf("Error, expected '0' but %u is found! \n", byte);
		}
		
		node->left = readDict(in);

		if (fread(&byte, 1, 1, in) == 0)
		{
			return node;
		}

		if(byte != 1)
		{
			printf("Error, expected '1' but %u is found! \n", byte);
		}
		
		node->right = readDict(in);

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


// Driver program to test above functions 
int main(int argc, char* argv[])
{
	FILE* input, * output, * dictionary;

	if (argc != 4)
	{
		fputs("Error, invalid number of arguments!", stderr);
		fputs("Expected format: HuffmanDec <input_file> <output_file> <dictionary>", stderr);
		return -1;
	}


	input = fopen(argv[1], "rb");
	if (input == NULL) { fputs("File error", stderr); return -1; }

	output = fopen(argv[2], "wb");
	if (output == NULL) { fputs("File error", stderr); return -1; }

	dictionary = fopen(argv[3], "rb");
	if (dictionary == NULL) { fputs("File error", stderr); return -1; }


	int header_size = 8;
	uint8_t* file_header = (uint8_t*)malloc(header_size); 
	if (file_header == NULL)
	{
		fputs("Memory error", stderr); return;
	}

	fread(file_header, 1, header_size, input);
	fwrite(file_header, 1, header_size, output);

	uint16_t data_sizeX = *(uint16_t*)(file_header - 4);
	uint16_t data_sizeY = *(uint16_t*)(file_header - 2);
	uint32_t data_size = data_sizeX * data_sizeY;

	free(file_header);

	struct HuffmanpNode* root = readDict(dictionary); 

	struct BitReader br = { input, 0, 0 };

	uint8_t bit = readBit(&br);

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
			fwrite(&curr->data, 2, 1, output);
			curr = root;
			data_size -= 2;
			if (data_size == 0)
				break;
		}

		bit = readBit(&br);
	}

	// terminate
	fclose(input);
	fclose(output);
	fclose(dictionary);

	return 0;

}
