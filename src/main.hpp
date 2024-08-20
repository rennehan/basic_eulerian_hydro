#ifndef MAIN_HPP
#define MAIN_HPP

#ifdef WITH_DOUBLE
typedef double real;
#else
typedef float real;
#endif

#ifdef WITH_BOX_INT32
typedef uint32_t box_int;
#else
typedef uint64_t box_int;
#endif

#define MAX_TIME 1.0

#ifndef DIMENSION
#define DIMENSION 2
#endif

#ifndef N_CELLS_1D
#define N_CELLS_1D 6
#endif

#endif /* MAIN_HPP */
