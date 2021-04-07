#define _CRT_SECURE_NO_WARNINGS

// C program for Huffman Coding 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "BitWritter.h"
#include "MinHeap.h"


// This constant can be avoided by explicitly 
// calculating height of Huffman Tree 
#define MAX_TREE_HT 10000 


// The main function that builds Huffman tree 
struct MinHeapNode* buildHuffmanTree(int16_t data[], int_least32_t  freq[], int_least32_t  size)

{
	struct MinHeapNode* left, * right, * top;

	// Step 1: Create a min heap of capacity 
	// equal to size. Initially, there are 
	// modes equal to size. 
	struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

	// Iterate while size of heap doesn't become 1 
	while (!isSizeOne(minHeap)) {

		// Step 2: Extract the two minimum 
		// freq items from min heap 
		left = extractMin(minHeap);
		right = extractMin(minHeap);

		// Step 3: Create a new internal 
		// node with frequency equal to the 
		// sum of the two nodes frequencies. 
		// Make the two extracted node as 
		// left and right children of this new node. 
		// Add this node to the min heap 
		// '$' is a special value for internal nodes, not used 
		top = newNode(-1, left->freq + right->freq);

		top->left = left;
		top->right = right;

		insertMinHeap(minHeap, top);
	}

	// Step 4: The remaining node is the 
	// root node and the tree is complete. 
	return extractMin(minHeap);
}

void calculateCodes(struct MinHeapNode* root, uint8_t  arr[], int_least32_t  top, uint8_t** codes, int16_t* code_len)

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

	// If this is a leaf node, then 
	// it contains one of the input 
	// characters, print the character 
	// and its code from arr[] 
	if (isLeaf(root)) {

		code_len[root->data + 32768] = top;
		codes[root->data + 32768] = malloc(top * sizeof(uint8_t));
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

void writeDict(FILE* out, struct MinHeapNode* root)
{
	const uint8_t left = 0;
	const uint8_t right = 1;
	const uint8_t data = 2;

	// Assign 0 to left edge and recur 
	if (root->left) {

		fwrite(&left, 1, 1, out);
		writeDict(out, root->left);
	}

	// Assign 1 to right edge and recur 
	if (root->right) {

		fwrite(&right, 1, 1, out);
		writeDict(out, root->right);
	}

	if (isLeaf(root)) {
		fwrite(&data, 1, 1, out);
		fwrite(&root->data, 2, 1, out);
	}
}


// Driver program to test above functions 
int main(int argc, char* argv[])
{
	FILE *input, *output, *dictionary;

	if (argc != 4)
	{
		fputs("Error, invalid number of arguments!", stderr);
		fputs("Expected format: HuffmanEnc <input_file> <output_file> <dictionary>", stderr);
		return -1;
	}
		
	
	int16_t in_sample;

	input = fopen(argv[1], "rb");
	if (input == NULL) { fputs("File error", stderr); return -1; }
	
	output = fopen(argv[2], "wb");
	if (output == NULL) { fputs("File error", stderr); return -1; }
	
	int header_size = 8;
	uint8_t* file_header = (uint8_t*)malloc(header_size); 
	if (file_header == NULL)
	{
		fputs("Memory error", stderr); return;
	}

	fread(file_header, 1, header_size, input);
	fwrite(file_header, 1, header_size, output);

	free(file_header);


	int_least32_t* histogram = malloc(sizeof(int_least32_t) * 65536);
	if (histogram == NULL)
	{
		fputs("Memory error", stderr); return;
	}


	for (int i = 0; i < 65536; i++)
		histogram[i] = 0;
	

	while (fread(&in_sample, sizeof(int16_t), 1, input) == 1)
	{
		histogram[in_sample + 32768]++;
	}

	fseek(input, header_size, SEEK_SET);

	int non_zero_cnt = 0;
	for (int i = 0; i < 65536; i++)
	{
		if(histogram[i] != 0)
			non_zero_cnt++;
	}

	int16_t* data = malloc(non_zero_cnt * sizeof(int16_t));
	int_least32_t* freqz = malloc(non_zero_cnt * sizeof(int_least32_t));

	int j = 0;
	for (int i = 0; i < 65536; i++)
	{
		if (histogram[i] != 0)
		{
			data[j] = (int16_t)(i - 32768);
			freqz[j] = histogram[i];
			j++;
		}
	}

	free(histogram);

	// Construct Huffman Tree 
	struct MinHeapNode* root
		= buildHuffmanTree(data, freqz, non_zero_cnt);

	// Print Huffman codes using 
	// the Huffman tree built above 
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

	calculateCodes(root, arr, 0, codes, code_len);

	dictionary = fopen(argv[3], "wb");
	if (dictionary == NULL) { fputs("File error", stderr); return -1; }

	//printf("########################################\n");
	//printf("Huffman Dictionary:\n");
	//for (int i = 0; i < non_zero_cnt; i++)
	//{
	//	printf("%d: ", data[i]);
	//	printArr(codes[data[i] + 32768], code_len[data[i] + 32768]);
	//}
	//printf("########################################\n");

	writeDict(dictionary, root);
	
	struct BitWritter bw = { output, 0, 0 };
	while (fread(&in_sample, sizeof(int16_t), 1, input) == 1)
	{
		for (int i = 0; i < code_len[in_sample + 32768]; i++)
			writeBit(&bw, codes[in_sample + 32768][i]);
	}
	
	flushBitWritter(&bw);

	free(data);
	free(freqz);


	// terminate
	fclose(input);
	fclose(output);
	fclose(dictionary);

	return 0;

}
