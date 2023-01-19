#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define MEM_SIZE  1024

int main(int argc, char *argv[]){
  if (argc != 3) {
    fprintf(stderr, "Too many/Too less arguments\n");
    return 1;
  }

  char read_flag[4] = "9999";
  char buff_size[4];
  int buff_size_int;


  while(1){
    char* memseg_char;
    key_t key = ftok(argv[1], 'a');
    if(key < 0) {
        fprintf(stderr, "Error creating key\n");
        return 2;
    }
    int shmid = shmget(key, MEM_SIZE, 0666);
    if(shmid < 0) {
        fprintf(stderr, "Error getting shared memory\n");
        return 3;
    }
    void *memseg = shmat(shmid, NULL, 0);
    if(memseg == (void *)-1) {
        fprintf(stderr, "Error attaching shared memory segment\n");
        return 4;
    }
    memseg_char = (char *)memseg;
    //printf("%s\n", memseg_char);

    for(int i=0; i<4; i++){
      buff_size[i] = memseg_char[i+4];
    }
    buff_size_int = atoi(buff_size);
    char main_buff[buff_size_int+1];


    for(int i=8; i<buff_size_int+8; i++){
      main_buff[i-8] = memseg_char[i];
    }
	main_buff[buff_size_int] = '\0';
    printf("%s\n", main_buff);

    for(int i=0; i<4; i++){
      memseg_char[i] = read_flag[i];
    }
    strcpy(memseg, memseg_char);



	FILE *file = fopen(argv[2], "a");

	int results = fputs(main_buff, file);
	if (results == EOF) {
    	fprintf(stderr, "Failed to write to the file.\n");
	}
	fclose(file);




    if(shmdt(memseg) == -1) {
        fprintf(stderr, "Error detaching shared memory segment\n");
        return 5;
    }
    sleep(1);
  }

  return 0;
}
