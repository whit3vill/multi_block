#include "multi_block.h"

struct bintree {
	int key;
	struct bintree *left;
	struct bintree *right;
};
struct bintree *root = NULL;

void insert (struct bintree **root, int val) {
	struct bintree *temp = NULL;
	if (!*root) {
		temp = (struct bintree *)malloc(sizeof(struct bintree));
		temp->left = NULL;
		temp->right = NULL;
		temp->key = val;
		*root = temp;

		return;
	}
	if (val < (*root)->key)	return insert(&(*root)->left, val);
	else if (val > (*root)->key)	return insert(&(*root)->right, val);
}


struct bintree *search (struct bintree *root, int val) {
	if (!root) return NULL;
	if (val < root->key)	return search(root->left, val);
	else if (val > root->key)	return search(root->right, val);
	else if (val == root->key)	return root;
}

int myhash(const char *string) {
	int i;
	unsigned int num = 0;

	unsigned char result[MD5_DIGEST_LENGTH];



	MD5(string, strlen(string), result);

	for (i=0; i<16; i++) {
		printf("%02x", result[i]);
		num += (int)(result[i]) * (int)(pow((double)(16), (double)(6-2*(i))));
	}
	printf("\n");

	return num;
}
const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
void main (int argc, char *argv[]) {
	char *filename;
	FILE *fp;
	char buf[BUFSIZE];
	char *string;
	char *line;
	int count;

	filename = argv[1];
	fp = fopen (filename, "r");
	if (fp == NULL)	printf("No %s in directory.\n", filename);

	while ((line = fgets(buf, BUFSIZE, fp)) != NULL) {
		string = strtok(line, ",");
		count = 0;
		while (string) {
			count++;
			if (count == 2) {
				string[strlen(string)-1] = 0;
				printf("%s\n", string);
				printf("%u\n", myhash(string));
			}
			string = strtok(NULL, ",");
		}
    }
    string = "google.com";
    printf("%u\n", myhash(string));
    printf("%u\n", myhash("google.com"));
	insert(&root, 9);
	insert(&root, 4);
	insert(&root, 15);
	insert(&root, 6);
    insert(&root, 12);
    insert(&root, 17);
    insert(&root, 2);
}

