/**
 *  \file math_utils.h
 *
 */
#ifndef GAMMOU_BASE_MATH_UTILS_H_
#define GAMMOU_BASE_MATH_UTILS_H_

/**
 *  \brief a 9-th order taylor approximation of the sin function
 *  \param x a real in [-PI, PI]
 *  \note
 */
float fast_sin(float x);

/**
 *
 */
float fast_tanh(float x);

float clamp(float x, float min, float max);
float exclude_zero(float x, float epsilon);

float absolute(float x);
float copy_sign(float from, float to);

/**
 * \brief Linear congruential generator step
 */
unsigned int linear_congruential_step(unsigned int last);

#endif