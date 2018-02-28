#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stl3d_lib.h"

stl_error_t
stl_from_heightmap_uchar(
	unsigned char *vals,
	unsigned int width,
	unsigned int height,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	)
{
	stl_error_t  error = STL_SUCCESS;
	unsigned int i = 0;
	double       *vals_double = NULL;

	if((NULL == vals) || (0 == width) || (0 == height))
	{
		error = STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	vals_double = (double *)malloc(sizeof(vals_double[0] * width * height));
	if(NULL == vals_double)
	{
		error = STL_LOG_ERR(STL_ERROR_MEMORY_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		for(i = 0; i < width * height; i++)
		{
			vals_double[i] = vals[i];
		}

		error = stl_from_heightmap_double(vals_double, width, height, scale_pct, base_height, units_per_pixel, stl);
	}

	if(NULL != vals_double)
	{
		free(vals_double);
		vals_double = NULL;
	}

	return STL_LOG_ERR(error);
}

stl_error_t
stl_from_heightmap_char(
	signed char *vals,
	unsigned int width,
	unsigned int height,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	)
{
	stl_error_t  error = STL_SUCCESS;
	unsigned int i = 0;
	double       *vals_double = NULL;

	if((NULL == vals) || (0 == width) || (0 == height))
	{
		error = STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	vals_double = (double *)malloc(sizeof(vals_double[0] * width * height));
	if(NULL == vals_double)
	{
		error = STL_LOG_ERR(STL_ERROR_MEMORY_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		for(i = 0; i < width * height; i++)
		{
			vals_double[i] = vals[i];
		}

		error = stl_from_heightmap_double(vals_double, width, height, scale_pct, base_height, units_per_pixel, stl);
	}

	if(NULL != vals_double)
	{
		free(vals_double);
		vals_double = NULL;
	}

	return STL_LOG_ERR(error);
}

/* Make an STL object from an array of double values
 */
stl_error_t
stl_from_heightmap_double(
	double *vals,
	unsigned int width,
	unsigned int height,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	)
{
	stl_error_t  error = STL_ERROR_UNSUPPORTED;

	if((NULL == vals) || (0 == width) || (0 == height) || (scale_pct <= 0.0) || (units_per_pixel <= 0.0) || (NULL == stl))
	{
		error = STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	return STL_LOG_ERR(error);
}
