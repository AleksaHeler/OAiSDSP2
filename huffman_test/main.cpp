#include <iostream>
#include <cstdlib>
#include <map>
#include <queue>
#include <string>

#define MAX_TREE_HT 256
using namespace std;

map<short, string> codes_16;
map<short, short> freq_16;

struct MinHeapNode_16
{
	short data;
	short unsigned freq;
	MinHeapNode_16 *left, *right;

	MinHeapNode_16(short data, short freq)
	{
		left = right = NULL;
		this->data = data;
		this->freq = freq;
	}
};


struct compare
{
	bool operator()(MinHeapNode_16* l, MinHeapNode_16* r)
	{
		return (l->freq > r->freq);
	}
};

void storeCodes(struct MinHeapNode_16* root, string str)
{
	if (root == NULL)
		return;

	if (root->data != 0xFFFF)
		codes_16[root->data] = str;

	storeCodes(root->left, str + "0");
	storeCodes(root->right, str + "1");
}

// STL priority queue to store heap tree, with respect
priority_queue<MinHeapNode_16*, vector<MinHeapNode_16*>, compare> minHeap;

// function to build the Huffman tree and store it
// in minHeap
int dict_size = 0; //to see final compression
void HuffmanCodes(int size)
{
	struct MinHeapNode_16 *left, *right, *top;
	for (map<short, short>::iterator v = freq_16.begin(); v != freq_16.end(); v++) {
		minHeap.push(new MinHeapNode_16(v->first, v->second));
		dict_size += 4;
	}

	while (minHeap.size() != 1)
	{
		left = minHeap.top();
		minHeap.pop();
		right = minHeap.top();
		minHeap.pop();
		top = new MinHeapNode_16(0xFFFF, left->freq + right->freq);
		top->left = left;
		top->right = right;
		minHeap.push(top);
	}
	storeCodes(minHeap.top(), "");
}

void calcFreq(vector<short>& v, int n)
{
	for (int i = 0; i<n; i++)
		freq_16[v[i]]++;
}

// function iterates through the encoded string s
// if s[i]=='1' then move to node->right
// if s[i]=='0' then move to node->left
// if leaf node append the node->data to our output string
void decode_file(struct MinHeapNode_16* root, string s, vector<short>& v)
{
	string ans = "";
	struct MinHeapNode_16* curr = root;
	for (int i = 0; i<s.size(); i++)
	{
		if (s[i] == '0')
			curr = curr->left;
		else
			curr = curr->right;

		if (curr->left == NULL && curr->right == NULL)
		{
			v.push_back(curr->data);
			curr = root;
		}
	}
}

int main()
{	
	//napomena
	//nije radjena provera da li broj ponavljanja moze da stane u unsigned short
	//mozda ne treba da se salje ceo freq_16 nego samo data, tj mozda moze bez broja ponavljanja

	vector<short> arr = { 0, 0, 5, 13, 0, 0, 1, 0, 2, 0, 4, 5, 1111, 4, 616, 615, 0, 516, 0, 0, 0, 7};
	int s = arr.size();
	for (int i = 0; i < s; i++)
		arr.push_back(arr[i]);
	for (int i = 0; i < s; i++)
		arr.push_back(arr[i]);
	for (int i = 0; i < s; i++)
		arr.push_back(arr[i]);

	calcFreq(arr, arr.size());
	
	cout << "Frequencies: " << endl;
	for (auto v = freq_16.begin(); v != freq_16.end(); v++)
		cout << v->first << ' ' << v->second << endl;
	

	HuffmanCodes(arr.size());	

	string encodedString;
	for (auto i : arr)
		encodedString += codes_16[i];

	cout << "Encoded Huffman data:\n" << encodedString << endl;
		
	vector<short> v;
	decode_file(minHeap.top(), encodedString, v);
	cout << "\nDecoded Huffman Data:" << endl;
	for (auto it = v.begin(); it != v.end() - 1; it++)
	{
		cout << *it << ", ";
	}
	cout << *(v.end() - 1) << endl;

	cout << "\nBefore: " << v.size() * 2 << endl;
	cout << "After (without dictionary): " << encodedString.length() / 8 << endl;
	cout << "After (with dictionary): " << (encodedString.length() / 8) + dict_size << endl;


	short len = encodedString.length() / 8;
	short remainder = encodedString.length() - len*8;

	cout << endl;
	cout << "len: " << len << endl;
	cout << "remainder: " << remainder << endl;

	char* toBeWritten = new char[len];
	for (int i = 0; i < len; i++)
		toBeWritten[i] = 0;

	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < 8; j++)
			toBeWritten[i] |= (encodedString[i * 8 + j] == '1' ? 1 : 0) << (7 - j);
	}

	char remainder_buffer = 0;

	for (int i = 0; i < remainder; i++) {
		remainder_buffer |= (encodedString[len * 8 + i] == '1' ? 1 : 0) << (7 - i);
	}

	string decodedString;
	for (int i = 0; i < len; i++)
	{
		for (int j = 7; j >= 0; j--)
			decodedString += (toBeWritten[i] & (1 << j)) ? "1" : "0";
	}

	for (int i = 0; i < remainder; i++)
		decodedString += (remainder_buffer & (0x80 >> i)) ? "1" : "0";
	
	cout << endl;
	cout << "Encoded string:" << endl;
	cout << encodedString << endl;

	cout << "Decoded string:" << endl;
	cout << decodedString << endl;

	cout << endl;
	if (encodedString == decodedString)
		cout << "Successful compression" << endl;
	else
		cout << "Error in compression" << endl;

	v.clear();
	decode_file(minHeap.top(), encodedString, v);
	cout << "\nDecoded Huffman Data:" << endl;
	for (auto it = v.begin(); it != v.end() - 1; it++)
	{
		cout << *it << ", ";
	}
	cout << *(v.end() - 1) << endl;

	delete[] toBeWritten;

	return 0;
}