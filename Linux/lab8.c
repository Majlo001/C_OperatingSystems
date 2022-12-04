#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv){
  if (argc > 2 || argc == 1) {
    fprintf(stderr, "Too many arguments\n");
    return 1;
  }

  int n=strlen(argv[1]);
  for(int i=0; i<n; i++){
    if (!isdigit(argv[1][i])) {
      fprintf(stderr, "Argument is not integer\n");
      return 2;
    }
  }

  int number = atoi(argv[1]);
  if (number < 1 || number > 13) {
    fprintf(stderr, "Agument is not from interval <1,13>\n");
    return 3;
  }
  if (number <= 2) {
    return 1;
  }


  int f1 = fork();
  if (f1 == 0){
    char newnumber[4];
    snprintf(newnumber, 4, "%d", number-1);
    execlp("./a.out", "./a.out", newnumber, NULL);
    fprintf(stderr, "Exec failed: %s\n", strerror(errno));
    return 1;
  }
  if (f1 == -1){
    fprintf(stderr, "Fork failed: %s\n", strerror(errno));
  }


  int f2 = fork();
  if (f2 == 0){
    char newnumber[4];
    snprintf(newnumber, 4, "%d", number-2);
    execlp("./a.out", "./a.out", newnumber, NULL);
    fprintf(stderr, "Exec failed: %s\n", strerror(errno));
    return 1;
  }
  if (f2 == -1){
    fprintf(stderr, "Fork failed: %s\n", strerror(errno));
  }

  int status2, child2 = wait(&status2);
  int status1, child1 = wait(&status1);


  fprintf(stdout, "%d\t%d\t%d\t%d\n", getpid(), child1, number-1, WEXITSTATUS(status1));
  fprintf(stdout, "%d\t%d\t%d\t%d\n", getpid(), child2, number-2, WEXITSTATUS(status2));
  fprintf(stdout, "%d\t\t\t%d\n\n", getpid(), WEXITSTATUS(status1) + WEXITSTATUS(status2));

  return WEXITSTATUS(status1) + WEXITSTATUS(status2);
}
