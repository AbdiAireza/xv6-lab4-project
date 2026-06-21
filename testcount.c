/*#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h" 

#define CHILDREN 4        
#define ITERATIONS 100000 

int main(int argc, char *argv[]) {
    int i, j;
    int pid;

    printf(1, "Starting race condition test with %d children, each calling getpid() %d times...\n", CHILDREN, ITERATIONS);

    
    for (i = 0; i < CHILDREN; i++) {
        pid = fork();
        
        if (pid < 0) {
            printf(1, "Fork failed!\n");
            exit();
        } 
        
        if (pid == 0) {
            sleep(1);
            
            for (j = 0; j < ITERATIONS; j++) {
                getpid();
            }
            exit(); 
        }
    }

    
    for (i = 0; i < CHILDREN; i++) {
        wait();
    }

    
    
    int actual_count = getcount(SYS_getpid); 
    int expected_count = CHILDREN * ITERATIONS;

    printf(1, "\n--- Test Results ---\n");
    printf(1, "Expected total calls: %d\n", expected_count);
    printf(1, "Actual counted calls: %d\n", actual_count);

    if (actual_count == expected_count) {
        printf(1, "Result: NO Race Condition (Counts match perfectly).\n");
    } else {
        printf(1, "Result: RACE CONDITION DETECTED! Lost %d updates.\n", expected_count - actual_count);
    }

    exit();
}

*/





#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

#define NCHILD 8
#define NITER 100000
#define NCPU_TEST 4

int
main(void)
{
  int before_total, after_total;
  int before_cpu[NCPU_TEST];
  int after_cpu[NCPU_TEST];
  int i, j, pid;
  int expected;
  int total_diff;
  int cpu_diff_sum = 0;

  before_total = getcount(SYS_getpid);

  for(i = 0; i < NCPU_TEST; i++)
    before_cpu[i] = getcpucount(i, SYS_getpid);

  for(i = 0; i < NCHILD; i++){
    pid = fork();

    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }

    if(pid == 0){
      for(j = 0; j < NITER; j++){
        getpid();
      }
      exit();
    }
  }

  for(i = 0; i < NCHILD; i++)
    wait();

  after_total = getcount(SYS_getpid);

  for(i = 0; i < NCPU_TEST; i++)
    after_cpu[i] = getcpucount(i, SYS_getpid);

  expected = NCHILD * NITER;
  total_diff = after_total - before_total;

  for(i = 0; i < NCPU_TEST; i++)
    cpu_diff_sum += after_cpu[i] - before_cpu[i];

  for(i = 0; i < NCPU_TEST; i++){
    printf(1, "CPU %d getpid count: %d\n",
           i, after_cpu[i] - before_cpu[i]);
  }

  printf(1, "Total getpid count: %d\n", total_diff);
  printf(1, "Expected count: %d\n", expected);

  if(total_diff == expected && cpu_diff_sum == expected)
    printf(1, "Per-CPU counters are correct\n");
  else
    printf(1, "Counter mismatch detected\n");

  exit();
}
