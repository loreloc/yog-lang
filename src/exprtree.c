
#include "exprtree.h"

struct expr_tree *expr_tree_make_op(enum operator op)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));
	new_tree->type = NODE_OPERATOR;
	new_tree->value.op = op;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_lit(int64_t lit)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));
	new_tree->type = NODE_LITERAL;
	new_tree->value.lit = lit;
	new_tree->left = NULL;
	new_tree->right = NULL;

	return new_tree;
}

struct expr_tree *expr_tree_make_sym(struct symbol *sym)
{
	struct expr_tree *new_tree = ymalloc(sizeof(struct expr_tree));
	new_tree->type = NODE_VARIABLE;
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

int64_t expr_tree_eval(struct expr_tree *tree)
{
	if(tree->type == NODE_LITERAL)
		return tree->value.lit;
	else if(tree->type == NODE_VARIABLE)
		return tree->value.sym->value;

	int64_t left_val = expr_tree_eval(tree->left);
	int64_t right_val = expr_tree_eval(tree->right);

	switch(tree->value.op)
	{
		case OP_PLUS:
			return left_val + right_val;
		case OP_MINUS:
			return left_val - right_val;
		case OP_MUL:
			return left_val * right_val;
		default: // case OP_DIV:
			return left_val / right_val;
	}
}

