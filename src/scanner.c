
#include "scanner.h"

#define TEXT_SIZE ID_STR_SIZE
#define TEXT_OVERFLOW_MSG "TOKEN TOO LONG"

// the states of the finite state automata
enum fsa_state
{
	FSA_START,
	FSA_LITERAL,
	FSA_WORD,
	FSA_SIGN,
	FSA_OPERATOR,
	FSA_EQ,
	FSA_NEQ,
	FSA_LT,
	FSA_LTE,
	FSA_GT,
	FSA_GTE,
	FSA_COLON,
	FSA_ASSIGN,
	FSA_SEMICOLON,
	FSA_PAREN,
	FSA_ERROR,
	FSA_EOF,
	FSA_ACCEPT
};

// the classes of the characters in the source code
enum char_class
{
	CHAR_WHITESPACE,
	CHAR_DIGIT,
	CHAR_ALPHABETICAL,
	CHAR_SIGN,
	CHAR_OPERATOR,
	CHAR_LESS,
	CHAR_GREATER,
	CHAR_COLON,
	CHAR_EQUAL,
	CHAR_SEMICOLON,
	CHAR_PAREN,
	CHAR_EOF,
	CHAR_UNKNOW
};

void report_lexical_error(struct lex_context *ctx, struct location loc, char *text);
void update_cursor(struct location *loc, char c);
enum char_class identify_char(char c);
enum fsa_state next_state(enum fsa_state state, enum char_class class);
struct token make_token_word(char *text, struct symbol_table *st, struct location loc);
struct token make_token_operator(char *text, struct location loc);
struct token make_token_paren(char *text, struct location loc);

void lex_context_init(struct lex_context *ctx, FILE *source, struct symbol_table *st, struct error_list *errs)
{
	ctx->source = source;
	ctx->lookahead = '\0';
	ctx->loc.row = 1;
	ctx->loc.col = 1;
	ctx->st = st;
	ctx->errs = errs;
}

struct token lex(struct lex_context *ctx)
{
	char text[TEXT_SIZE] = { '\0' };
	size_t text_len = 0;
	struct location text_loc;
	enum fsa_state state = FSA_START;
	char character;

	// get the first character
	if(ctx->lookahead == '\0')
	{
		character = fgetc(ctx->source);
	}
	else
	{
		character = ctx->lookahead;
		ctx->lookahead = '\0';
	}

	while(true)
	{
		// get the next automata state
		enum fsa_state new_state = next_state(state, identify_char(character));

		// handle the different state transitions
		if(state == FSA_START)
		{
			if(new_state == FSA_EOF)
				break;

			if(new_state != FSA_START)
				text_loc = ctx->loc;
		}
		else if(state == FSA_ERROR && new_state != FSA_ERROR)
		{
			report_lexical_error(ctx, text_loc, (text_len < TEXT_SIZE) ? text : TEXT_OVERFLOW_MSG);
			memset(text, '\0', TEXT_SIZE);
			text_len = 0;
			new_state = FSA_START;
		}
		else if(new_state == FSA_ACCEPT)
		{
			if(text_len == TEXT_SIZE)
			{
				report_lexical_error(ctx, text_loc, TEXT_OVERFLOW_MSG);
				memset(text, '\0', TEXT_SIZE);
				text_len = 0;
				new_state = FSA_START;
			}
			else
			{
				ctx->lookahead = character;
				break;
			}
		}

		state = new_state;

		// add the read character into the text buffer
		if(state != FSA_START && text_len < TEXT_SIZE)
			text[text_len++] = character;

		update_cursor(&ctx->loc, character);

		character = fgetc(ctx->source);
	}

	// construct the result token
	struct token result;
	switch(state)
	{
		case FSA_LITERAL:
			result.type = TOKEN_LITERAL;
			result.lit = atoi(text);
			result.loc = text_loc;
			break;

		case FSA_WORD:
			result = make_token_word(text, ctx->st, text_loc);
			break;

		case FSA_SIGN:
			result = make_token_operator(text, text_loc);
			break;

		case FSA_OPERATOR:
			result = make_token_operator(text, text_loc);
			break;

		case FSA_COLON:
			result.type = TOKEN_COLON;
			result.loc = text_loc;
			break;

		case FSA_ASSIGN:
			result.type = TOKEN_ASSIGN;
			result.loc = text_loc;
			break;

		case FSA_EQ:
			result.type = TOKEN_EQ;
			result.loc = text_loc;
			break;

		case FSA_NEQ:
			result.type = TOKEN_NEQ;
			result.loc = text_loc;
			break;

		case FSA_LT:
			result.type = TOKEN_LT;
			result.loc = text_loc;
			break;

		case FSA_LTE:
			result.type = TOKEN_LTE;
			result.loc = text_loc;
			break;

		case FSA_GT:
			result.type = TOKEN_GT;
			result.loc = text_loc;
			break;

		case FSA_GTE:
			result.type = TOKEN_GTE;
			result.loc = text_loc;
			break;

		case FSA_SEMICOLON:
			result.type = TOKEN_SEMICOLON;
			result.loc = text_loc;
			break;

		case FSA_PAREN:
			result = make_token_paren(text, text_loc);
			break;

		default:
			result.type = TOKEN_EOF;
			result.loc = ctx->loc;
			break;
	}

	return result;
}

void report_lexical_error(struct lex_context *ctx, struct location loc, char *text)
{
	error_list_add(ctx->errs, error_make_invalid_token(loc, text));
}

void update_cursor(struct location *loc, char c)
{
	if(c == '\n')
	{
		loc->row++;
		loc->col = 1;
	}
	else
	{
		loc->col++;
	}
}

enum char_class identify_char(char c)
{
	// identify the character class of a character
	if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return CHAR_WHITESPACE;
	else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return CHAR_ALPHABETICAL;
	else if(c >= '0' && c <= '9')
		return CHAR_DIGIT;
	else if(c == '+' || c == '-')
		return CHAR_SIGN;
	else if(c == '*' || c == '/')
		return CHAR_OPERATOR;
	else if(c == '<')
		return CHAR_LESS;
	else if(c == '>')
		return CHAR_GREATER;
	else if(c == ':')
		return CHAR_COLON;
	else if(c == '=')
		return CHAR_EQUAL;
	else if(c == ';')
		return CHAR_SEMICOLON;
	else if(c == '(' || c == ')')
		return CHAR_PAREN;
	else if(c == EOF)
		return CHAR_EOF;
	else
		return CHAR_UNKNOW;
}

enum fsa_state next_state(enum fsa_state state, enum char_class class)
{
	static const enum fsa_state Transition_Table[17][13] =
	{
		             /*   WHITESPACE  DIGIT        ALPHABETICAL SIGN        OPERATOR      LESS        GREATER      COLON       EQUAL       SEMICOLON      PAREN       EOF         UNKNOW   */
		/* START     */ { FSA_START,  FSA_LITERAL, FSA_WORD,    FSA_SIGN,   FSA_OPERATOR, FSA_LT,     FSA_GT,      FSA_COLON,  FSA_EQ,     FSA_SEMICOLON, FSA_PAREN,  FSA_EOF,    FSA_ERROR },
		/* LITERAL   */ { FSA_ACCEPT, FSA_LITERAL, FSA_ERROR,   FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* WORD      */ { FSA_ACCEPT, FSA_WORD,    FSA_WORD,    FSA_ERROR,  FSA_ERROR,    FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* SIGN      */ { FSA_ACCEPT, FSA_LITERAL, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* OPERATOR  */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* EQ        */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* NEQ       */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* LT        */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ERROR,  FSA_NEQ,     FSA_ACCEPT, FSA_LTE,    FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* LTE       */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ERROR,  FSA_ERROR,   FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* GT        */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ERROR,  FSA_ERROR,   FSA_ACCEPT, FSA_GTE,    FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* GTE       */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ERROR,  FSA_ERROR,   FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* COLON     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ASSIGN, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* ASSIGN    */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ERROR,  FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* EQUAL     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ERROR,  FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* SEMICOLON */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* PAREN     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT, FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT, FSA_ACCEPT, FSA_ERROR },
		/* ERROR     */ { FSA_START,  FSA_ERROR,   FSA_ERROR,   FSA_ERROR,  FSA_ERROR,    FSA_ERROR,  FSA_ERROR,   FSA_ERROR,  FSA_ERROR,  FSA_ERROR,     FSA_ERROR,  FSA_ACCEPT, FSA_ERROR }
	};

	return Transition_Table[(size_t)state][(size_t)class];
}

struct token make_token_word(char *text, struct symbol_table *st, struct location loc)
{
	struct token tok;

	if(strcmp(text, "var") == 0)
		tok.type = TOKEN_VAR;
	else if(strcmp(text, "begin") == 0)
		tok.type = TOKEN_BEGIN;
	else if(strcmp(text, "end") == 0)
		tok.type = TOKEN_END;
	else if(strcmp(text, "int") == 0)
		tok.type = TOKEN_INT;
	else if(strcmp(text, "read") == 0)
		tok.type = TOKEN_READ;
	else if(strcmp(text, "write") == 0)
		tok.type = TOKEN_WRITE;
	else if(strcmp(text, "if") == 0)
		tok.type = TOKEN_IF;
	else if(strcmp(text, "else") == 0)
		tok.type = TOKEN_ELSE;
	else if(strcmp(text, "while") == 0)
		tok.type = TOKEN_WHILE;
	else
	{
		tok.type = TOKEN_IDENTIFIER;
		
		// find the identifier in the symbol table
		struct symbol *sym = symbol_table_find(*st, text);

		// if the symbol isn't found add it to the symbol table
		if(sym == NULL)
			sym = symbol_table_add(st, text);

		tok.sym = sym;
	}

	tok.loc = loc;

	return tok;
}

struct token make_token_operator(char *text, struct location loc)
{
	struct token tok;

	switch(text[0])
	{
		case '+':
			tok.type = TOKEN_PLUS;
			break;

		case '-':
			tok.type = TOKEN_MINUS;
			break;

		case '*':
			tok.type = TOKEN_MUL;
			break;

		default: // case '/':
			tok.type = TOKEN_DIV;
			break;
	}

	tok.loc = loc;

	return tok;
}

struct token make_token_paren(char *text, struct location loc)
{
	struct token tok;

	switch(text[0])
	{
		case '(':
			tok.type = TOKEN_LPAREN;
			break;

		default: // case ')':
			tok.type = TOKEN_RPAREN;
			break;
	}

	tok.loc = loc;

	return tok;
}

