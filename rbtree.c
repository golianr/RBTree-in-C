#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*
 * ------------------------------------------------------------
 * Red-Black Tree implementation in C
 * ------------------------------------------------------------
 *
 * Operations:
 *  - insert
 *  - search
 *  - delete
 *  - find min / max
 *  - inorder / preorder / postorder traversal
 *  - height
 *  - node count
 *  - validation of BST + RB-tree properties
 *  - destroy / free memory
 *
 * ------------------------------------------------------------
 */

typedef enum
{
    RED,
    BLACK
} Color;

typedef struct RBNode
{
    int key;
    Color color;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
} RBNode;

typedef struct RBTree
{
    RBNode *root;
    RBNode *nil;   /* sentinel leaf */
} RBTree;

/* ------------------------------------------------------------
 * Utility helpers
 * ------------------------------------------------------------ */

static RBNode *rbt_create_node(RBTree *tree, int key)
{
    RBNode *node = malloc(sizeof(RBNode));
    assert(node != NULL);

    node->key = key;
    node->color = RED;
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = tree->nil;

    return node;
}

static RBTree *rbt_create(void)
{
    RBTree *tree = malloc(sizeof(RBTree));
    assert(tree != NULL);

    tree->nil = malloc(sizeof(RBNode));
    assert(tree->nil != NULL);

    /* Sentinel NIL is always black */
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->parent = tree->nil;
    tree->nil->key = 0;

    tree->root = tree->nil;
    return tree;
}

/* Left rotation around node x
 *
 *        x                       y
 *         \                     / \
 *          y        ==>        x   c
 *         / \                   \
 *        b   c                   b
 */
static void rbt_left_rotate(RBTree *tree, RBNode *x)
{
    RBNode *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == tree->nil)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

/* Right rotation around node y */
static void rbt_right_rotate(RBTree *tree, RBNode *y)
{
    RBNode *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == tree->nil)
        tree->root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

/* ------------------------------------------------------------
 * Search / min / max
 * ------------------------------------------------------------ */

static RBNode *rbt_search_node(const RBTree *tree, int key)
{
    RBNode *current = tree->root;

    while (current != tree->nil)
    {
        if (key == current->key)
            return current;
        else if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }

    return tree->nil;
}

static bool rbt_contains(const RBTree *tree, int key)
{
    return rbt_search_node(tree, key) != tree->nil;
}

static RBNode *rbt_min_node(const RBTree *tree, RBNode *node)
{
    while (node->left != tree->nil)
        node = node->left;
    return node;
}

static RBNode *rbt_max_node(const RBTree *tree, RBNode *node)
{
    while (node->right != tree->nil)
        node = node->right;
    return node;
}

static int rbt_min(const RBTree *tree)
{
    assert(tree->root != tree->nil);
    return rbt_min_node(tree, tree->root)->key;
}

static int rbt_max(const RBTree *tree)
{
    assert(tree->root != tree->nil);
    return rbt_max_node(tree, tree->root)->key;
}

/* ------------------------------------------------------------
 * Insertion
 * ------------------------------------------------------------ */

static void rbt_insert_fixup(RBTree *tree, RBNode *z)
{
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            RBNode *y = z->parent->parent->right; /* uncle */

            if (y->color == RED)
            {
                /* Case 1: parent and uncle are red */
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    /* Case 2: triangle */
                    z = z->parent;
                    rbt_left_rotate(tree, z);
                }

                /* Case 3: line */
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbt_right_rotate(tree, z->parent->parent);
            }
        }
        else
        {
            /* Symmetric cases */
            RBNode *y = z->parent->parent->left; /* uncle */

            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rbt_right_rotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbt_left_rotate(tree, z->parent->parent);
            }
        }
    }

    tree->root->color = BLACK;
}

static bool rbt_insert(RBTree *tree, int key)
{
    RBNode *z = rbt_create_node(tree, key);
    RBNode *y = tree->nil;
    RBNode *x = tree->root;

    /* Standard BST insertion position search */
    while (x != tree->nil)
    {
        y = x;

        if (key == x->key)
        {
            /* Duplicate keys are not inserted */
            free(z);
            return false;
        }
        else if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if (y == tree->nil)
        tree->root = z;
    else if (key < y->key)
        y->left = z;
    else
        y->right = z;

    rbt_insert_fixup(tree, z);
    return true;
}

/* ------------------------------------------------------------
 * Deletion
 * ------------------------------------------------------------ */

static void rbt_transplant(RBTree *tree, RBNode *u, RBNode *v)
{
    if (u->parent == tree->nil)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

static void rbt_delete_fixup(RBTree *tree, RBNode *x)
{
    while (x != tree->root && x->color == BLACK)
    {
        if (x == x->parent->left)
        {
            RBNode *w = x->parent->right; /* sibling */

            if (w->color == RED)
            {
                /* Case 1 */
                w->color = BLACK;
                x->parent->color = RED;
                rbt_left_rotate(tree, x->parent);
                w = x->parent->right;
            }

            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                /* Case 2 */
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == BLACK)
                {
                    /* Case 3 */
                    w->left->color = BLACK;
                    w->color = RED;
                    rbt_right_rotate(tree, w);
                    w = x->parent->right;
                }

                /* Case 4 */
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rbt_left_rotate(tree, x->parent);
                x = tree->root;
            }
        }
        else
        {
            /* Symmetric cases */
            RBNode *w = x->parent->left;

            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                rbt_right_rotate(tree, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == BLACK && w->left->color == BLACK)
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == BLACK)
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    rbt_left_rotate(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rbt_right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }

    x->color = BLACK;
}

static bool rbt_delete(RBTree *tree, int key)
{
    RBNode *z = rbt_search_node(tree, key);
    if (z == tree->nil)
        return false;

    RBNode *y = z;
    Color y_original_color = y->color;
    RBNode *x;

    if (z->left == tree->nil)
    {
        x = z->right;
        rbt_transplant(tree, z, z->right);
    }
    else if (z->right == tree->nil)
    {
        x = z->left;
        rbt_transplant(tree, z, z->left);
    }
    else
    {
        y = rbt_min_node(tree, z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            rbt_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rbt_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z);

    if (y_original_color == BLACK)
        rbt_delete_fixup(tree, x);

    return true;
}

/* ------------------------------------------------------------
 * Traversals
 * ------------------------------------------------------------ */

static void rbt_inorder_print(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return;

    rbt_inorder_print(tree, node->left);
    printf("%d ", node->key);
    rbt_inorder_print(tree, node->right);
}

static void rbt_preorder_print(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return;

    printf("%d ", node->key);
    rbt_preorder_print(tree, node->left);
    rbt_preorder_print(tree, node->right);
}

static void rbt_postorder_print(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return;

    rbt_postorder_print(tree, node->left);
    rbt_postorder_print(tree, node->right);
    printf("%d ", node->key);
}

/* ------------------------------------------------------------
 * Tree metrics
 * ------------------------------------------------------------ */

static int rbt_height_node(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return -1;

    int left_h = rbt_height_node(tree, node->left);
    int right_h = rbt_height_node(tree, node->right);

    return 1 + (left_h > right_h ? left_h : right_h);
}

static int rbt_height(const RBTree *tree)
{
    return rbt_height_node(tree, tree->root);
}

static int rbt_count_nodes_node(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return 0;

    return 1
         + rbt_count_nodes_node(tree, node->left)
         + rbt_count_nodes_node(tree, node->right);
}

static int rbt_count_nodes(const RBTree *tree)
{
    return rbt_count_nodes_node(tree, tree->root);
}

/* ------------------------------------------------------------
 * Validation
 * ------------------------------------------------------------ */

/* Checks BST ordering with bounds */
static bool rbt_validate_bst_node(const RBTree *tree, RBNode *node,
                                  long long min_key, long long max_key)
{
    if (node == tree->nil)
        return true;

    if ((long long)node->key <= min_key || (long long)node->key >= max_key)
        return false;

    return rbt_validate_bst_node(tree, node->left, min_key, node->key) &&
           rbt_validate_bst_node(tree, node->right, node->key, max_key);
}

/* Verifies that no red node has a red child */
static bool rbt_validate_red_rule(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return true;

    if (node->color == RED)
    {
        if (node->left->color == RED || node->right->color == RED)
            return false;
    }

    return rbt_validate_red_rule(tree, node->left) &&
           rbt_validate_red_rule(tree, node->right);
}

/* Returns black height if valid, -1 if invalid */
static int rbt_black_height_check(const RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return 1; /* NIL is black */

    int left_bh = rbt_black_height_check(tree, node->left);
    int right_bh = rbt_black_height_check(tree, node->right);

    if (left_bh == -1 || right_bh == -1 || left_bh != right_bh)
        return -1;

    return left_bh + (node->color == BLACK ? 1 : 0);
}

static bool rbt_is_valid(const RBTree *tree)
{
    if (tree->root == tree->nil)
        return true;

    /* Root must be black */
    if (tree->root->color != BLACK)
        return false;

    /* BST ordering must hold */
    if (!rbt_validate_bst_node(tree, tree->root, LLONG_MIN, LLONG_MAX))
        return false;

    /* Red node cannot have red child */
    if (!rbt_validate_red_rule(tree, tree->root))
        return false;

    /* Black height must be equal on all root->leaf paths */
    if (rbt_black_height_check(tree, tree->root) == -1)
        return false;

    return true;
}

/* ------------------------------------------------------------
 * Destroy / free memory
 * ------------------------------------------------------------ */

static void rbt_destroy_nodes(RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
        return;

    rbt_destroy_nodes(tree, node->left);
    rbt_destroy_nodes(tree, node->right);
    free(node);
}

static void rbt_destroy(RBTree *tree)
{
    if (tree == NULL)
        return;

    rbt_destroy_nodes(tree, tree->root);
    free(tree->nil);
    free(tree);
}