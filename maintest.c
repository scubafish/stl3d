#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stl3d_lib.h"

void print_usage(const char *program)
{
	printf("Usage: %s -a (x, y, or z) -d degrees -i input_file -o output_file\n", program);
}

int main(int argc, char **argv)
{
	int        error = 0;
	int        i = 0;
	stl_axis_t axis = STL_AXIS_UNKNOWN;
	float      degrees = 0.0;
	char       *input_file = NULL;
	char       *output_file = NULL;
	stl_t      *stl = NULL;

	if(argc != 9)
	{
		print_usage(argv[0]);
		exit(1);
	}

	/* Simple arg processing, not using getopt() so it is windows friendly */
	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-a") == 0)
		{
			if(strcmp(argv[i+1], "x") == 0)
			{
				axis = STL_AXIS_X;
			}
			else if(strcmp(argv[i+1], "y") == 0)
			{
				axis = STL_AXIS_Y;
			}
			else if(strcmp(argv[i+1], "z") == 0)
			{
				axis = STL_AXIS_Z;
			}
			else
			{
				printf("Invalid Axis: %s\n", argv[i+1]);
				print_usage(argv[0]);
				exit(1);
			}
		}
		else if(strcmp(argv[i], "-d") == 0)
		{
			degrees = atof(argv[i+1]);
		}
		else if(strcmp(argv[i], "-i") == 0)
		{
			input_file = argv[i+1];
		}
		else if(strcmp(argv[i], "-o") == 0)
		{
			output_file = argv[i+1];
		}
		else
		{
			printf("Unknown option: %s\n", argv[i]);
			print_usage(argv[0]);
			exit(1);
		}

		i++;
	}

	if((NULL == input_file) || (NULL == output_file) || (axis == STL_AXIS_UNKNOWN))
	{
		print_usage(argv[0]);
		exit(1);
	}

	if(STL_SUCCESS == error)
	{
		error = stl_read_file(input_file, &stl);
	}

	if(STL_SUCCESS == error)
	{
		stl_print(stl);
	}

	/* Do the rotation */
	if(STL_SUCCESS == error)
	{
		error = stl_rotate(axis, degrees, stl);
	}

	if(STL_SUCCESS == error)
	{
		stl_print(stl);
	}

	/* Save to output file */
	if(STL_SUCCESS == error)
	{
		error = stl_write_file(output_file, stl);
	}

	/* Cleanup, get out */
	if(NULL != stl)
	{
		stl_free(stl);
		stl = NULL;
	}

	exit(0);
}
