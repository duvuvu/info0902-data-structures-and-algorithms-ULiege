// Placez ici votre implémentation d'arbre binaire de recherche pour BP_firstfit.h

/*
 * Modified from avl_bf.h for First-Fit BST
 */

#ifndef BST_FIRSTFIT_H
#define BST_FIRSTFIT_H

/*
 * node->bf = height(node->right) - height(node->left)
 * balance factors can be kept up-to-date by knowning the previous balance factors and the change in height
 * it is not necesary to know the absolute height
 */
 
enum avlbf {
    LEFTHEAVY = -1,
    BALANCED = 0,
    RIGHTHEAVY = 1
};

enum avltraversal {
    PREORDER,
    INORDER,
    POSTORDER
};

typedef struct avlnode { // FF update: modify the structure of avlnode
    struct avlnode *left;
    struct avlnode *right;
    struct avlnode *parent;
    char bf;
    void *data;
    size_t key; // FF update: added to store key of disk in the list
    size_t space; // FF update: added to store free space of disk
    size_t maxspace; // FF update: added to store maximum free space in subtree
} avlnode;

typedef struct {
    int (*compare)(const void *, const void *);
    void (*print)(void *);
    void (*destroy)(void *);

    avlnode root;
    avlnode nil;
} avltree;

#define AVL_ROOT(avlt) (&(avlt)->root)
#define AVL_NIL(avlt) (&(avlt)->nil)
#define AVL_FIRST(avlt) ((avlt)->root.left)
#define AVL_MINIMAL(avlt) ((avlt)->min)

#define AVL_ISEMPTY(avlt) ((avlt)->root.left == &(avlt)->nil && (avlt)->root.right == &(avlt)->nil)
#define AVL_APPLY(avlt, func, cookie, order) avl_apply((avlt), (avlt)->root.left, (func), (cookie), (order))

avltree *avl_create(int (*compare_func)(const void *, const void *), void (*destroy_func)(void *));
void avl_destroy(avltree *avlt);

avlnode *avl_find(avltree *avlt, void *data);
avlnode *avl_successor(avltree *avlt, avlnode *node);

int avl_apply(avltree *avlt, avlnode *node, int (*func)(void *, void *), void *cookie, enum avltraversal order);
void avl_print(avltree *avlt, void (*print_func)(void *));

avlnode *avl_insert(avltree *avlt, void *data, size_t freeSpace, size_t key); // FF update: avl_insert needs freeSpace parameter
void *avl_delete(avltree *avlt, avlnode *node, int keep);

int avl_check_order(avltree *avlt, void *min, void *max);
int avl_check_height(avltree *avlt);

// FF update: new functions for first-fit algorithm
void avl_update_maxspace(avltree *avlt, avlnode *node);
avlnode *tree_search_ff(avltree *avlt, size_t size);

#endif /* BST_FIRSTFIT_H */
 