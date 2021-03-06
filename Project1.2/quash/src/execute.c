/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"

#include <sys/wait.h>

#include <stdio.h>

#include "quash.h"

#include <signal.h>

// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)
  
  
 IMPLEMENT_DEQUE_STRUCT(PIDDeque, pid_t);
 IMPLEMENT_DEQUE(PIDDeque, pid_t);

 typedef struct Job {
	 int job_id;
	 char* cmd;
	 
	 PIDDeque pid_list;
 } Job;
 
 static Job __new_job() {
	 return (Job) {
		 0, 
		 get_command_string(), 
		 new_PIDDeque(1),
	 };
 }
 
 
 IMPLEMENT_DEQUE_STRUCT(JobDeque, Job);
 IMPLEMENT_DEQUE(JobDeque, Job);
 
 JobDeque jobs;
 
 typedef struct ExecEnv {
	 int num;
	 
	 int fpd[2][2];
	 
	 Job job;
 } ExecEnv;
 
 static void __destroy_job(Job job) {
	 if (job.cmd != NULL) {
		 free(job.cmd);
	 }
	 destroy_PIDDeque(&job.pid_list);
 }
 
 
 static void printJobsHelper(Job j) {
	 print_job(j.job_id, peek_front_PIDDeque(&j.pid_list), j.cmd);
 }
 
 
/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  

  // Change this to true if necessary
  *should_free = true;

  return getcwd(NULL, 0);
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  

  // TODO: Remove warning silencers

  return getenv(env_var);
}

#define PIPE_NEXT(exec_env)			(exec_env).fpd[(exec_env).num % 2]
#define PIPE_END(exec_env, proc, end) (exec_env).fpd[((exec_env).num - (proc)) % 2][end]


// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  if (is_empty_JobDeque(&jobs)) {
	  return;
  } 
  
  size_t jobsLength = length_JobDeque(&jobs);
  
  for (size_t i = 0; i < jobsLength; i++){
	  Job thisJob = pop_front_JobDeque(&jobs);
	  
	  size_t pidLength = length_PIDDeque(&thisJob.pid_list);
	  
	  pid_t front = peek_front_PIDDeque(&thisJob.pid_list);
	  for (size_t j = 0; j < pidLength; j++) {
		  pid_t thisPID = pop_front_PIDDeque(&thisJob.pid_list);
		  
		  pid_t wait;
		  
		  int status;
		  
		  wait = waitpid(thisPID, &status, WNOHANG);
		  
		  if (wait == 0) {
			  //child
			  push_back_PIDDeque(&thisJob.pid_list, thisPID);
		  } else {
			  
			  assert(wait == thisPID);
		  }
	  }
	  
	  if (is_empty_PIDDeque(&thisJob.pid_list)) {
		  //all processes completer => job complete
		  print_job_bg_complete(thisJob.job_id, front, thisJob.cmd);
		  __destroy_job(thisJob);
	  } else {
		  push_back_JobDeque(&jobs, thisJob);
	  } 
  }

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  execvp(exec, args);

  perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  char** str = cmd.args;

  // TODO: Implement echo
  while((*str != NULL) && (*(str + 1) != NULL)) {
	  printf("%s ", *str );
	  str++;
  }
  
  if (*str != NULL) {
	  printf("%s\n", *str);
  }

  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Implement export.
  // HINT: This should be quite simple.
  setenv(env_var, val, 1);
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;

  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }

  // TODO: Change directory

  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
  chdir(dir);

  setenv("OLD PWD", lookup_env("PWD"), 1);
  setenv("PWD", dir, 1);
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  //(void) signal; // Silence unused variable warning
  //(void) job_id; // Silence unused variable warning

  // TODO: Kill all processes associated with a background job
  size_t length = length_JobDeque(&jobs);
  
  for (size_t i = 0; i < length; i++) {
	  Job thisJob = pop_front_JobDeque(&jobs);
	  size_t lengthP = length_PIDDeque(&thisJob.pid_list);
	  
	  if (job_id == thisJob.job_id) {
		  for (size_t j = 0; j < lengthP; j++) {
			  pid_t thisProcess = pop_front_PIDDeque(&thisJob.pid_list);
			  kill(thisProcess, signal);
			  push_back_PIDDeque(&thisJob.pid_list, thisProcess);
		  }
	  }
	  
	  push_back_JobDeque(&jobs, thisJob);
  }
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  bool shouldFree = false;
  
  char* cwd = get_current_directory(&shouldFree);
  
  fprintf(stdout, "%s\n", cwd);

  // Flush the buffer before returning
  fflush(stdout);
  
  if (shouldFree) {
	  free(cwd);
  }
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  apply_JobDeque(&jobs, printJobsHelper);

  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, ExecEnv* exec_env) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true
											   
											   
											   
(void) r_in;
(void) r_out;
(void) r_app;  

  if (p_out) {
	  pipe(PIPE_NEXT(*exec_env));
  }											
											   
  // TODO: Setup pipes, redirects, and new process
  pid_t pid = fork();
  if (pid == 0) {
	  //child
	  if (p_in) {
		  close(PIPE_END(*exec_env, 1, 1));
		  dup2(PIPE_END(*exec_env, 1, 0), STDIN_FILENO);
		  close(PIPE_END(*exec_env, 1, 0));
	  }
	  
	  if (p_out) {
		  close(PIPE_END(*exec_env, 0, 0));
		  dup2(PIPE_END(*exec_env, 0, 1), STDOUT_FILENO);
		  close(PIPE_END(*exec_env, 0, 1));
	  }
	  
	  
	  
	  
	  child_run_command(holder.cmd); // This should be done in the child branch of a fork
	  exit(EXIT_SUCCESS);
	  
  } else if (pid > 0) {
	  //parent
	  
	  push_back_PIDDeque(&exec_env->job.pid_list, pid);
	  
	  
	  parent_run_command(holder.cmd); // This should be done in the parent branch of
										// a fork
  } else {
	  exit(EXIT_FAILURE);
  }
  if (p_in) {
	  close (PIPE_END(*exec_env, 0, 0));
	  close (PIPE_END(*exec_env, 0, 1));
  }
  
  ++exec_env->num;
}

// Run a list of commands
void run_script(CommandHolder* holders) {
	static bool first = true;
	
  if (holders == NULL)
    return;

  if (first) {
	  jobs = new_destructable_JobDeque(2, __destroy_job);
	  first = false;
  }

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;
  ExecEnv exec_env;
  
  exec_env.num = 0;
  exec_env.job = __new_job();

  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i], &exec_env);

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    while (!is_empty_PIDDeque(&exec_env.job.pid_list)) {
		int status = 0;
		
		if (waitpid(pop_front_PIDDeque(&exec_env.job.pid_list), &status, 0) == -1) {
			perror("ERROR: Unexpected waitpid return");
			exit(EXIT_FAILURE);
		} 
	}
	
	__destroy_job(exec_env.job);	//
  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    if (is_empty_JobDeque(&jobs))
		exec_env.job.job_id = 1;
	else 
		exec_env.job.job_id = peek_back_JobDeque(&jobs).job_id + 1;
	
	push_back_JobDeque(&jobs, exec_env.job);

    // TODO: Once jobs are implemented, uncomment and fill the following line
    print_job_bg_start(exec_env.job.job_id, peek_front_PIDDeque(&exec_env.job.pid_list), exec_env.job.cmd);
  }
}
