
#include "exprtree.h"

struct expr_tree *expr_tree_make_op(enum operator op)
{
	struct expr_tree *new_tree = malloc(sizeof(struct expr_tree));
	new_tree->type = NODE_OPERATOR;
	new_tree->op = op;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_lit(int64_t lit)
{
	struct expr_tree *new_tree = malloc(sizeof(struct expr_tree));
	new_tree->type = NODE_LITERAL;
	new_tree->lit = lit;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_sym(struct symbol *sym)
{
	struct expr_tree *new_tree = malloc(sizeof(struct expr_tree));
	new_tree->type = NODE_VARIABLE;
	new_tree->sym = sym;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

void expr_tree_clear(struct expr_tree *tree)
{
	if(tree == NULL)
		return;

	// clear the left and the right sub-trees
	expr_tree_clear(tree->left);
	expr_tree_clear(tree->right);

	free(tree);
}

