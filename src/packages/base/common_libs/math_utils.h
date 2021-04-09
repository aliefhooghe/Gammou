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

float absolute(float x);

#endif