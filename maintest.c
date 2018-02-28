#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stl3d_lib.h"

void print_usage(const char *program)
{
	printf("Usage: %s [-r axis degrees] [-s x_pct y_pct z_pct] -i input_file -o output_file\n", program);
	printf("       -r: Rotate along the specified axis (x, y, or z) the specified number of degrees\n");
	printf("       -s: Scale by the specified percentag along each axis\n");
}

int main(int argc, char **argv)
{
	stl_error_t error = STL_SUCCESS;
	int         i = 0;

	stl_axis_t  axis = STL_AXIS_UNKNOWN;
	float       degrees = 0.0;

	double      pct_x = 0.0;
	double      pct_y = 0.0;
	double      pct_z = 0.0;

	char        *input_file = NULL;
	char        *output_file = NULL;

	stl_t       *stl = NULL;

	/* Simple arg processing, not using getopt() so it is windows friendly */
	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-r") == 0)
		{
			/* Need 2 more args for this option */
			if((i + 2) >= argc)
			{
				print_usage(argv[0]);
				exit(1);
			}

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

			degrees = (float)atof(argv[i+2]);

			i += 2;
		}
		else if(strcmp(argv[i], "-s") == 0)
		{
			/* Need 3 more args for this option */
			if((i + 3) >= argc)
			{
				print_usage(argv[0]);
				exit(1);
			}

			pct_x = atof(argv[i+1]);
			pct_y = atof(argv[i+2]);
			pct_z = atof(argv[i+3]);

			i += 3;
		}
		else if(strcmp(argv[i], "-i") == 0)
		{
			/* Need 1 more args for this option */
			if((i + 1) >= argc)
			{
				print_usage(argv[0]);
				exit(1);
			}

			input_file = argv[i+1];
			i++;
		}
		else if(strcmp(argv[i], "-o") == 0)
		{
			/* Need 1 more args for this option */
			if((i + 1) >= argc)
			{
				print_usage(argv[0]);
				exit(1);
			}

			output_file = argv[i+1];
			i++;
		}
		else
		{
			printf("Unknown option: %s\n", argv[i]);
			print_usage(argv[0]);
			exit(1);
		}
	}

	if((NULL == input_file) || (NULL == output_file) ||
		((axis == STL_AXIS_UNKNOWN) && ((pct_x == 0.0) || (pct_y == 0.0) || (pct_z == 0.0)))
		)
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
		stl_print_stats(stl);
	}

#if 0
	if(STL_SUCCESS == error)
	{
		stl_print(stl);
	}
#endif

	/* Do the rotation */
	if((STL_SUCCESS == error) && (STL_AXIS_UNKNOWN != axis))
	{
		error = stl_rotate(axis, degrees, stl);
	}

	if((STL_SUCCESS == error) && (pct_x != 0.0))
	{
		error = stl_scale(pct_x, pct_y, pct_z, stl);
	}

#if 0
	if(STL_SUCCESS == error)
	{
		stl_print(stl);
	}
#endif

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
