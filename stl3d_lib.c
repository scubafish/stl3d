#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stl3d_lib.h"

#define STL_PI 3.14159265358979323846

int _log_err(int error, char *file, int line)
{
	if(STL_SUCCESS != error)
	{
		fprintf(stderr, "Error: %d  %s(%d)\n", error, file, line);
	}

	return error;
}


static double deg2rad(double deg)
{
	return (deg * STL_PI / 180);
}


void stl_print(stl_t *stl)
{
	unsigned int i = 0;

	if(NULL == stl)
	{
		printf("NULL stl\n");
		return;
	}

	printf("Skipping Header\n");
	printf("stl->facets_count: %d\n", stl->facets_count);

	for(i = 0; i < stl->facets_count; i++)
	{
		printf("Facet %d:\n", i+1);

		printf("   Norm: %f %f %f\n", stl->facets[i].normal.x, stl->facets[i].normal.y, stl->facets[i].normal.z);
		printf("      V1  : %f %f %f\n", stl->facets[i].verticies[0].x, stl->facets[i].verticies[0].y, stl->facets[i].verticies[0].z);
		printf("      V2  : %f %f %f\n", stl->facets[i].verticies[1].x, stl->facets[i].verticies[1].y, stl->facets[i].verticies[1].z);
		printf("      V3  : %f %f %f\n", stl->facets[i].verticies[2].x, stl->facets[i].verticies[2].y, stl->facets[i].verticies[2].z);
	}
}

/* This function is incomplete
 */
void stl_print_stats(stl_t *stl)
{
	unsigned int i = 0;
	unsigned int j = 0;
	double length_x = 0.0;
	double length_y = 0.0;
	double length_z = 0.0;

	double min_x = 0.0;
	double min_y = 0.0;
	double min_z = 0.0;

	double max_x = 0.0;
	double max_y = 0.0;
	double max_z = 0.0;

	double surface_area = 0.0;

	if(NULL == stl)
	{
		printf("NULL stl\n");
		return;
	}

	printf("stl->facets_count: %d\n", stl->facets_count);

	if(stl->facets_count == 0)
	{
		return;
	}

	/* Prime the pump - set a min and max using the first point */
	min_x = stl->facets[0].verticies[0].x;
	min_y = stl->facets[0].verticies[0].y;
	min_z = stl->facets[0].verticies[0].z;

	max_x = stl->facets[0].verticies[0].x;
	max_y = stl->facets[0].verticies[0].y;
	max_z = stl->facets[0].verticies[0].z;

	for(i = 0; i < stl->facets_count; i++)
	{
		for(j = 0; j < 3; j++)
		{
			/* Find min x */
			if(stl->facets[i].verticies[j].x < min_x)
			{
				min_x = stl->facets[i].verticies[j].x;
			}

			/* Find max x */
			if(stl->facets[i].verticies[j].x > max_x)
			{
				max_x = stl->facets[i].verticies[j].x;
			}

			/* Find min y */
			if(stl->facets[i].verticies[j].y < min_y)
			{
				min_y = stl->facets[i].verticies[j].y;
			}

			/* Find max y */
			if(stl->facets[i].verticies[j].y > max_y)
			{
				max_y = stl->facets[i].verticies[j].y;
			}

			/* Find min z */
			if(stl->facets[i].verticies[j].z < min_z)
			{
				min_z = stl->facets[i].verticies[j].z;
			}

			/* Find max z */
			if(stl->facets[i].verticies[j].z > max_z)
			{
				max_z = stl->facets[i].verticies[j].z;
			}
		}
	}

	printf("min_x: %f   max_x: %f   width: %f\n", min_x, max_x, max_x - min_x);
	printf("min_y: %f   max_y: %f   width: %f\n", min_y, max_y, max_y - min_y);
	printf("min_z: %f   max_z: %f   width: %f\n", min_z, max_z, max_z - min_z);
}


void stl_free(stl_t *stl)
{
	if(NULL == stl)
	{
		return;
	}

	if(NULL != stl->facets)
	{
		free(stl->facets);
		stl->facets = NULL;
	}

	free(stl);
}


static void _rot_vec_x(double cs, double sn, vertex_t *vertex)
{
	double py = 0.0;
	double pz = 0.0;

	py = (double)vertex->y * cs - vertex->z * sn;
	pz = (double)vertex->y * sn + vertex->z * cs;

	vertex->y = (float)py;
	vertex->z = (float)pz;
}


static void _rot_vec_y(double cs, double sn, vertex_t *vertex)
{
	double px = 0.0;
	double pz = 0.0;

	px = (double)vertex->x * cs + vertex->z * sn;
	pz = -(double)vertex->x * sn + vertex->z * cs;

	vertex->x = (float)px;
	vertex->z = (float)pz;
}

static void _rot_vec_z(double cs, double sn, vertex_t *vertex)
{
	double px = 0.0;
	double py = 0.0;
	
	px = (double)vertex->x * cs - vertex->y * sn;
	py = (double)vertex->x * sn + vertex->y * cs;

	vertex->x = (float)px;
	vertex->y = (float)py;
}

stl_error_t stl_rotate(stl_axis_t axis, float degrees, stl_t *stl)
{
	stl_error_t  error = STL_SUCCESS;
	unsigned int i = 0;
	double       radians = 0.0;
	double       cs = 0.0;
	double       sn = 0.0;

	if(NULL == stl)
	{
		return STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		radians = deg2rad(degrees);
		cs = cos(radians);
		sn = sin(radians);

		/* TODO - The elegant way to do this is to set up a rotation matrix,
		 * which will also let us rotate around any vector. Do this.
		 */
		for(i = 0; i < stl->facets_count; i++)
		{
			if(STL_AXIS_X == axis)
			{
				_rot_vec_x(cs, sn, &stl->facets[i].normal);
				_rot_vec_x(cs, sn, &stl->facets[i].verticies[0]);
				_rot_vec_x(cs, sn, &stl->facets[i].verticies[1]);
				_rot_vec_x(cs, sn, &stl->facets[i].verticies[2]);
			}
			else if(STL_AXIS_Y == axis)
			{
				_rot_vec_y(cs, sn, &stl->facets[i].normal);
				_rot_vec_y(cs, sn, &stl->facets[i].verticies[0]);
				_rot_vec_y(cs, sn, &stl->facets[i].verticies[1]);
				_rot_vec_y(cs, sn, &stl->facets[i].verticies[2]);
			}
			else
			{
				/* axis == z */
				_rot_vec_z(cs, sn, &stl->facets[i].normal);
				_rot_vec_z(cs, sn, &stl->facets[i].verticies[0]);
				_rot_vec_z(cs, sn, &stl->facets[i].verticies[1]);
				_rot_vec_z(cs, sn, &stl->facets[i].verticies[2]);
			}
		}
	}

	return STL_LOG_ERR(error);
}

stl_error_t stl_scale(double pct_x, double pct_y, double pct_z, stl_t *stl)
{
	stl_error_t  error = STL_SUCCESS;
	unsigned int i = 0;
	double       scale_x = pct_x / 100.0;
	double       scale_y = pct_y / 100.0;
	double       scale_z = pct_z / 100.0;

	if(NULL == stl)
	{
		error = STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	if(STL_SUCCESS == error)
	{
		for(i = 0; i < stl->facets_count; i++)
		{
			stl->facets[i].verticies[0].x *= (float)scale_x;
			stl->facets[i].verticies[0].y *= (float)scale_y;
			stl->facets[i].verticies[0].z *= (float)scale_z;

			stl->facets[i].verticies[1].x *= (float)scale_x;
			stl->facets[i].verticies[1].y *= (float)scale_y;
			stl->facets[i].verticies[1].z *= (float)scale_z;

			stl->facets[i].verticies[2].x *= (float)scale_x;
			stl->facets[i].verticies[2].y *= (float)scale_y;
			stl->facets[i].verticies[2].z *= (float)scale_z;
		}
	}

	return STL_LOG_ERR(error);
}
