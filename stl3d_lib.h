#ifndef _STL3D_LIB_H
#define _STL3D_LIB_H

#ifdef __cplusplus
extern "C"{
#endif

/* Various error codes that could be returned from the library.
 * STL_ERROR is the generic one (meaning it should be mapped to something
 * more meaningful at some point)
 */
#define STL_SUCCESS            0
#define STL_ERROR              1
#define STL_ERROR_INVALID_ARG  2
#define STL_ERROR_IO_ERROR     2
#define STL_ERROR_MEMORY_ERROR 3

/* The axis that the action will be performed around
 */
#define STL_AXIS_UNKNOWN 0
#define STL_AXIS_X       1
#define STL_AXIS_Y       2
#define STL_AXIS_Z       3

/* One of STL_AXIS_X, STL_AXIS_X, or STL_AXIS_X */
typedef int stl_axis_t;

#define STL_HEADER_SIZE 80

/* STL file format taken from https://en.wikipedia.org/wiki/STL_(file_format)
 *
 * At the moment this only handles binary STL files, not ASCII.
 */

typedef struct
{
	float t1;
	float t2;
	float t3;
} triplet_t;

typedef struct
{
	triplet_t normal;
	triplet_t vertex1;
	triplet_t vertex2;
	triplet_t vertex3;

	/* Attribute byte count */
	unsigned short abc;
} facet_t;

typedef struct
{
	unsigned char header[STL_HEADER_SIZE];
	unsigned int facets_count;
	facet_t  *facets;
} stl_t;


#if 1

int _log_err(int error, char *file, int line);
#define STL_LOG_ERR(error) _log_err(error, __FILE__, __LINE__)

#else

#define STL_LOG_ERR(error) (error)

#endif

/* Open and read an STL file into an STL object
 */
int stl_read_file(char *input_file, stl_t **stl_new);

/* Create and write a new STL file using the supplied
 * STL object. This function will fail if the output
 * file already exists.
 */
int stl_write_file(char *output_file, stl_t *stl);

/* Rotate the STL object along the specified axis the specified
 * number of degrees.
 */
int stl_rotate(stl_axis_t axis, float degrees, stl_t *stl);

/* Print to stdout the elements of the STL object
 */
void stl_print(stl_t *stl);

/* Free the STL object that was created by stl_read_file()
 */
void stl_free(stl_t *stl);


#ifdef __cplusplus
}
#endif

#endif  /* _STL3D_LIB_H */
