
#include "ast.h"

struct ast *ast_make_nonterminal(enum ast_nonterminal_type nt)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = AST_NONTERMINAL;
	tree->value.nt = nt;
	tree->children_cnt = 0;
	tree->children = NULL;

	return tree;
}

struct ast *ast_make_terminal(struct token tok)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = AST_TERMINAL;
	tree->value.tok = tok;
	tree->children_cnt = 0;
	tree->children = NULL;

	return tree;
}

void ast_clear(struct ast *tree)
{
	if(tree == NULL)
		return;

	// clear the child trees
	for(size_t i = 0; i < tree->children_cnt; ++i)
		ast_clear(tree->children[i]);

	yfree(tree);
}

struct ast *ast_add_child(struct ast *tree, struct ast *child)
{
	tree->children = realloc(tree->children, (tree->children_cnt + 1) * sizeof(struct ast *));
	tree->children[tree->children_cnt++] = child;

	return child;
}

