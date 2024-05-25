#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Function for comparing mask bytes and file bytes
int compare_repr_bytes(int* byte_repr, char* bytes, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
	if (byte_repr[i] != (int) bytes[i])
	{
	    return 0;
	}
    }
    return 1;
}

int file_process(char* file_path, char* search_mask)
{
    char* need_debug = getenv("LAB11DEBUG");
    FILE* file = fopen(file_path, "r");
    if (!file)
    {
	// Couldn't open file with read permission
	return -1;
    }

    // Number of bytes in mask
    size_t mask_byte_length = strlen(search_mask) / 2 - 1;
    // Int representation of bytes
    int mask_repr[mask_byte_length];
    
    for (size_t i = 0; i < mask_byte_length; i++)
    {
	char mask_byte[3];
	strncpy(mask_byte, (search_mask + 2 * (i + 1)), 2);
	mask_byte[2] = '\0';
	mask_repr[i] = (int) strtol(mask_byte, NULL, 16);
    }

    char read_bytes[mask_byte_length];
    size_t substring_file_position = 0;
    while (fread(read_bytes, sizeof(char), mask_byte_length, file) == mask_byte_length)
    {
	substring_file_position++;
	if (compare_repr_bytes(mask_repr, read_bytes, mask_byte_length))
	{
	    if (need_debug)
	    {
		fprintf(stderr, "File: %s |Found EXPRESSION: %s starting from byte number %zu.\n", file_path, search_mask, substring_file_position);
	    }
	    fclose(file);
	    // Found needed byte substring
	    return 0;
	}
	fseek(file, 1 -  mask_byte_length, SEEK_CUR);
    }
    
    if (need_debug)
    {
	fprintf(stderr, "File: %s |EXPRESSION %s is not found.\n", file_path, search_mask);
    }
    fclose(file);
    // Didn't find needed byte substring
    return 1;
}
