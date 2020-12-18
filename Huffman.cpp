#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
struct HuffManNode
{
	char data;
	int freq;
	int totalcode;
	HuffManNode *left, *right;
};
struct node
{
	char data;
	int freq;
	float probability;
	char *code;
	node *next;
};
void swap(int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}
void swap(char *a, char *b)
{
	char t = *a;
	*a = *b;
	*b = t;
}
void insert_sort(node *list)
{
	node *p = list;
	node *q = p;
	p = p->next;
	while (p != NULL)
	{
		q = list;
		while (q->next != NULL)
		{
			if (q->freq > p->freq)
			{
				swap(&q->freq, &p->freq);
				swap(&q->data, &p->data);
			}
			else
				q = q->next;
		}
		p = p->next;
	}
}
void insertNode(node **list, char data)
{
	bool flag = false;
	if (*list == NULL)
	{
		node *new_node = new node;
		new_node->data = data;
		new_node->freq = 1;
		new_node->next = NULL;
		*list = new_node;
		return;
	}
	node *p = *list;
	while (p)
	{
		if (p->data == data)
		{
			(p->freq)++;
			flag = true;
		}
		if (p->next == NULL)
			break;
		p = p->next;
	}
	if (!flag)
	{
		node *new_node = new node;
		new_node->data = data;
		new_node->freq = 1;
		new_node->next = NULL;
		p->next = new_node;
		return;
	}
}
int list_length(node *list)
{
	node *p = list;
	int count = 0;
	while (p != NULL)
	{
		count++;
		p = p->next;
	}
	return count;
}
HuffManNode *createHuffManNode(char data, int freq)
{
	HuffManNode *new_node = new HuffManNode;
	new_node->data = data;
	new_node->freq = freq;
	new_node->right = NULL;
	new_node->left = NULL;
	return new_node;
}
HuffManNode *createHuffManTree(node *list)
{
	HuffManNode *p = new HuffManNode;
	node *li = list;
	p->freq = li->freq;
	p->left = createHuffManNode(li->data, li->freq);
	li = li->next;
	p->right = createHuffManNode(li->data, li->freq);
	p->freq += li->freq;
	p->data = NULL;
	li = li->next;
	while (li)
	{
		HuffManNode *parent = new HuffManNode;
		if (li->freq < p->freq)
		{
			parent->left = createHuffManNode(li->data, li->freq);
			parent->right = p;
		}
		else
		{
			parent->right = createHuffManNode(li->data, li->freq);
			parent->left = p;
		}
		parent->freq = li->freq + p->freq;
		parent->data = NULL;
		p = parent;
		li = li->next;
	}
	p->totalcode = 0;
	return p;
}
void inorder(HuffManNode *root)
{
	if (root != NULL)
	{
		inorder(root->left);
		printf("%d\n", root->freq);
		inorder(root->right);
	}
}
void getCodes(HuffManNode *root, node *list)
{
	HuffManNode *t = root;
	node *p = list;
	int i = 0;
	while (p)
	{
		while (t)
		{
			if (t->data == p->data)
			{
				p->code[i] = '\0';
				root->totalcode += strlen(p->code) * p->freq;
				break;
			}
			if (t->left->data)
			{
				if (t->left->data == p->data)
				{
					p->code[i] = '0';
					i++;
					p->code[i] = '\0';
					root->totalcode += strlen(p->code) * p->freq;
					break;
				}
				p->code[i] = '1';
				i++;
				t = t->right;
			}
			else if (t->right->data)
			{
				if (t->right->data == p->data)
				{
					p->code[i] = '1';
					i++;
					p->code[i] = '\0';
					root->totalcode += strlen(p->code) * p->freq;
					break;
				}
				p->code[i] = '0';
				i++;
				t = t->left;
			}
		}
		i = 0;
		t = root;
		p = p->next;
	}
}
uint8_t stringToByte(const char *s)
{
	uint8_t buffer = 0;
	int count = 0;
	while (count < 8)
	{
		for (int i = 0; i < 8; i++)
		{
			if (s[i] == '1')
			{
				buffer = buffer << 1;
				buffer |= 1;
			}
			else
				buffer = buffer << 1;
			count++;
		}
	}
	return buffer;
}
void convertBaseToString(int input, char *output)
{
	int i, rem;
	int base = 2;
	int digits = 8;
	char digitsArray[3] = "01";
	for (i = digits; i > 0; i--)
	{
		rem = input % base;
		input = input / base;
		output[i - 1] = digitsArray[rem];
	}
	output[digits] = '\0';
}
void compress(const char *read, const char *write, node *list)
{
	FILE *read_file = fopen(read, "r");
	FILE *write_file = fopen(write, "wb");
	char c;
	char *binary = new char[9];
	binary[8] = '\0';
	int count = 0;
	uint8_t buffer;
	node *p = list;
	if (read_file)
	{
		while ((c = getc(read_file)) != EOF)
		{
			p = list;
			while (p)
			{
				if (p->data == c)
					break;
				p = p->next;
			}
			for (int i = 0; i < strlen(p->code); i++)
			{
				binary[count++] = p->code[i];
				if (count == 8)
				{
					buffer = stringToByte(binary);
					fwrite(&buffer, sizeof(buffer), 1, write_file);
					count = 0;
				}
			}
		}
		if (count < 8)
		{
			for (int i = count; i < 8; i++)
			{
				binary[i] = '0';
			}
			buffer = stringToByte(binary);
			fwrite(&buffer, sizeof(buffer), 1, write_file);
		}
		fclose(read_file);
	}
	fclose(write_file);
}
void compressTxt(const char *read, const char *write, node *list)
{
	FILE *read_file = fopen(read, "r");
	FILE *write_file = fopen(write, "w");
	char c;
	node *p = list;
	if (read_file)
	{
		while ((c = getc(read_file)) != EOF)
		{
			p = list;
			while (p)
			{
				if (p->data == c)
					break;
				p = p->next;
			}
			fputs(p->code, write_file);
		}
		fclose(read_file);
	}
	fclose(write_file);
}
void decompress(const char *read, const char *write, HuffManNode *root)
{
	FILE *read_file = fopen(read, "rb");
	FILE *write_file = fopen(write, "w");
	uint8_t buffer;
	HuffManNode *t = root;
	int count = 0;
	char *c = new char[9];
	if (read_file)
	{
		while (1 == fread(&buffer, sizeof(buffer), 1, read_file))
		{
			convertBaseToString(buffer, c);
			for (int i = 0; i < 8; i++)
			{
				if (c[i] == '1')
					t = t->right;
				else if (c[i] == '0')
					t = t->left;
				if ((!t->left) && (!t->right))
				{
					fprintf(write_file, "%c", t->data);
					t = root;
				}
				count++;
				if (count == root->totalcode)
				{
					break;
				}
			}
		}
		fclose(read_file);
	}
	fclose(write_file);
}
int file_size(const char *read)
{
	int size = 0;
	FILE *read_file = fopen(read, "r");
	char c;
	if (read_file)
	{
		while ((c = getc(read_file)) != EOF)
			size += 8;
	}
	fclose(read_file);
	return size;
}
int binary_file_size(const char *read)
{
	int size = 0;
	FILE *read_file = fopen(read, "r");
	char c;
	if (read_file)
	{
		while ((c = getc(read_file)) != EOF)
			size += 1;
	}
	fclose(read_file);
	return size;
}
float get_probability_ratio(node *list, int total)
{
	float avgCodeLen = 0;
	float ratio;
	node *p = list;
	while (p)
	{
		p->probability = p->freq / (total * 1.0);
		avgCodeLen += p->probability * strlen(p->code);
		p = p->next;
	}
	printf("Average Code Length: %f\n", avgCodeLen);
	ratio = 8.0 / avgCodeLen;
	return ratio;
}
void print_freqTable(node *list)
{
	node *p = list;
	printf("____________________\n");
	printf("data\tfrequency\n");
	printf("____________________\n");
	while (p)
	{
		printf("%c\t%d\n", p->data, p->freq);
		p = p->next;
	}
	printf("____________________\n\n");
}
void print_codeTable(node *list)
{
	node *p = list;
	printf("____________________\n");
	printf("data\tcode\n");
	printf("____________________\n");
	while (p)
	{
		printf("%c\t%s\n", p->data, p->code);
		p = p->next;
	}
	printf("____________________\n\n");
}
void print_probabilityTable(node *list)
{
	node *p = list;
	printf("____________________\n");
	printf("data\tprobability\n");
	printf("____________________\n");
	while (p)
	{
		printf("%c\t%f\n", p->data, p->probability);
		p = p->next;
	}
	printf("____________________\n\n");
}
void write_tree(const char *file, node *list, HuffManNode *root)
{
	FILE *write_file = fopen(file, "w");
	node *p = list;
	fprintf(write_file, "%d ", root->totalcode);
	while (p)
	{
		fprintf(write_file, "%s%c", p->code, p->data);
		p = p->next;
	}
	fclose(write_file);
}
HuffManNode *build_tree(const char *file)
{
	FILE *read_file = fopen(file, "r");
	HuffManNode *root = new HuffManNode;
	root->left = NULL;
	root->right = NULL;
	HuffManNode *p = root;
	char c;
	if (read_file)
	{
		fscanf(read_file, "%d", &(root->totalcode));
		while ((c = getc(read_file)) != EOF)
		{
			if (c == ' ')
				break;
		}
		while ((c = getc(read_file)) != EOF)
		{
			if (c == '0')
			{
				if (p->left == NULL)
				{
					HuffManNode *l = new HuffManNode;
					l->left = NULL;
					l->right = NULL;
					p->left = l;
				}
				p = p->left;
			}
			else if (c == '1')
			{
				if (p->right == NULL)
				{
					HuffManNode *r = new HuffManNode;
					r->left = NULL;
					r->right = NULL;
					p->right = r;
				}
				p = p->right;
			}
			else
			{
				p->data = c;
				p = root;
			}
		}
	}
	return root;
}
double calcTime(double start, double end)
{
	double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
	return timeSpent;
}
int main()
{
	double start, end;
	const char *read = "test_file_input.txt"; // input file name
	const char *write = "compressed.bin"; 	  // compressed output file
	const char *writetxt = "compressed.txt";  // compressed output file shown in characters 
	const char *decomLoc = "decompressed.txt"; // file after decompression 
	node *list = new node;
	int n;
	list = NULL;
	int c;
	FILE *read_file;
	read_file = fopen(read, "r");
	start = clock();

	if (read_file)
	{
		while ((c = getc(read_file)) != EOF)
			insertNode(&list, c);
		fclose(read_file);
	}

	insert_sort(list);
	print_freqTable(list);
	n = list_length(list);
	HuffManNode *t = createHuffManTree(list);

	node *p = list;
	while (p)
	{
		p->code = new char[n];
		p = p->next;
	}
	getCodes(t, list);

	print_codeTable(list);
	p = list;

	
	compress(read, write, list);
	end = clock();
	printf("time spent to compress:%lfseconds\n", calcTime(start, end));
	compressTxt(read, writetxt, list);

	printf("before compression: %f MB\n", file_size(read) / 8000000.0);
	printf("compressed file: %f MB\n", binary_file_size(writetxt) / 8000000.0);

	float ratio = get_probability_ratio(list, t->freq);
	int ratioAppr = round(ratio);
	print_probabilityTable(list);
	printf("Compression ratio:%f , approximately: %d\n", ratio, ratioAppr);

	write_tree("tree.txt", list, t);
	HuffManNode *root = build_tree("tree.txt");
	start = clock();
	decompress(write, decomLoc, root);
	end = clock();
	printf("time spent to decompress:%lf seconds\n", calcTime(start, end));
	return 0;
}
