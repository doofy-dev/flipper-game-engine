//
// Doofy's Math library
//

#pragma once

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
//#define abs(x) ((x)>0?(x):-(x))

typedef struct {
    float x;
    float y;
} Vector;

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
 * @param start Start vector
 * @param end   End vector
 * @param t     Time (0-1 range)
 * @return      2d Vector between start and end at time
 */
Vector lerp_2d(Vector start, Vector end, float t);

/**
 * Quadratic lerp function
 *
 * @param start     Start vector
 * @param control   Control point
 * @param end       End vector
 * @param t         Time (0-1 range)
 * @return          2d Vector at time
 */
Vector quadratic_2d(Vector start, Vector control, Vector end, float t);

/**
 * Add vector components together
 *
 * @param a     First vector
 * @param b     Second vector
 * @return      Resulting vector
 */
Vector vector_add(Vector a, Vector b);

/**
 * Subtract vector components together
 *
 * @param a First vector
 * @param b Second vector
 * @return  Resulting vector
 */
Vector vector_sub(Vector a, Vector b);

/**
 * Multiplying vector components together
 *
 * @param a First vector
 * @param b Second vector
 * @return  Resulting vector
 */
Vector vector_mul_components(Vector a, Vector b);

/**
 * Dividing vector components
 *
 * @param a First vector
 * @param b Second vector
 * @return  Resulting vector
 */
Vector vector_div_components(Vector a, Vector b);

/**
 * Rotate vector
 *
 * @param a Vector
 * @param degrees Degrees
 * @return  Resulting vector
 */
Vector vector_rotate(Vector a, float degrees);

/**
 * Calculating Vector length
 *
 * @param a Direction vector
 * @return  Length of the vector
 */
float vector_magnitude(Vector a);

/**
 * Get a normalized vector (length of 1)
 *
 * @param a Direction vector
 * @return  Normalized vector
 */
Vector vector_normalized(Vector a);

/**
 * Calculate two vector's distance
 *
 * @param a First vector
 * @param b Second vector
 * @return  Distance between vectors
 */
float vector_distance(Vector a, Vector b);

/**
 * Calculate the dot product of the vectors.
 * No need to normalize, it will do it
 *
 * @param a First vector
 * @param b Second vector
 * @return  value from -1 to 1
 */
float vector_dot(Vector a, Vector b);


Matrix identity_matrix();

Matrix scale_matrix(const Vector *scale);

Matrix translation_matrix(const Vector *pos);

Matrix rotation_matrix(float angle);

Matrix matrix_multiply(const Matrix *a, const Matrix *b);

Vector get_matrix_translation(const Matrix *m);

Vector matrix_mul_vector(const Matrix *a, const Vector *v);