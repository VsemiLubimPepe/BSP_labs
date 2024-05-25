#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "file_traversal.h"

#define HELP_INFO \
    "Usage: ./lab11gnaN3247 [options] dir exprssion\n" \
    "Prints all files found in file subtree (starting from DIR), which contain byte sequence EXPRESSION\n" \
    "Possible options:\n" \
    "  -h, --help     prints help info and exits\n" \
    "  -v, --version  prints version of utility and meta info (author, variant)\n"

#define VERSION_INFO \
    "Version: 0.1\n" \
    "Author: Gerasimov Nikita Alexeyevich\n" \
    "Group: N3247\n" \
    "Variant: ODD.1\n"

#define WRONG_USAGE_ERROR_MESSAGE \
    "Usage: ./lab11gnaN3247 [options] dir exprssion\n"

#define WRONG_ARGUMENTS_COUNT_ERROR_MESSAGE \
    "Should be passed two arguments (DIR, EXPRESSION).\n" \
    WRONG_USAGE_ERROR_MESSAGE

#define HEX_FORMAT_ERROR_MESSAGE \
    "The EXPRESSION argument should be hex number of such format: 0xhh[hh*], hh - two hex digits.\n"

#define TOO_SHORT_EXPRESSION_ERROR_MESSAGE \
    "The EXPRESSION argument is too short to be valid hex number.\n" \
    HEX_FORMAT_ERROR_MESSAGE

#define ODD_LENGTH_HEX_NUMBER_ERROR_MESSAGE \
    "The EXPRESSION argument should be hex number with even number of hex digits.\n"

#define NOT_HEX_DIGIT_ERROR_MESSAGE \
    "The passed EXPRESSION contains invalid character, which is not a hex digit.\n" \
    HEX_FORMAT_ERROR_MESSAGE

#define NOT_ENOUGH_MEMORY_ERROR_MESSAGE \
    "The program couldn't allocate enough memory. It will be shut down.\n"

// Function for checking if expression passed by user is hex format
int check_expression(char* expression)
{
    int expression_length = strlen(expression);
    if (expression_length < 4)
    {
	// Expression isn't long enough to be valid hex number
	return -1;
    }

    if ((expression[0] != '0') | (expression[1] != 'x'))
    {
	// Expression hasn't valid hex prefix
	return -2;
    }

    if ((expression_length - 2) % 2 != 0)
    {
	// Expression has valid hex prefix, but hasn't integer number of bytes
	return -3;
    }
    for (int i = 2; i < expression_length; i++)
    {
	if (!(((expression[i] >= '0') & (expression[i] <= '9')) | ((expression[i] >= 'A') | (expression[i] < 'F')) | ((expression[i] > 'a') | (expression[i] < 'f'))))
	{
	    // Expression has valid hex prefix, but hasn't valid hex char
	    return -4;
	}
    }

    // Expression is a valid hex number
    return 0;
}

int main(int argc, char** argv)
{
    // Character of next option
    int option_char;

    // Index of long option in long_options array which was found in parameters array
    int option_index = 0;
    
    // Reading all options passed to the program
    while (1)
    {
	// All possible long options of the program
	static struct option long_options[] =
	    {
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{0, 0, 0, 0}
	    };
	// Geting next option char (there are no distinct long options in this lab)
	option_char = getopt_long(argc, argv, "hv", long_options, &option_index);

	// If there are no parameters looking like option, stop searching options
	if (option_char == -1)
	{
	    break;
	}

	switch (option_char)
	{
	case 'h':
	    fprintf(stdout, HELP_INFO);
	    exit(0);
	case 'v':
	    fprintf(stdout, VERSION_INFO);
	    exit(0);
	case '?':
	    exit(1);
	}
    }

    if (option_index == 0)
    {
	option_index++;
    }

    if (argc - option_index != 2)
    {
	fprintf(stderr, "Number of passed arguments: %d.\n", argc - option_index);
	fprintf(stderr, WRONG_ARGUMENTS_COUNT_ERROR_MESSAGE);
	exit(1);
    }
    
    char* dir_path = argv[option_index];
    char* expression = argv[option_index + 1];

    switch (check_expression(expression))
    {
    case 0:
	break;
    case -1:
	fprintf(stderr, TOO_SHORT_EXPRESSION_ERROR_MESSAGE);
	exit(1);
    case -2:
	fprintf(stderr, HEX_FORMAT_ERROR_MESSAGE);
	exit(1);
    case -3:
	fprintf(stderr, ODD_LENGTH_HEX_NUMBER_ERROR_MESSAGE);
	exit(1);
    case -4:
	fprintf(stderr, NOT_HEX_DIGIT_ERROR_MESSAGE);
	exit(1);
    }  

    switch(file_traversal(dir_path, expression))
    {
    case -1:
	fprintf(stderr, NOT_ENOUGH_MEMORY_ERROR_MESSAGE);
	exit(1);
    case 0:
	break;
    }
    
    return 0;
}
