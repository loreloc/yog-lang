
#include "exprtree.h"

struct expr_tree *expr_tree_make_operator(enum operator op)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));

	new_tree->type = EXPR_NODE_OPERATOR;
	new_tree->value.op = op;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_literal(int64_t lit)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));

	new_tree->type = EXPR_NODE_LITERAL;
	new_tree->value.lit = lit;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_symbol(struct symbol *sym)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));

	new_tree->type = EXPR_NODE_VARIABLE;
	new_tree->value.sym = sym;
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

int64_t expr_tree_evaluate(struct expr_tree *tree)
{
	if(tree->type == EXPR_NODE_LITERAL)
		return tree->value.lit;
	else if(tree->type == EXPR_NODE_VARIABLE)
		return tree->value.sym->value;

	int64_t lval = (tree->left != NULL) ? expr_tree_evaluate(tree->left) : 0;
	int64_t rval = expr_tree_evaluate(tree->right);

	switch(tree->value.op)
	{
		case OP_PLUS:
			return lval + rval;

		case OP_MINUS:
			return lval - rval;

		case OP_MUL:
			return lval * rval;

		default: // case OP_DIV:
			return lval / rval;
	}
}

