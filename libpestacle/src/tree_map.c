#include <assert.h>
#include <stdlib.h>

#include <pestacle/stack.h>
#include <pestacle/memory.h>
#include <pestacle/tree_map.h>


static TreeMapNode*
TreeMap_nil(
	TreeMap* self
) {
	return &(self->nil);
}


static TreeMapNode*
TreeMap_root(
	TreeMap* self
) {
	return &(self->root);
}


static TreeMapNode*
TreeMap_first(
	TreeMap* self
) {
	return self->root.left;
}


static TreeMapNode*
TreeMap_new_node(
	TreeMap* self,
	void* key,
	TreeMapNode* parent
) {
	TreeMapNode* ret = (TreeMapNode*)checked_malloc(sizeof(TreeMapNode));

	ret->parent = parent;

	for(int i = 0; i < 2; ++i)
		ret->child[i] = TreeMap_nil(self);

	ret->color = TreeMapNodeColor_RED;
	ret->key = key;
	ret->value = 0;

	return ret;
}


void
TreeMap_init(
	TreeMap* self
) {
	assert(self);

	// nil sentinel node
	self->nil.left = self->nil.right = self->nil.parent = TreeMap_nil(self);
	self->nil.color = TreeMapNodeColor_BLACK;
	self->nil.key = 0;
	self->nil.value = 0;

	// root sentinel node root
	self->root.left = self->root.right = self->root.parent = TreeMap_nil(self);
	self->root.color = TreeMapNodeColor_BLACK;
	self->root.key = 0;
	self->root.value = 0;
}


void
TreeMap_destroy(
	TreeMap* self
) {
	assert(self);

	Stack stack;
	Stack_init(&stack);

	Stack_push(&stack, TreeMap_first(self));
	while(!Stack_empty(&stack)) {
		TreeMapNode* node = (TreeMapNode*)Stack_pop(&stack);
		if (node != TreeMap_nil(self)) {
			for(int i = 0; i < 2; ++i)
				Stack_push(&stack, node->child[i]);

			free(node);
		}
	}

	Stack_destroy(&stack);
}


TreeMapNode*
TreeMap_find(
	TreeMap* self,
	void* key
) {
	assert(self);

	// Tree descent
 	TreeMapNode* node = TreeMap_first(self);
 	while(node != TreeMap_nil(self)) {
 		if (node->key == key)
 			return node;

 		node = node->child[key > node->key];
 	}

 	// No node found
 	return 0;
}


static void
TreeMap_rotate(
	TreeMap* self,
	TreeMapNode* x,
	bool left
) {
	assert(self);
	assert(x);

	TreeMapNode* y = x->child[left];

	x->child[left] = y->child[1 - left];
	if (x->child[left] != TreeMap_nil(self))
		x->child[left]->parent = x;

	y->parent = x->parent;
	if (x == x->parent->child[0])
		x->parent->child[0] = y;
	else
		x->parent->child[1] = y;

	y->child[1 - left] = x;
	x->parent = y;
}


static void
TreeMap_insert_balance(
	TreeMap* self,
	TreeMapNode* current
) {
	assert(self);
	assert(current);

	do {
		if (current->parent == current->parent->parent->left) {
			TreeMapNode* uncle = current->parent->parent->right;
			if (uncle->color == TreeMapNodeColor_RED) {
				current->parent->color = TreeMapNodeColor_BLACK;
				uncle->color = TreeMapNodeColor_BLACK;

				current = current->parent->parent;
				current->color = TreeMapNodeColor_RED;
			} else {
				if (current == current->parent->right) {
					current = current->parent;
					TreeMap_rotate(self, current, true);
				}

				current->parent->color = TreeMapNodeColor_BLACK;
				current->parent->parent->color = TreeMapNodeColor_RED;
				TreeMap_rotate(self, current->parent->parent, false);
			}
		} else {
			TreeMapNode* uncle = current->parent->parent->left;
			if (uncle->color == TreeMapNodeColor_RED) {
				current->parent->color = TreeMapNodeColor_BLACK;
				uncle->color = TreeMapNodeColor_BLACK;

				current = current->parent->parent;
				current->color = TreeMapNodeColor_RED;
			} else {
				if (current == current->parent->left) {
					current = current->parent;
					TreeMap_rotate(self, current, false);
				}

				current->parent->color = TreeMapNodeColor_BLACK;
				current->parent->parent->color = TreeMapNodeColor_RED;
				TreeMap_rotate(self, current->parent->parent, true);
			}
		}
	} while (current->parent->color == TreeMapNodeColor_RED);
}


TreeMapNode*
TreeMap_insert(
	TreeMap* self,
	void* key
) {
	assert(self);

	// Search for the parent node of the new node
	TreeMapNode* parent = TreeMap_root(self);
	TreeMapNode* node = TreeMap_first(self);

 	while(node != TreeMap_nil(self)) {
 		// If the key is already present, return 0
 		if (node->key == key)
 			return 0;

 		// Update
 		parent = node;
 		node = parent->child[key > node->key];
 	}

	// Insert the node
	TreeMapNode* ret = TreeMap_new_node(self, key, parent);
	if ((parent == TreeMap_root(self)) || (key < parent->key))
		parent->left = ret;
	else
		parent->right = ret;

	// Rebalance the tree
	if (ret->parent->color == TreeMapNodeColor_RED)
		TreeMap_insert_balance(self, ret);

	// Enforce the color of the first node
	TreeMap_first(self)->color = TreeMapNodeColor_BLACK;

	// Job done
	return ret;
}


static TreeMapNode*
TreeMap_successor(
	TreeMap* self,
	TreeMapNode* node
) {
	assert(self);
	assert(node);

	TreeMapNode* ret = node->right;
	if (ret != TreeMap_nil(self))
		for( ; ret->left != TreeMap_nil(self); ret = ret->left);
	else {
		for(ret = node->parent; node == ret->right; node = ret, ret = ret->parent);

		if (ret == TreeMap_root(self))
			ret = 0;
	}

	return ret;
}


static void
TreeMap_erase_balance(
	TreeMap* self,
	TreeMapNode* current
) {
	assert(self);
	assert(current);

	TreeMapNode* sibling;
	do {
		if (current == current->parent->left) {
			sibling = current->parent->right;

			if (sibling->color == TreeMapNodeColor_RED) {
				sibling->color = TreeMapNodeColor_BLACK;
				current->parent->color = TreeMapNodeColor_RED;
				TreeMap_rotate(self, current->parent, true);
				sibling = current->parent->right;
			}

			if (sibling->right->color == TreeMapNodeColor_BLACK && sibling->left->color == TreeMapNodeColor_BLACK) {
				sibling->color = TreeMapNodeColor_RED;
				if (current->parent->color == TreeMapNodeColor_RED) {
					current->parent->color = TreeMapNodeColor_BLACK;
					break;
				} else
					current = current->parent;
			} else {
				if (sibling->right->color == TreeMapNodeColor_BLACK) {
					sibling->left->color = TreeMapNodeColor_BLACK;
					sibling->color = TreeMapNodeColor_RED;
					TreeMap_rotate(self, sibling, false);
					sibling = current->parent->right;
				}

				sibling->color = current->parent->color;
				current->parent->color = TreeMapNodeColor_BLACK;
				sibling->right->color = TreeMapNodeColor_BLACK;
				TreeMap_rotate(self, current->parent, true);
				break;
			}
		} else {
			sibling = current->parent->left;

			if (sibling->color == TreeMapNodeColor_RED) {
				sibling->color = TreeMapNodeColor_BLACK;
				current->parent->color = TreeMapNodeColor_RED;
				TreeMap_rotate(self, current->parent, false);
				sibling = current->parent->left;
			}

			if (sibling->right->color == TreeMapNodeColor_BLACK && sibling->left->color == TreeMapNodeColor_BLACK) {
				sibling->color = TreeMapNodeColor_RED;
				if (current->parent->color == TreeMapNodeColor_RED) {
					current->parent->color = TreeMapNodeColor_BLACK;
					break;
				} else
					current = current->parent;
			} else {
				if (sibling->left->color == TreeMapNodeColor_BLACK) {
					sibling->right->color = TreeMapNodeColor_BLACK;
					sibling->color = TreeMapNodeColor_RED;
					TreeMap_rotate(self, sibling, true);
					sibling = current->parent->left;
				}

				sibling->color = current->parent->color;
				current->parent->color = TreeMapNodeColor_BLACK;
				sibling->left->color = TreeMapNodeColor_BLACK;
				TreeMap_rotate(self, current->parent, false);
				break;
			}
		}
	} while (current != TreeMap_first(self));
}


void 
TreeMap_erase(
	TreeMap* self,
	TreeMapNode* node
) {
	assert(self);
	assert(node);

	TreeMapNode* target;
	if (node->left == TreeMap_nil(self) || node->right == TreeMap_nil(self))
		target = node;
	else {
		target = TreeMap_successor(self, node);
		node->key = target->key;
		node->value = target->value;
	}

	TreeMapNode* child = (target->left == TreeMap_nil(self)) ? target->right : target->left;
	if (target->color == TreeMapNodeColor_BLACK) {
		if (child->color == TreeMapNodeColor_RED)
			child->color = TreeMapNodeColor_BLACK;
		else {
			if (target == TreeMap_first(self))
				;
			else
				TreeMap_erase_balance(self, target);
		}
	} 

	if (child != TreeMap_nil(self))
		child->parent = target->parent;

	if (target == target->parent->left)
		target->parent->left = child;
	else
		target->parent->right = child;

	free(target);
}
