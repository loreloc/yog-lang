
#include "scanner.h"

#define TEXT_MAX_SIZE 32

// the states of the finite state automata
typedef enum
{
	FSA_START,
	FSA_LITERAL,
	FSA_WORD,
	FSA_OPERATOR,
	FSA_COLON,
	FSA_SEMICOLON,
	FSA_ERROR,
	FSA_ACCEPT

} fsa_state_t;

// the classes of the characters in the source code
typedef enum
{
	CHAR_WHITESPACE,
	CHAR_DIGIT,
	CHAR_ALPHABETICAL,
	CHAR_OPERATOR,
	CHAR_COLON,
	CHAR_SEMICOLON,
	CHAR_UNKNOW

} char_class_t;

void update_cursor(location_t *cursor, char c);
char_class_t identify_char(char c);
fsa_state_t next_state(fsa_state_t state, char_class_t class);
token_t get_token_word(char *text);
token_t get_token_operator(char *text);

void scanner_init(scanner_t *scanner, FILE *file)
{
	scanner->file = file;
	scanner->lookahead = '\0';
	scanner->cursor.row = scanner->cursor.col = 1;
}

bool scan(scan_result_t *result, scanner_t *scanner, err_handler_t *err_handler)
{
	char text[TEXT_MAX_SIZE] = { '\0' };
	size_t text_length = 0;
	location_t text_location;

	fsa_state_t state = FSA_START;
	char character;

	// check the lookahead character
	if(scanner->lookahead == '\0')
	{
		character = fgetc(scanner->file);
	}
	else
	{
		character = scanner->lookahead;
		scanner->lookahead = '\0';
	}

	// check for the end of the file
	if(feof(scanner->file))
		return false;

	// get the next state of the automata
	state = next_state(state, identify_char(character));

	// update the cursor
	update_cursor(&scanner->cursor, character);

	// ignore whitespaces
	while(state == FSA_START)
	{
		// get the next character
		character = fgetc(scanner->file);

		// check for the end of the file
		if(feof(scanner->file))
			return false;

		// change the state
		state = next_state(state, identify_char(character));

		// update the cursor
		update_cursor(&scanner->cursor, character);
	}

	// initialize the result location
	text_location.row = scanner->cursor.row;
	text_location.col = scanner->cursor.col - 1;

	// add the read character to the text
	text[text_length++] = character;

	while(true)
	{
		// read the next character
		character = fgetc(scanner->file);

		// check fo the end of the file
		if(feof(scanner->file))
			break;

		// get the next state of the automata
		fsa_state_t new_state = next_state(state, identify_char(character));

		// check if the new state is final
		if(new_state == FSA_ACCEPT)
		{
			scanner->lookahead = character;
			break;
		}

		// change the state
		state = new_state;

		if(text_length < TEXT_MAX_SIZE)
		{
			// add the read character to the text
			text[text_length++] = character;
		}

		// update the cursor
		update_cursor(&scanner->cursor, character);
	}

	// check if the text string is too long
	if(text_length == TEXT_MAX_SIZE)
	{
		err_handler_add(err_handler,
			(err_t)
			{
				.location = text_location,
				.type = ERROR_LEXICAL,
				.message = string_duplicate("text overflow")
			});

		return false;
	}

	// construct the result token
	switch(state)
	{
		case FSA_LITERAL:
			result->token.type = TOKEN_LITERAL;
			result->token.literal = atoi(text);
			result->location = text_location;
			break;

		case FSA_WORD:
			result->token = get_token_word(text);
			result->location = text_location;
			break;

		case FSA_OPERATOR:
			result->token = get_token_operator(text);
			result->location = text_location;
			break;

		case FSA_COLON:
			result->token.type = TOKEN_COLON;
			result->location = text_location;
			break;

		case FSA_SEMICOLON:
			result->token.type = TOKEN_SEMICOLON;
			result->location = text_location;
			break;

		default:
			err_handler_add(err_handler,
				(err_t)
				{
					.location = text_location,
					.type = ERROR_LEXICAL,
					.message = string_duplicate(text)
				});
			return false;
	}

	return true;
}

void update_cursor(location_t *cursor, char c)
{
	if(c == '\n')
	{
		cursor->row++;
		cursor->col = 1;
	}
	else
	{
		cursor->col++;
	}
}

char_class_t identify_char(char c)
{
	// identify the character class of a character
	if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return CHAR_WHITESPACE;
	else if((c >= 'A' && c <= 'z') || (c >= 'a' && c <= 'z'))
		return CHAR_ALPHABETICAL;
	else if(c >= '0' && c <= '9')
		return CHAR_DIGIT;
	else if(c == '+' || c == '-' || c == '*' || c == '/' || c == '=')
		return CHAR_OPERATOR;
	else if(c == ':')
		return CHAR_COLON;
	else if(c == ';')
		return CHAR_SEMICOLON;
	else
		return CHAR_UNKNOW;
}

fsa_state_t next_state(fsa_state_t state, char_class_t class)
{
	static const fsa_state_t Transition_Table[7][7] =
	{
		             /*   WHITESPACE  DIGIT        ALPHABETICAL OPERATOR      COLON       SEMICOLON      UNKNOW    */
		/* START     */ { FSA_START,  FSA_LITERAL, FSA_WORD,    FSA_OPERATOR, FSA_COLON,  FSA_SEMICOLON, FSA_ERROR  },
		/* LITERAL   */ { FSA_ACCEPT, FSA_LITERAL, FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* WORD      */ { FSA_ACCEPT, FSA_WORD,    FSA_WORD,    FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* OPERATOR  */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* COLON     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* SEMICOLON */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* ERROR     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ERROR  }
	};

	// return the next state of the finite state automata
	return Transition_Table[(size_t)state][(size_t)class];
}

token_t get_token_word(char *text)
{
	token_t result;

	if(strcmp(text, "var") == 0)
		result.type = TOKEN_VAR;
	else if(strcmp(text, "begin") == 0)
		result.type = TOKEN_BEGIN;
	else if(strcmp(text, "end") == 0)
		result.type = TOKEN_END;
	else if(strcmp(text, "int") == 0)
		result.type = TOKEN_INT;
	else
	{
		result.type = TOKEN_IDENTIFIER;
		result.string = string_duplicate(text);
	}

	return result;
}

token_t get_token_operator(char *text)
{
	token_t result;

	switch(text[0])
	{
		case '+': result.type = TOKEN_PLUS;  break;
		case '-': result.type = TOKEN_MINUS; break;
		case '*': result.type = TOKEN_MUL;   break;
		case '/': result.type = TOKEN_DIV;   break;
		default:  result.type = TOKEN_EQUAL; break;
	}

	return result;
}

