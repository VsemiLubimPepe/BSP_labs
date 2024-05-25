#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "file_process.h"

// Structure for queue node of directory which was found during traversing
struct dir_queue_node
{
    // Directroy path
    char* dir_name;
    // Pointer to the next node of the directory queue
    struct dir_queue_node* next_dir;
};

// Function for checking, if queue is empty
int dir_queue_empty(struct dir_queue_node* dir_queue_head)
{
    if  (dir_queue_head->next_dir)
    {
	return 0;
    }
    return 1;
}

// Function for adding new directory queue tail element
int add_dir_queue_element(struct dir_queue_node* dir_queue_head, char* dir_name)
{
    // Pointer to the last element of queue
    struct dir_queue_node* last_dir_queue_node = dir_queue_head;
    // Getting the last element pointer
    while (last_dir_queue_node->next_dir)
    {
	last_dir_queue_node = last_dir_queue_node->next_dir;
    }
    // Creating new directory queue node
    struct dir_queue_node* new_node = (struct dir_queue_node*) malloc(sizeof(struct dir_queue_node));
    if (!new_node)
    {
	// Couldn't allocate enough memory
	return 1;
    }
    // Allocating memory for path of the directory 
    new_node->dir_name = (char*) malloc(sizeof(char) * (strlen(dir_name) + 1));
    if (!new_node->dir_name)
    {
	// Couldn't allocate enough memory
	free(new_node);
	return 1;
    }
    new_node->dir_name = strcpy(new_node->dir_name, dir_name);
    new_node->next_dir = NULL;
    last_dir_queue_node->next_dir = new_node;
    return 0;
}

// Function for removing directory queue head element
int remove_dir_queue_element(struct dir_queue_node* dir_queue_head)
{
    // Checking, if directory queue is empty
    if (dir_queue_empty(dir_queue_head))
    {
	return 1;
    }
    // Pointer to the new first element
    struct dir_queue_node* new_first_element = dir_queue_head->next_dir->next_dir;
    // Freeing memory of directory name in node
    free(dir_queue_head->next_dir->dir_name);
    // Freeing memory of the current first element
    free(dir_queue_head->next_dir);
    // Setting pointer to the new firest element
    dir_queue_head->next_dir = new_first_element;
    return 0;
}

void free_dir_queue(struct dir_queue_node* dir_queue_head)
{
    while (!remove_dir_queue_element(dir_queue_head));
}

int file_traversal(char* dir_path, char* search_mask)
{   
    // Creating head queue element
    struct dir_queue_node dir_queue_head = {.dir_name = NULL, .next_dir = NULL};
    // Adding passed to the program directory to the directory queue
    if (add_dir_queue_element(&dir_queue_head, dir_path))
    {
	// Couldn't allocate enough memory
	return -1;
    }
    
    // Traversing through the elements of the directory queue
    while (!dir_queue_empty(&dir_queue_head))
    {
	// Path to the directory which is traversed
	char* current_dir_name = dir_queue_head.next_dir->dir_name;
	// Pointer to the current directory which is traversed
	DIR* current_dir = opendir(current_dir_name);

	// Checking if directory stream could be opened
	if (!current_dir)
	{
	    // Add the switch for errno of opendir
	    printf("Couldn't open directory %s.\n", dir_queue_head.next_dir->dir_name);
	    remove_dir_queue_element(&dir_queue_head);
	    continue;
	}

	// Pointer to the struct with info about next file
	struct dirent* next_file;
	// Traversing through file tree
	while ((next_file = readdir(current_dir)))
	{
	    // TODO: add switch check for file type
	    if (next_file->d_type == DT_REG)
	    {
		// Can I use chdir() and not allocate memory for file_path (if there was no execute permission i couldn't open that directory?)
		char* current_file_name = (char*) malloc(sizeof(char) * (strlen(current_dir_name) + strlen(next_file->d_name) + 2));
		if (!current_file_name)
		{
		    free_dir_queue(&dir_queue_head);
		    // Couldn't allocate enough memory
		    return -1;
		}
		current_file_name = strcpy(current_file_name, current_dir_name);
		current_file_name = strcat(current_file_name, "/");
		current_file_name = strcat(current_file_name, next_file->d_name);

		switch (file_process(current_file_name, search_mask))
		{
		case -1:
		    fprintf(stderr, "Couldn't open file: %s.\n", current_file_name);
		    break;
		case 0:
		    fprintf(stderr, "%s\n", current_file_name);
		    break;
		case 1:
		    break;
		}
		
		free(current_file_name);
	    }
	    if (next_file->d_type == DT_DIR)
	    {
		if ((strcmp(next_file->d_name, ".") == 0) || (strcmp(next_file->d_name, "..") == 0))
		{
		    continue;
		}
		
		// Allocating space for path (don't know if it is necessary)
		char* found_dir_name = (char*) malloc(sizeof(char) * (strlen(current_dir_name) + strlen(next_file->d_name) + 2));
		if (!found_dir_name)
		{
		    free_dir_queue(&dir_queue_head);
		    // Couldn't allocate enough memory
		    return -1;
		}
		found_dir_name = strcpy(found_dir_name, current_dir_name);
		found_dir_name = strcat(found_dir_name, "/");
		found_dir_name = strcat(found_dir_name, next_file->d_name);
		if (access(found_dir_name, X_OK) == -1)
		{
		    fprintf(stderr, "Couldn't access files in the directory: %s.\n", found_dir_name);
		    free(found_dir_name);
		    continue;
		}
		add_dir_queue_element(&dir_queue_head, found_dir_name);
		free(found_dir_name);
	    }
	    
	}
	closedir(current_dir);
	remove_dir_queue_element(&dir_queue_head);
    }

    return 0;
}
