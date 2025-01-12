#ifndef PESTACLE_TREE_MAP_H
#define PESTACLE_TREE_MAP_H

#ifdef __cplusplus
extern "C" {
#endif


enum TreeMapNodeColor {
	TreeMapNodeColor_BLACK,
	TreeMapNodeColor_RED
}; // enum TreeMapNodeColor


struct s_TreeMapNode {
	struct s_TreeMapNode* parent;
	union {
		struct s_TreeMapNode* child[2];
		struct { 
			struct s_TreeMapNode* left;
			struct s_TreeMapNode* right;			
		};
	};
	enum TreeMapNodeColor color;
	void* key;
	void* value;
}; // struct s_TreeMapNode

typedef struct s_TreeMapNode TreeMapNode;


typedef struct {
	TreeMapNode root;
	TreeMapNode nil;
} TreeMap;


extern void
TreeMap_init(
	TreeMap* self
);


extern void
TreeMap_destroy(
	TreeMap* self
);


extern bool
TreeMap_empty(
	TreeMap* self
);


extern TreeMapNode*
TreeMap_pick(
	TreeMap* self
);


extern TreeMapNode*
TreeMap_find(
	TreeMap* self,
	void* key
);


extern TreeMapNode*
TreeMap_insert(
	TreeMap* self,
	void* key
);


extern void
TreeMap_erase(
	TreeMap* self,
	TreeMapNode* node
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_TREE_MAP_H */
