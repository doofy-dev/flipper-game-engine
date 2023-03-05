//
// Doofy's Math library
//

#pragma once
#include <furi.h>
#include <math.h>

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))
#define l_abs(x) ((x) < 0 ? -(x) : (x))
//#define M_PI 3.141592653589793238462643383279502884
//#define M_PI_2 1.57079632679489661923

typedef struct {
    float x;
    float y;
} Vector;

#define VECTOR_ZERO (Vector){0,0}

typedef struct {
    float data[9];
} Matrix;

/**
 * Lerp function
 *
 * @param v0    Start value
 * @param v1    End value
 * @param t     Time (0-1 range)
 * @return      Point between v0-v1 at a given time
 */
float lerp(float v0, float v1, float t);

/**
 * 2D lerp function
 *
 * @param dst   Where to store, 2d Vector between start and end at time
 * @param start Start vector
 * @param end   End vector
 * @param t     Time (0-1 range)
 */
void lerp_2d(const Vector *const start, const Vector *const end, float t, Vector *dst);

/**
 * Quadratic lerp function
 *
 * @param dst   Where to store, 2d Vector at time
 * @param start     Start vector
 * @param control   Control point
 * @param end       End vector
 * @param t         Time (0-1 range)
 */
void
quadratic_2d(const Vector *const start, const Vector *const control, const Vector *const end, float t, Vector *dst);

/**
 * Add vector components together
 *
 * @param dst   Where to store, Resulting vector
 * @param a     First vector
 * @param b     Second vector
 */
void vector_add(const Vector *const a, const Vector *const b, Vector *dst);

/**
 * Subtract vector components together
 *
 * @param dst   Where to store, Resulting vector
 * @param a First vector
 * @param b Second vector
 */
void vector_sub(const Vector *const a, const Vector *const b, Vector *dst);

/**
 * Multiplying vector components together
 *
 * @param dst   Where to store, Resulting vector
 * @param a First vector
 * @param b Second vector
 */
void vector_mul_components(const Vector *const a, const Vector *const b, Vector *dst);

void vector_mul(const Vector *const a, float amount, Vector *dst);

/**
 * Dividing vector components
 *
 * @param dst   Where to store, Resulting vector
 * @param a First vector
 * @param b Second vector
 */
void vector_div_components(const Vector *const a, const Vector *const b, Vector *dst);

void vector_div(const Vector *const a, float b, Vector *dst);

/**
 * Rotate vector
 *
 * @param dst   Where to store, Resulting vector
 * @param a Vector
 * @param degrees Degrees
 */
void vector_rotate(const Vector *const a, float degrees, Vector *dst);


void vector_perpendicular(const Vector *const a, Vector *dst);

float vector_cross(const Vector *const a, const Vector *b);

/**
 * Calculating Vector length
 *
 * @param a Direction vector
 * @param dst   Where to store, Resulting vector
*/
float vector_magnitude(const Vector *const a);

/**
 * Get a normalized vector (length of 1)
 *
 * @param a Direction vector
 * @param dst   Where to store, Resulting vector
*/
void vector_normalized(const Vector *const a, Vector *dst);

/**
 * Calculate two vector's distance
 *
 * @param a First vector
 * @param b Second vector
 * @return  Distance between vectors
 */
float vector_distance(const Vector *const a, const Vector *b);

/**
 * Calculate the normalized dot product of the vectors.
 * No need to normalize, it will do it
 *
 * @param a First vector
 * @param b Second vector
 * @return  value from -1 to 1
 */
float vector_dot_normalized(const Vector *const a, const Vector *b);

float vector_dot(const Vector *const a, const Vector *b);

bool vector_project(const Vector *const lineA, const Vector *const lineB, const Vector *const point, Vector *dst);

void identity_matrix(Matrix *m);

void scale_matrix(float scale, Matrix *m);

void translation_matrix(const Vector *const pos, Matrix *m);

void rotation_matrix(float angle, Matrix *m);

void matrix_multiply(const Matrix *const a, const Matrix *const b, Matrix *dst);

void matrix_mul_vector(const Matrix *const a, const Vector *const v, Vector *dst);

void get_matrix_translation(const Matrix *const m, Vector *dst);

void get_matrix_scale(const Matrix *const m, Vector *dst);

float get_matrix_rotation(const Matrix *const m);
