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
#define STL_ERROR_UNSUPPORTED  4

typedef unsigned int stl_error_t;

/* 0,0 origin when processing heightmaps
 */
#define STL_ORIGIN_TOP_LEFT 0
#define STL_ORIGIN_BOTTOM_LEFT 1

typedef unsigned int stl_origin_t;


/* The axis that the action will be performed around
 */
#define STL_AXIS_UNKNOWN 0
#define STL_AXIS_X       1
#define STL_AXIS_Y       2
#define STL_AXIS_Z       3

/* One of STL_AXIS_X, STL_AXIS_X, or STL_AXIS_X */
typedef unsigned int stl_axis_t;

#define STL_HEADER_SIZE 80

/* STL file format taken from https://en.wikipedia.org/wiki/STL_(file_format)
 *
 * At the moment this only handles binary STL files, not ASCII.
 */

typedef struct
{
	float x;
	float y;
	float z;
} vertex_t;

typedef struct
{
	vertex_t normal;
	vertex_t verticies[3];

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

/* Create a new empty STL object with the specified number of fascents allocated
 */
stl_error_t stl_new(stl_t **stl, unsigned int fascets_count);

/* Free the STL object that was created by stl_read_file()
 */
void stl_free(stl_t *stl);

/* Open and read an STL file into an STL object
 */
stl_error_t stl_read_file(char *input_file, stl_t **stl_new);

/* Create and write a new STL file using the supplied
 * STL object. This function will fail if the output
 * file already exists.
 */
stl_error_t stl_write_file(char *output_file, stl_t *stl);

/* Rotate the STL object along the specified axis the specified
 * number of degrees.
 */
stl_error_t stl_rotate(stl_axis_t axis, float degrees, stl_t *stl);

/* Rotate the stl object along each axis by the specified percentages
 *
 * A value of 100.0 means don't scale that axis.
 */
stl_error_t stl_scale(double pct_x, double pct_y, double pct_z, stl_t *stl);

/* Make an STL object from a file containing 8 bit unsigned grayscale values
 */
stl_error_t
stl_from_heightmap_uchar_file(
	char *filename,
	stl_origin_t origin,
	unsigned int cols,
	unsigned int rows,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	);

/* Make an STL object from an array of unsigned 8 bit grayscale values
 */
stl_error_t
stl_from_heightmap_uchar(
	const unsigned char *vals,
	stl_origin_t origin,
	unsigned int cols,
	unsigned int rows,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	);

/* Make an STL object from an array of signed 8 bit grayscale values
 */
stl_error_t
stl_from_heightmap_char(
	const signed char *vals,
	stl_origin_t origin,
	unsigned int cols,
	unsigned int rows,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	);

/* Make an STL object from an array of double values
 *
 * scale_pct: Percentage to scale the height values (100.0 is actual value)
 * base_height: "Padding" below lowest height value. This will not be scaled by scale_pct. Must be > 0
 * units_per_pixel: x/y distance betyween points. These will not be scaled.
 */
stl_error_t
stl_from_heightmap_double(
	const double *vals,
	stl_origin_t origin,
	unsigned int cols,
	unsigned int rows,
	double scale_pct,
	double base_height,
	double units_per_pixel,
	stl_t **stl
	);

/* Print to stdout the elements of the STL object
 */
void stl_print(stl_t *stl);

/* Calculate and print some basic stats about the STL object
 */
void stl_print_stats(stl_t *stl);


#ifdef __cplusplus
}
#endif

#endif  /* _STL3D_LIB_H */
