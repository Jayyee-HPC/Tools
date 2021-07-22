#ifndef __GSJ_GLOBAL_VAR_H_INCLUDE__
#define __GSJ_GLOBAL_VAR_H_INCLUDE__

/* Compiler instructions*/
#define USE_UNSTABLE_GEOS_CPP_API
#define DEBUG
#define NUMA_ENABLED
//#define USE_ST_INTERSECTION 1
//#define USE_ST_INTERSECTS 1
#define USE_ST_UNION 1

/* Global Variables */
#define WORKER_PREPARING 0
#define LOCAL_TASKS_PUSHED 1
#define LOCAL_TASKS_DONE 2
#define INIT_VICTIM -1
#define NO_MORE_VICTIM -2
#define ELEM_PER_PROC 2
#define ITERATIONS_PER_CHECK_JOB_REQUEST 100
#define NUM_JOBS_TO_MARK_FINISHED 128
#define NUM_JOBS_TO_SEND 128
#define ALL_LOCAL_JOB_NOT_COMPLETED 0xFFFF0001
#define STARVE_PROCESS_WAITING 0xFFFF0002
#define ALL_LOCAL_JOB_COMPLETED 0xFFFF0003
#define THRESH_HOLD_FOR_CHECK_JOB_REQUEST 100
#define SEND_BUF_0_SIZE 4
#define NUM_THIVES 35
#define NUM_THREADS 36
#define TASKS_PER_JOB 20

/* Types */
typedef unsigned int uint;
typedef unsigned long ulong;

#endif //ndef __GSJ_GLOBAL_VAR_H_INCLUDE__
