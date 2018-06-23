
#include "scanner.h"

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
		/* ERROR     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT }
	};

	// return the next state of the finite state automata
	return Transition_Table[(size_t)state][(size_t)class];
}

void scanner_init(scanner_t *scanner, FILE *source_file)
{
	scanner->source_file = source_file;
	scanner->cursor.row = scanner->cursor.col = 1;
	scanner->lookahead = true;
}

