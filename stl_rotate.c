#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stl3d_lib.h"

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
	return (deg * M_PI / 180);
}

static double rad2deg(double rad)
{
	return (rad * 180 / M_PI);
}

void stl_print(stl_t *stl)
{
	int i = 0;

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

		printf("   Norm: %f %f %f\n", stl->facets[i].normal.t1, stl->facets[i].normal.t2, stl->facets[i].normal.t3);
		printf("      V1  : %f %f %f\n", stl->facets[i].vertex1.t1, stl->facets[i].vertex1.t2, stl->facets[i].vertex1.t3);
		printf("      V2  : %f %f %f\n", stl->facets[i].vertex2.t1, stl->facets[i].vertex2.t2, stl->facets[i].vertex2.t3);
		printf("      V3  : %f %f %f\n", stl->facets[i].vertex3.t1, stl->facets[i].vertex3.t2, stl->facets[i].vertex3.t3);
	}
}

/* This routine packs 4 big endian bytes from buffer into a 32 bit number,
 * converting it to the platforms native endian-ness.
 */
static unsigned int stl_pack_be32(
	const unsigned char *buffer
	)
{
	return(((unsigned int)buffer[3] <<  0) |
		((unsigned int)buffer[2] <<  8) |
		((unsigned int)buffer[1] << 16) |
		((unsigned int)buffer[0] << 24) );
}

/* This routine packs 4 little endian bytes from buffer into a 32 bit number,
 * converting it to the platforms native endian-ness.
 */
static unsigned int stl_pack_le32(
	const unsigned char *buffer
	)
{
	return(((unsigned int)buffer[3] <<  24) |
		((unsigned int)buffer[2] << 16) |
		((unsigned int)buffer[1] <<  8) |
		((unsigned int)buffer[0] <<  0) );
}

/* This routine packs 2 little endian bytes from buffer into a 16 bit number,
 * converting it to the platforms native endian-ness.
 */
static unsigned short stl_pack_le16(
	const unsigned char *buffer
	)
{
	return(((unsigned short)buffer[1] <<  8) | ((unsigned short)buffer[0] <<  0));
}


/* Takes a 32 bit integer in the platform's native endianness and converts
 * it to a 4 character array in little endian format suitable for writing to disc.
 */
static int stl_unpack_le32(const unsigned int val, unsigned char *buffer)
{
	int error = STL_SUCCESS;

	if(NULL == buffer)
	{
		return STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	buffer[0] = (unsigned char)((val >> 0) & 0xFF);
	buffer[1] = (unsigned char)((val >> 8) & 0xFF);
	buffer[2] = (unsigned char)((val >> 16) & 0xFF);
	buffer[3] = (unsigned char)((val >> 24) & 0xFF);

	return STL_LOG_ERR(error);
}

/* Takes a 16 bit integer in the platform's native endianness and converts
 * it to a 2 character array in little endian format suitable for writing to disc.
 */
static int stl_unpack_le16(const unsigned short val, unsigned char *buffer)
{
	int error = STL_SUCCESS;

	if(NULL == buffer)
	{
		return STL_LOG_ERR(STL_ERROR_INVALID_ARG);
	}

	buffer[0] = (unsigned char)((val >> 0) & 0xFF);
	buffer[1] = (unsigned char)((val >> 8) & 0xFF);

	return STL_LOG_ERR(error);
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

static int stl_read_next_triplet(FILE *fp, triplet_t *triplet)
{
	int error = STL_SUCCESS;
	int res = 0;
	float vals[3];

	if((NULL == fp) || (NULL == triplet))
	{
		error = STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		res = fread(vals, 1, sizeof(vals), fp);
		if(sizeof(vals) != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		triplet->t1 = vals[0];
		triplet->t2 = vals[1];
		triplet->t3 = vals[2];
	}

	return STL_LOG_ERR(error);
}

static int stl_read_next_facet(FILE *fp, facet_t *facet)
{
	int           error = STL_SUCCESS;
	int           res = 0;
	unsigned char buffer[48];
	float         vals[3];
	unsigned char uint16_bytes[2];

	if((NULL == fp) || (NULL == facet))
	{
		error = STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		error = stl_read_next_triplet(fp, &(facet->normal));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_read_next_triplet(fp, &(facet->vertex1));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_read_next_triplet(fp, &(facet->vertex2));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_read_next_triplet(fp, &(facet->vertex3));
	}

	if(STL_SUCCESS == error)
	{
		res = fread(uint16_bytes, 1, sizeof(uint16_bytes), fp);
		if(sizeof(uint16_bytes) != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		facet->abc = stl_pack_le16(uint16_bytes);
	}

	return STL_LOG_ERR(error);
}

int stl_read_file(char *input_file, stl_t **stl_new)
{
	int           error = STL_SUCCESS;
	FILE          *fp;
	int           i = 0;
	int           res = 0;
	unsigned char uint32_bytes[4];
	stl_t         *stl = NULL;

	if((NULL == input_file) || (NULL == stl_new))
	{
		return STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		fp = fopen(input_file, "rb");
		if(NULL == fp)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}


	if(STL_SUCCESS == error)
	{
		if(fseek(fp, 0, SEEK_SET))
		{
			/* Could not seek to start of file for some reason */
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		stl = (stl_t *)malloc(sizeof(*stl));
		if(NULL == stl)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		memset(stl, 0x00, sizeof(*stl));

		res = fread(stl->header, 1, STL_HEADER_SIZE, fp);
		if(STL_HEADER_SIZE != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		res = fread(uint32_bytes, 1, sizeof(uint32_bytes), fp);
		if(sizeof(uint32_bytes) != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		stl->facets_count = stl_pack_le32(uint32_bytes);

//		printf("stl->facets_count = %d\n", stl->facets_count);

		stl->facets = (facet_t *)malloc(stl->facets_count * sizeof(facet_t));
		if(NULL == stl->facets)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
		else
		{
			memset(stl->facets, 0x00, sizeof(stl->facets[0]));
		}
	}

	if(STL_SUCCESS == error)
	{
		for(i = 0; i < stl->facets_count; i++)
		{
			error = stl_read_next_facet(fp, &(stl->facets[i]));
			if(STL_SUCCESS != error)
			{
				break;
			}
		}		
	}

	/* Cleanup */
	if(STL_SUCCESS != error)
	{
		stl_free(stl);
		stl = NULL;
	}
	else
	{
		*stl_new = stl;
	}

	if(NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}

	return STL_LOG_ERR(error);
}

static int stl_write_next_triplet(FILE *fp, triplet_t *triplet)
{
	int error = STL_SUCCESS;
	int res = 0;
	float vals[3];

	if((NULL == fp) || (NULL == triplet))
	{
		error = STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		vals[0] = triplet->t1;
		vals[1] = triplet->t2;
		vals[2] = triplet->t3;

		res = fwrite(vals, 1, sizeof(vals), fp);
		if(sizeof(vals) != res)
		{
			error = STL_LOG_ERR(STL_ERROR_IO_ERROR);
		}
	}

	return STL_LOG_ERR(error);

}

static int stl_write_next_facet(FILE *fp, facet_t *facet)
{
	int           error = STL_SUCCESS;
	int           res = 0;
//	unsigned char buffer[48];
	float         vals[3];
	unsigned char uint16_bytes[2];

	if((NULL == fp) || (NULL == facet))
	{
		error = STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		error = stl_write_next_triplet(fp, &(facet->normal));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_write_next_triplet(fp, &(facet->vertex1));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_write_next_triplet(fp, &(facet->vertex2));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_write_next_triplet(fp, &(facet->vertex3));
	}

	if(STL_SUCCESS == error)
	{
		error = stl_unpack_le16(facet->abc, uint16_bytes);
	}

	if(STL_SUCCESS == error)
	{
		res = fwrite(uint16_bytes, 1, sizeof(uint16_bytes), fp);
		if(sizeof(uint16_bytes) != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	return STL_LOG_ERR(error);
}

int stl_write_file(char *output_file, stl_t *stl)
{
	int      error = STL_SUCCESS;
	int      res = 0;
	int      i = 0;
	FILE     *fp = NULL;
	unsigned char buffer[4];

	if((NULL == output_file) || (NULL == stl))
	{
		return STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		/* Check if exists. If it does, fail */
		fp = fopen(output_file, "rb");
		if(NULL != fp)
		{
			fprintf(stderr, "Error: Output file %s already exists\n", output_file);
			fclose(fp);
			fp = NULL;

			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		/* Now create the new file */
		fp = fopen(output_file, "wb");
		if(NULL == fp)
		{
			fprintf(stderr, "Error: Could not create Output file %s\n", output_file);

			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		res = fwrite(stl->header, 1, STL_HEADER_SIZE, fp);
		if(STL_HEADER_SIZE != res)
		{
			error = STL_LOG_ERR(STL_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		error = stl_unpack_le32(stl->facets_count, buffer);
	}

	if(STL_SUCCESS == error)
	{
		res = fwrite(buffer, 1, 4, fp);
		if(4 != res)
		{
			error = STL_LOG_ERR(STL_ERROR_IO_ERROR);
		}
	}

	if(STL_SUCCESS == error)
	{
		for(i = 0; i < stl->facets_count; i++)
		{
			error = stl_write_next_facet(fp, &(stl->facets[i]));

			if(STL_SUCCESS != error)
			{
				break;
			}
		}
	}

	if(NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}

	return STL_LOG_ERR(error);
}

static void _rot_vec(double cs, double sn, float *f1, float *f2)
{
	double px = 0.0;
	double py = 0.0;

	px = *f1 * cs - *f2 * sn;
	py = *f1 * sn + *f2 * cs;

	*f1 = (float)px;
	*f2 = (float)py;

}

int stl_rotate(stl_axis_t axis, float degrees, stl_t *stl)
{
	int    error = STL_SUCCESS;
	int    i = 0;
	double radians = 0.0;
	double cs = 0.0;
	double sn = 0.0;

	if(NULL == stl)
	{
		return STL_LOG_ERR(STL_ERROR);
	}

	if(STL_SUCCESS == error)
	{
		radians = deg2rad(degrees);
		cs = cos(radians);
		sn = sin(radians);

//		printf("deg: %f   rad: %f   cs: %f   sn: %f\n", degrees, radians, cs, sn);

		for(i = 0; i < stl->facets_count; i++)
		{
			_rot_vec(cs, sn, &stl->facets[i].normal.t1, &stl->facets[i].normal.t2);
			_rot_vec(cs, sn, &stl->facets[i].vertex1.t1, &stl->facets[i].vertex1.t2);
			_rot_vec(cs, sn, &stl->facets[i].vertex2.t1, &stl->facets[i].vertex2.t2);
			_rot_vec(cs, sn, &stl->facets[i].vertex3.t1, &stl->facets[i].vertex3.t2);
		}
	}

	return STL_LOG_ERR(error);
}
