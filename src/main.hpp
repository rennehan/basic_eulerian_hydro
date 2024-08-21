#ifndef MAIN_HPP
#define MAIN_HPP

#ifdef WITH_DOUBLE
typedef double real;
#else
typedef float real;
#endif

#ifdef WITH_BOX_INT32
typedef int32_t box_int;
#else
typedef int64_t box_int;
#endif

#define MAX_TIME 0

#ifndef DIMENSION
#define DIMENSION 2
#endif

#ifndef N_CELLS_1D
#define N_CELLS_1D 5
#endif

#endif /* MAIN_HPP */
