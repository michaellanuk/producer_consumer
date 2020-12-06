/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

using namespace std;

#define INCORRECT_NUMBER_OF_PARAMETERS   1
#define INVALID_COMMAND_LINE_ARGUMENT    2

const int NUMBER_OF_SEMAPHORES = 3;
const int MAX_TIME_TO_ADD_JOBS = 5;
const int MAX_TIME_BETWEEN_JOBS = 10;
const int MAX_WAITING_TIME = 20;
const int MUTEX = 0;
const int FULL = 1;
const int EMPTY = 2;

typedef struct _job_t {
  int id;
  int duration;
} Job;

typedef struct _queue_t {
  int head;
  int tail;
  int size;
  Job* job_ptr;
} Queue;

void *producer (void *id);
void *consumer (void *id);
void push(Job new_job);
Job get();

// Producers and consumers need to access below global variables
int num_jobs, queue_size, sem_id;
Queue queue;

int main (int argc, char **argv) {

  // Read in and check validity of command line arguments
  if (argc != 5) {
    cerr << "Incorrect number of parameters passed to the command line" << endl;
    return INCORRECT_NUMBER_OF_PARAMETERS;
  }

  queue_size = check_arg(argv[1]);
  if (queue_size < 0) {
    cerr << "Invalid value for size of queue" << endl;
    return INVALID_COMMAND_LINE_ARGUMENT;
  }
  num_jobs = check_arg(argv[2]);
  if (num_jobs < 0) {
    cerr << "Invalid value for number of jobs" << endl;
    return INVALID_COMMAND_LINE_ARGUMENT;
  }
  int num_producers = check_arg(argv[3]);
  if (num_producers < 0) {
    cerr << "Invalid value for number of producers" << endl;
    return INVALID_COMMAND_LINE_ARGUMENT;
  }
  int num_consumers = check_arg(argv[4]);
  if (num_consumers < 0) {
    cerr << "Invalid value for number of consumers" << endl;
    return INVALID_COMMAND_LINE_ARGUMENT;
  }

  // Set up and initialise required data structures
  queue.head = 0;
  queue.tail = 0;
  queue.size = queue_size;
  queue.job_ptr = new Job[queue_size];

  // Set up and initialise semaphores
  sem_id = sem_create(SEM_KEY, NUMBER_OF_SEMAPHORES);

  sem_init(sem_id, MUTEX, 1);
  sem_init(sem_id, FULL, 0);
  sem_init(sem_id, EMPTY, queue_size);

  // Create required producers and consumers
  pthread_t producers[num_producers];
  pthread_t consumers[num_consumers];

  // Initialise producer and consumer objects
  for (int i = 0; i < num_producers; i++) {
    pthread_create(&producers[i], NULL, producer, (void *) (intptr_t) (i + 1));
  }

  for (int i = 0; i < num_consumers; i++) {
    pthread_create(&consumers[i], NULL, consumer, (void *) (intptr_t) (i + 1));
  }

  // Wait for each producer and consumer to finish
  for (int i = 0; i < num_producers; i++) {
    pthread_join(producers[i], NULL);
  }

  for (int i = 0; i < num_consumers; i++) {
    pthread_join(consumers[i], NULL);
  }

  // Quit and clean up

  sem_close(sem_id);

  delete [] queue.job_ptr;

  return 0;
}

void *producer (void *id) {
  int producer_id = (intptr_t) id;
  Job current_job;

  for (int i = 0; i < num_jobs; i++) {
    int duration = rand() % MAX_TIME_BETWEEN_JOBS + 1;

    sleep(rand() % MAX_TIME_TO_ADD_JOBS + 1);

    if (sem_timedwait(sem_id, EMPTY, MAX_WAITING_TIME)) {
      cout << "Producer timed out after 20 seconds of waiting." << endl;      
    }

    sem_wait(sem_id, MUTEX);

    /*--------------ENTERING-CRITICAL-SECTION--------------*/

    current_job.id = (queue.tail + 1);
    current_job.duration = duration;

    push(current_job);

    /*--------------LEAVING-CRITICAL-SECTION--------------*/

    sem_signal(sem_id, MUTEX);
    sem_signal(sem_id, FULL);

    cout << "Producer (" << producer_id << "): Job id " << current_job.id 
    << " duration " << current_job.duration << endl;
  }

  cout << "Producer (" << producer_id << "): No more jobs left to generate" << endl;

  pthread_exit(0);
}

void *consumer (void *id) {
  int consumer_id = (intptr_t) id;
  Job current_job;

  while (sem_timedwait(sem_id, FULL, MAX_WAITING_TIME) == 0) {
    
    sem_wait(sem_id, MUTEX);

    /*--------------ENTERING-CRITICAL-SECTION--------------*/

    current_job = get();

    /*--------------LEAVING-CRITICAL-SECTION--------------*/

    sem_signal(sem_id, MUTEX);
    sem_signal(sem_id, EMPTY);

    cout << "Consumer (" << consumer_id << "): Job id " << current_job.id
    << " executing sleep duration " << current_job.duration << endl;

    sleep(current_job.duration);

    cout << "Consumer (" << consumer_id << "): Job id " << current_job.id 
    << " completed" << endl;
  }

  cout << "Consumer (" << consumer_id << "): No more jobs left" << endl;

  pthread_exit(0);
}

void push(Job new_job) {
  queue.job_ptr[queue.tail] = new_job;
  queue.tail = ((queue.tail + 1) % queue_size);
}

Job get() {
  Job fetched_job = queue.job_ptr[queue.head];
  queue.head = ((queue.head + 1) % queue.size);

  return fetched_job;
}