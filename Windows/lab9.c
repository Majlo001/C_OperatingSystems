// SO IS1 220B LAB09
// Miłosz Szczepanik
// sm51115@zut.edu.pl

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv){
	//printf("argv1: %s\n", argv[1]);
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


	  STARTUPINFO sInfo;
	  PROCESS_INFORMATION pInfo[2];
	  memset(&sInfo, 0, sizeof(sInfo));
	  memset(&pInfo, 0, sizeof(pInfo));
	  sInfo.cb = sizeof(sInfo);
	  char arg[50];
	  
	  
	  sprintf(arg, "lab9.exe %d",number-1);
	  if (CreateProcessA(NULL, arg, NULL, NULL, 0, 0, NULL, NULL, &sInfo, pInfo)){
		printf("hProcess: %p   dwProcessId: %d\n", pInfo[0].hProcess, pInfo[0].dwProcessId);
	  }
	  
	  sprintf(arg, "lab9.exe %d",number-2);
	  if (CreateProcessA(NULL, arg, NULL, NULL, 0, 0, NULL, NULL, &sInfo, pInfo+1)){
		printf("hProcess: %p   dwProcessId: %d\n", pInfo[1].hProcess, pInfo[1].dwProcessId);
	  }
  
  
	int child2 = GetProcessId(pInfo[1].hProcess);
	int child1 = GetProcessId(pInfo[0].hProcess);
  
	DWORD status1 = 0;
	DWORD status2 = 0;
  
	WaitForSingleObject(pInfo[1].hProcess, INFINITE);
	WaitForSingleObject(pInfo[0].hProcess, INFINITE);
	
	GetExitCodeProcess(pInfo[1].hProcess, &status2);
	GetExitCodeProcess(pInfo[0].hProcess, &status1);
	
	
	CloseHandle(pInfo[1].hProcess);
	CloseHandle(pInfo[1].hThread);
	CloseHandle(pInfo[0].hProcess);
	CloseHandle(pInfo[0].hThread);
  
	//fprintf(stdout, "Status1: %d\n", status1);
	//fprintf(stdout, "Status2: %d\n", status2);


	fprintf(stdout, "%d\t%d\t%d\t%d\n", GetCurrentProcessId(), child1, number-1, status1);
	fprintf(stdout, "%d\t%d\t%d\t%d\n", GetCurrentProcessId(), child2, number-2, status2);
	fprintf(stdout, "%d\t\t\t%d\n\n", GetCurrentProcessId(), status1 + status2);
  
	return status1 + status2;
}