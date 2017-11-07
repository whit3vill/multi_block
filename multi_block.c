#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

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

void hash() {
	int i;

	const char *string = "asdfasdf";
	unsigned char result[MD5_DIGEST_LENGTH];

	MD5(string, strlen(string), result);

	for (i=0; i<8; i++)
		printf("%02x", result[i]);
	printf("\n");
}

void main () {
	insert(&root, 9);
	insert(&root, 4);
	insert(&root, 15);
	insert(&root, 6);
    insert(&root, 12);
    insert(&root, 17);
    insert(&root, 2);

    hash();
}

