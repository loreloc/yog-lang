
#include "ast.h"

struct ast *ast_make(enum ast_node_type type)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = type;
	tree->next = NULL;
	tree->subtree = NULL;

	return tree;
}

void ast_clear(struct ast *tree)
{
	if(tree == NULL)
		return;

	// clear the abstract syntax subtree
	ast_clear(tree->subtree);

	// clear the next abstract syntax tree
	ast_clear(tree->next);

	yfree(tree);
}

void ast_add_subtree(struct ast *tree, struct ast *new_tree)
{
	struct ast *tmp = tree->subtree;

	if(tmp == NULL)
	{
		tree->subtree = new_tree;
	}
	else
	{
		while(tmp->next != NULL)
			tmp = tmp->next;

		tmp->next = new_tree;
	}
}

struct ast *ast_add_subtree_node(struct ast *tree, enum ast_node_type type)
{
	struct ast *new_tree = ast_make(type);

	ast_add_subtree(tree, new_tree);

	return new_tree;
}

struct ast *ast_add_subtree_literal(struct ast *tree, int64_t lit)
{
	struct ast *new_tree = ast_make(AST_NODE_LITERAL);
	new_tree->value.lit = lit;

	ast_add_subtree(tree, new_tree);

	return new_tree;
}

struct ast *ast_add_subtree_symbol(struct ast *tree, struct symbol *sym)
{
	struct ast *new_tree = ast_make(AST_NODE_IDENTIFIER);
	new_tree->value.sym = sym;

	ast_add_subtree(tree, new_tree);

	return new_tree;
}

struct ast* ast_get_subtree(struct ast *tree, size_t index)
{
	struct ast *tmp = tree->subtree;

	for(size_t i = 0; i < index; ++i)
		tmp = tmp->next;

	return tmp;
}

