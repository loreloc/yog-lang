
#include "ast.h"

struct ast *ast_make_nonterminal(enum ast_node_type type, size_t count, ...)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = type;
	tree->subtrees_cnt = count;
	tree->subtrees = ycalloc(count, sizeof(struct ast *));

	va_list subtrees;
	va_start(subtrees, count);

	for(size_t i = 0; i < tree->subtrees_cnt; ++i)
		tree->subtrees[i] = va_arg(subtrees, struct ast *);

	va_end(subtrees);

	return tree;
}

struct ast *ast_make_terminal(enum ast_node_type type)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = type;
	tree->subtrees_cnt = 0;
	tree->subtrees = NULL;

	return tree;
}

void ast_clear(struct ast *tree)
{
	if(tree == NULL)
		return;

	// clear the abstract syntax subtrees
	for(size_t i = 0; i < tree->subtrees_cnt; ++i)
		ast_clear(tree->subtrees[i]);

	yfree(tree);
}

