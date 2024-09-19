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

#define ICS 1
#define MAX_TIME 1.0
#define DUMP_INTERVAL 0.1

#ifndef DIMENSION
#define DIMENSION 2
#endif

#ifndef N_CELLS_1D
#define N_CELLS_1D 64
#endif

#define DEBUG

#define GAMMA 5.0 / 3.0
#endif /* MAIN_HPP */
