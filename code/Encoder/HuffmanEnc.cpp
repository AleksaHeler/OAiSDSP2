#include "HuffmanEnc.h"
#include <stdlib.h>
#include <stdio.h>


// A utility function allocate a new 
// min heap node with given character 
// and frequency of the character 
struct MinHeapNode* newNode(int16_t data, int_least32_t freq)
{
	struct MinHeapNode* temp = (struct MinHeapNode*)malloc
	(sizeof(struct MinHeapNode));

	if (temp == NULL)
	{
		fputs("Memory error", stderr); return NULL;
	}

	temp->left = temp->right = NULL;
	temp->data = data;
	temp->freq = freq;

	return temp;
}

// A utility function to create 
// a min heap of given capacity 
struct MinHeap* createMinHeap(int_least32_t  capacity)
{

	struct MinHeap* minHeap
		= (struct MinHeap*)malloc(sizeof(struct MinHeap));

	if (minHeap == NULL)
	{
		fputs("Memory error", stderr); return NULL;
	}

	// current size is 0 
	minHeap->size = 0;

	minHeap->capacity = capacity;

	minHeap->array
		= (struct MinHeapNode**)malloc(minHeap->
			capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

// A utility function to 
// swap two min heap nodes 
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

// The standard minHeapify function. 
void minHeapify(struct MinHeap* minHeap, int_least32_t  idx)
{
	int_least32_t  smallest = idx;
	int_least32_t  left = 2 * idx + 1;
	int_least32_t  right = 2 * idx + 2;

	if (left < minHeap->size && minHeap->array[left]->
		freq < minHeap->array[smallest]->freq)
		smallest = left;

	if (right < minHeap->size && minHeap->array[right]->
		freq < minHeap->array[smallest]->freq)
		smallest = right;

	if (smallest != idx) {
		swapMinHeapNode(&minHeap->array[smallest],
			&minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}

// A utility function to check 
// if size of heap is 1 or not 
int isSizeOne(struct MinHeap* minHeap)
{
	return (minHeap->size == 1);
}

// A standard function to extract 
// minimum value node from heap 
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	struct MinHeapNode* temp = minHeap->array[0];
	minHeap->array[0]
		= minHeap->array[minHeap->size - 1];

	--minHeap->size;
	minHeapify(minHeap, 0);

	return temp;
}

// A utility function to insert 
// a new node to Min Heap 
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
	++minHeap->size;
	int_least32_t  i = minHeap->size - 1;

	while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {

		minHeap->array[i] = minHeap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}

	minHeap->array[i] = minHeapNode;
}

// A standard function to build min heap 
void buildMinHeap(struct MinHeap* minHeap)
{
	int_least32_t  n = minHeap->size - 1;
	int_least32_t  i;

	for (i = (n - 1) / 2; i >= 0; --i)
		minHeapify(minHeap, i);
}

// A utility function to print an array of size n 
void printArr(uint8_t arr[], int n)
{
	int i;
	for (i = 0; i < n; ++i)
		printf("%d", arr[i]);

	printf("\n");
}

// Utility function to check if this node is leaf 
int isLeaf(struct MinHeapNode* root)
{
	return !(root->left) && !(root->right);
}

// Creates a min heap of capacity 
// equal to size and inserts all character of 
// data[] in min heap. Initially size of 
// min heap is equal to capacity 
struct MinHeap* createAndBuildMinHeap(int16_t data[], int_least32_t  freq[], int_least32_t  size)
{
	struct MinHeap* minHeap = createMinHeap(size);

	for (int_least32_t i = 0; i < size; ++i)
		minHeap->array[i] = newNode(data[i], freq[i]);

	minHeap->size = size;
	buildMinHeap(minHeap);

	return minHeap;
}


// The main function that builds Huffman tree 
struct MinHeapNode* buildHuffmanTree(int16_t data[], int_least32_t  freq[], int_least32_t  size)
{
	struct MinHeapNode* left, *right, *top;

	// Step 1: Create a min heap of capacity 
	// equal to size. Initially, there are 
	// modes equal to size. 
	struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

	// Iterate while size of heap doesn't become 1 
	while (!isSizeOne(minHeap)) 
	{
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
	if (root->left) 
	{
		arr[top] = 0;
		calculateCodes(root->left, arr, top + 1, codes, code_len);
	}

	// Assign 1 to right edge and recur 
	if (root->right) 
	{
		arr[top] = 1;
		calculateCodes(root->right, arr, top + 1, codes, code_len);
	}

	// If this is a leaf node, then 
	// it contains one of the input 
	// characters, print the character 
	// and its code from arr[] 
	if (isLeaf(root)) 
	{
		code_len[root->data + 32768] = top;
		codes[root->data + 32768] = (uint8_t*)malloc(top * sizeof(uint8_t));
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


void writeDict(QFile& out, struct MinHeapNode* root)
{
	const uint8_t left = 0;
	const uint8_t right = 1;
	const uint8_t data = 2;

	// Assign 0 to left edge and recur 
	if (root->left) 
	{
		out.putChar(left);
		writeDict(out, root->left);
	}

	// Assign 1 to right edge and recur 
	if (root->right) 
	{
		out.putChar(right);
		writeDict(out, root->right);
	}

	if (isLeaf(root)) 
	{
		out.putChar(data);
		out.putChar(*(((uchar*)&root->data) + 1));
		out.putChar(*((uchar*)&root->data));
	}
}

void getDictSize(struct MinHeapNode* root, ushort* ret_val) 
{
	const uint8_t left = 0;
	const uint8_t right = 1;
	const uint8_t data = 2;

	if (root->left) {
		(*ret_val)++;
		getDictSize(root->left, ret_val);
	}

	if (root->right) {
		(*ret_val)++;
		getDictSize(root->right, ret_val);
	}

	if (isLeaf(root)) 
		(*ret_val) += 3;
}

bool doHuffmanEncoding(QFile& outputFile, std::vector<short> &v_Y, std::vector<short> &v_U, std::vector<short> &v_V)
{
	//qCritical() << "HuffmanEncoding:";

	int_least32_t* histogram = (int_least32_t*)malloc(sizeof(int_least32_t) * 65536);
	if (histogram == NULL)
	{
		qCritical() << "Memory error" << endl;
		return false;
	}

	for (int i = 0; i < 65536; i++)
		histogram[i] = 0;

	for (auto it = v_Y.begin(); it != v_Y.end(); it++)
	{
		histogram[*it + 32768]++;
	}

	for (auto it = v_U.begin(); it != v_U.end(); it++)
	{
		histogram[*it + 32768]++;
	}

	for (auto it = v_V.begin(); it != v_V.end(); it++)
	{
		histogram[*it + 32768]++;
	}


	int non_zero_cnt = 0;
	for (int i = 0; i < 65536; i++)
	{
		if (histogram[i] != 0)
			non_zero_cnt++;
	}

	int16_t* data = (int16_t*)malloc(non_zero_cnt * sizeof(int16_t));
	int_least32_t* freqz = (int_least32_t*)malloc(non_zero_cnt * sizeof(int_least32_t));

	//qCritical() << "	Initializing data and frequencies...";

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

	delete[] histogram;

	// Construct Huffman Tree 
	//qCritical() << "	Building Huffman tree...";
	struct MinHeapNode* root = buildHuffmanTree(data, freqz, non_zero_cnt);


	// Print Huffman codes using 
	// the Huffman tree built above 
	uint8_t* arr = (uint8_t*)malloc(MAX_TREE_HT * sizeof(uint8_t));
	if (arr == NULL)
	{
		qCritical() << "Memory error" << endl;
		return false;
	}

	uint8_t** codes = (uint8_t**)malloc(65536 * sizeof(uint8_t*));
	if (codes == NULL)
	{
		qCritical() << "Memory error" << endl;
		return false;
	}

	int16_t* code_len = (int16_t*)malloc(65536 * sizeof(int16_t));
	if (code_len == NULL)
	{
		qCritical() << "Memory error" << endl;
		return false;
	}

	//qCritical() << "	Calculating codes...";
	calculateCodes(root, arr, 0, codes, code_len);

	//qCritical() << "	Writing dictionary size...";
	ushort dict_size = 0;
	getDictSize(root, &dict_size);
	if (outputFile.putChar(*(((uchar*)&dict_size) + 1)) == false)
		return false;

	if (outputFile.putChar(*((uchar*)&dict_size)) == false)
		return false;

	//qCritical() << "	Writing dictionary...";
	writeDict(outputFile, root);

	//qCritical() << "	Writing data in encoded format...";
	//Y writing
	struct BitWritter bw = { &outputFile, 0, 0 };
	for (auto it = v_Y.begin(); it != v_Y.end(); it++)
	{
		for (int i = 0; i < code_len[*it + 32768]; i++)
			writeBit(&bw, codes[*it + 32768][i]);
	}
	//U writing
	for (auto it = v_U.begin(); it != v_U.end(); it++)
	{
		for (int i = 0; i < code_len[*it + 32768]; i++)
			writeBit(&bw, codes[*it + 32768][i]);
	}
	//V writing
	for (auto it = v_V.begin(); it != v_V.end(); it++)
	{
		for (int i = 0; i < code_len[*it + 32768]; i++)
			writeBit(&bw, codes[*it + 32768][i]);
	}

	flushBitWritter(&bw);

	delete[] data;
	delete[] freqz;

	return true;
}