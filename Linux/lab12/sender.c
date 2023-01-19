#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MEM_SIZE  1024


int main(int argc, char *argv[]){
  if (argc != 3) {
    fprintf(stderr, "Too many/Too less arguments\n");
    return 1;
  }

  key_t key = ftok(argv[1], 'a');
  int fdi = open(argv[2], O_RDONLY);
  struct stat stat;
  fstat(fdi, &stat);

  char buff[MEM_SIZE];
  char temp_buff[MEM_SIZE-8];
  char read_flag[4];
  char buff_size[4];
  int rdin;



  while(1) {
    int shmid = shmget(key, MEM_SIZE, 0666 | IPC_CREAT | IPC_EXCL);
    if(shmid < 0) {
        fprintf(stderr, "Error getting shared memory\n");
        return 3;
    }
    void *memseg = shmat(shmid, NULL, 0);
    if(memseg == (void *)-1) {
        fprintf(stderr, "Error attaching shared memory segment\n");
        return 4;
    }

    rdin = read(fdi, temp_buff, MEM_SIZE-8);
    int temp_rdin = rdin;

    if (rdin == 0){
		if(shmdt(memseg) == -1) {
		    fprintf(stderr, "Error detaching shared memory segment\n");
		    return 5;
		}
		if(shmctl (shmid, IPC_RMID, NULL) == -1) {
		    fprintf(stderr, "Error removing sharem memory\n");
		    return 6;
		}
      break;
    }


    /* ======== NIE RUSZAC! ========= */
    /* ==== TU DZIEJE SIE MAGIA. ==== */
    int buff_size_size = 0;

    while(temp_rdin != 0){
      temp_rdin=temp_rdin/10;
      buff_size_size++;
    }

    char temp_buff_size[4];
    sprintf(temp_buff_size, "%d", rdin);
    sprintf(read_flag, "%d", 1111);


    if(buff_size_size < 4){
      int n = 4 - buff_size_size;

      for(int i = 0; i < 4; i++){
        if(buff_size_size + i < 4){
          buff_size[i] = '0';
        } else {
          buff_size[i] = temp_buff_size[i-n];
        }
      }
    } else {
      for(int i = 0; i < 4; i++){
        buff_size[i] = temp_buff_size[i];
      }
    }

    for(int i = 0; i < 4; i++){
      buff[i] = read_flag[i];
      buff[i+4] = buff_size[i];
    }
    for(int i = 0; i < rdin; i++){
      buff[i+8] = temp_buff[i];
    }
    /* ======= KONIEC MAGII. ======= */



    printf("%s\n", (char *)buff);
    strcpy(memseg, buff);
    while(1){
      sleep(1);
      char* memseg2_char;
      key_t key = ftok(argv[1], 'a');
      shmid = shmget(key, MEM_SIZE, 0666);
      void *memseg2 = shmat(shmid, NULL, 0);
      memseg2_char = (char *)memseg2;
      if(memseg2_char[0] == '9') break;
    }



    //  Czyszczenie
    memset(temp_buff, 0, sizeof temp_buff);
    memset(buff, 0, sizeof buff);
    if(shmdt(memseg) == -1) {
        fprintf(stderr, "Error detaching shared memory segment\n");
        return 5;
    }
    if(shmctl (shmid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Error removing sharem memory\n");
        return 6;
    }
  }


  close(fdi);
  return 0;
}
