
#include "ast.h"

struct ast *ast_make(enum ast_type type)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = type;
	tree->children_cnt = 0;
	tree->children = NULL;

	return tree;
}

struct ast *ast_make_literal(int64_t lit)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = AST_LITERAL;
	tree->value.lit = lit;
	tree->children_cnt = 0;
	tree->children = NULL;

	return tree;
}

struct ast *ast_make_symbol(struct symbol *sym)
{
	struct ast *tree = ymalloc(sizeof(struct ast));

	tree->type = AST_IDENTIFIER;
	tree->value.sym = sym;
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
	size_t new_children_cnt = tree->children_cnt + 1;
	tree->children = realloc(tree->children, new_children_cnt * sizeof(struct ast *));
	tree->children[tree->children_cnt] = child;
	tree->children_cnt = new_children_cnt;

	return child;
}

