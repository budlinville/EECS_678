/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t {
	int jobNumber;
	int arrivalTime;
	int runTime;		//burst time
	int priority;
	int startTime;		//first scheduled
	int eventTime;		//anytime anything happens -get scheduled, gets preempted,
	bool scheduled;
	bool completed;
	bool possiblyScheduled;		//for attempting to preempt an already completed job
								//false if we know FOR SURE if it ran or did not run
								//just used for PREEMPTION and finished
} job_t;

/*
typedef struct _core_t {
	job_t runningJob;
	int coreID;
	bool idle;
} core_t;
*/

//GLOBAL variables
int numCores = 0;
scheme_t curScheme = FCFS;
job_t **coreList = NULL;
int numJobs = 0;
int waitingTime = 0;
int turnaroundTime = 0;
int responseTime = 0;
priqueue_t priQueue;

//three compare functions
int priorityCompare(const void *this, const void *that) {
	//return -1 if this comes first
	//1 if that comes first
	job_t *thisJob = (job_t *) this;
	job_t *thatJob = (job_t *) that;

	if (thisJob->priority > thatJob->priority) {
		return -1;
	} else if (thisJob->priority < thatJob->priority) {
		return 1;
	}

	return 0;
}

int timeCompare(const void *this, const void *that) {
	//return -1 if this comes first
	//1 if that comes first
	job_t *thisJob = (job_t *) this;
	job_t *thatJob = (job_t *) that;

	if (thisJob->runTime > thatJob->runTime) {
		return 1;
	} else if (thisJob->runTime < thatJob->runTime) {
		return -1;
	}

	return 0;
}

//dont compare
int noCompare(const void *this, const void *that) {
	//Process *p_this = (Process *)this;
	//Process *p_that = (Process *)that;

	(void) this;
	(void) that;

	return 1;
}

/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme) {
	numCores = cores;
	curScheme = scheme;

	coreList = (job_t**)malloc(sizeof(job_t *) * (numCores));

	int i;
	for (i = 0; i < numCores; i++) {
		coreList[i] = NULL;
	}

	int (*comparer) (const void *, const void *);
	switch (curScheme) {
		case FCFS : comparer = noCompare; break;
		case SJF : comparer = timeCompare; break;
		case PSJF : comparer = timeCompare; break;
		case PRI : comparer = priorityCompare; break;	//prioirt
		case PPRI : comparer = priorityCompare; break;
		case RR : comparer = noCompare; break;
		default : printf("ERROR - NO SCHEME"); exit(-1);
	};

	priqueue_init(&priQueue, comparer);
}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

	int jobNumber;
	int arrivalTime;
	int runTime;		//burst time
	int priority;
	int startTime;		//first scheduled
	int eventTime;		//anytime anything happens -get scheduled, gets preempted,
	bool scheduled;
	bool completed;
	bool possiblyScheduled;
 */

 /*How do I calculate which core to use if all cores already have a job running*/
int scheduler_new_job(int job_number, int time, int running_time, int priority) {
	job_t *newJob = (job_t*)malloc(sizeof(job_t) * (numCores));
	newJob->jobNumber = job_number;
	newJob->arrivalTime = time;
	newJob->runTime = running_time;
	newJob->priority = priority;

	newJob->startTime = 0;
	newJob->eventTime = 0;
	newJob->scheduled = true;
	newJob->completed = false;
	newJob->possiblyScheduled = false;

	numJobs++;
	int coreToRunOn = 0;

	int i;
	for (i = 0; i < numCores; i++) {
		if (coreList[i] == NULL) {
			coreToRunOn = i;					//even if multiple idle cores, core with lowest id takes job
																//because i increments in ascending order

			coreList[i] = newJob;
			return coreToRunOn;
		} else {
			//stick on priority queue
			priqueue_offer(&priQueue, &newJob);

		}
	}

	//priqueue_offer(&priQueue, &newJob);

	return coreToRunOn;											//still dont know how to decide which core to run on.
}

/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time) {
	//not sure if priqueue_remove_at() is working properly
	job_t *newJob = priqueue_at(&priQueue, job_number);
	priqueue_remove(&priQueue, newJob);

	job_t *oldJob = coreList[core_id];


	//update waiting time
	waitingTime += (time - newJob->arrivalTime);

	//update turnaroundTime
	turnaroundTime += (time - oldJob->arrivalTime);

	//update responseTime
	newJob->startTime = time;
	responseTime += (newJob->arrivalTime - newJob->startTime);

	//job now completed
	newJob->completed = true;


	if (newJob != NULL)
	 	return newJob->jobNumber;

	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time) {
	job_t *newJob = priqueue_poll(&priQueue);
	job_t *oldJob = coreList[core_id];

	if (newJob == NULL)
		return -1;

	



	return newJob->jobNumber;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return ((double)waitingTime) / numJobs;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return ((double)turnaroundTime) / numJobs;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return ((double)responseTime) / numJobs;
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up() {
	priqueue_destroy(&priQueue);
	free(coreList);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue() {

}
