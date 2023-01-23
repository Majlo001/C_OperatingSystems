#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define MEM_SIZE 1024

void printBoard(char* recive_buff) {
    char row[3];
    for (int i = 0; i < 3; i++) {
        if (recive_buff[i + 1] == 'X') row[i] = 'X';
        else if (recive_buff[i + 1] == 'O') row[i] = 'O';
        else row[i] = ' ';
    }
    printf(" %c | %c | %c \n", row[0], row[1], row[2]);
    printf("---+---+---\n");

    for (int i = 0; i < 3; i++) {
        if (recive_buff[i + 4] == 'X') row[i] = 'X';
        else if (recive_buff[i + 4] == 'O') row[i] = 'O';
        else row[i] = ' ';
    }
    printf(" %c | %c | %c \n", row[0], row[1], row[2]);
    printf("---+---+---\n");

    for (int i = 0; i < 3; i++) {
        if (recive_buff[i + 7] == 'X') row[i] = 'X';
        else if (recive_buff[i + 7] == 'O') row[i] = 'O';
        else row[i] = ' ';
    }
    printf(" %c | %c | %c \n", row[0], row[1], row[2]);
}

int checkWin(char* recive_buff) {
    if (recive_buff[1] == 'X' && recive_buff[5] == 'X' && recive_buff[9] == 'X') {
        printf("Player X won!\n");
        return 1;
    }
    if (recive_buff[3] == 'X' && recive_buff[5] == 'X' && recive_buff[7] == 'X') {
        printf("Player X won!\n");
        return 1;
    }
    if (recive_buff[1] == 'O' && recive_buff[5] == 'O' && recive_buff[9] == 'O') {
        printf("Player O won!\n");
        return 2;
    }
    if (recive_buff[3] == 'O' && recive_buff[5] == 'O' && recive_buff[7] == 'O') {
        printf("Player O won!\n");
        return 2;
    }

    for (int i = 0; i < 3; i++) {
        if (recive_buff[i * 3 + 1] == 'X' && recive_buff[i * 3 + 2] == 'X' && recive_buff[i * 3 + 3] == 'X') {
            printf("Player X won!\n");
            return 1;
        }
        if (recive_buff[i + 1] == 'X' && recive_buff[i + 4] == 'X' && recive_buff[i + 7] == 'X') {
            printf("Player X won!\n");
            return 1;
        }
        if (recive_buff[i * 3 + 1] == 'O' && recive_buff[i * 3 + 2] == 'O' && recive_buff[i * 3 + 3] == 'O') {
            printf("Player O won!\n");
            return 2;
        }
        if (recive_buff[i + 1] == 'O' && recive_buff[i + 4] == 'O' && recive_buff[i + 7] == 'O') {
            printf("Player O won!\n");
            return 2;
        }
    }


    for (int i = 1; i < 11; i++) {
        if (recive_buff[i] == '0') {
            return 0;
        }
    }

    printf("It's a draw!\n");
    return 1;
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Too many/Too less arguments\n");
        return 1;
    }

    HANDLE MapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, argv[1]);

    char flag = '0';
    char board[9] = "000000000";
    char buff_array[MEM_SIZE];
    char* recive_buff;
    int position;

    if (MapFile == NULL) {
        printf("You are playing as a player X.\n");

        MapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MEM_SIZE, argv[1]);
        if (MapFile == NULL){
            fprintf(stderr, "Could not create file mapping object\n");
            return 2;
        }

        void* buff = MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MEM_SIZE);
        if (buff == NULL){
            fprintf(stderr, "Could not map view of file\n");
            CloseHandle(MapFile);
            return 3;
        }
        buff_array[0] = flag;
        for (int i = 1; i <= 10; i++) {
            buff_array[i] = board[i - 1];
        }
        buff_array[11] = '\0';



        while (1) {
            if (checkWin(buff_array) == 1 || checkWin(buff_array) == 2) {
                sleep(1);
                break;
            }
            while (1) {
                printf("Give position (1-9) to put X:");
                scanf("%d", &position);
                printf("\n");
                if (position < 10 && position > 0 && buff_array[position] != 'X' && buff_array[position] != 'O') {
                    buff_array[position] = 'X';
                    buff_array[0] = '1';
                    printf("%s\n", buff_array);
                    strcpy(buff, buff_array);
                    printBoard(buff_array);
                    printf("\n");
                    break;
                }
            }
            if (checkWin(buff_array) == 1 || checkWin(buff_array) == 2) {
                sleep(1);
                break;
            }

            while (1) {
                sleep(1);
                recive_buff = buff;
                if (recive_buff[0] == '9') {
                    printBoard(recive_buff);
                    printf("\n");
                    break;
                }
            }

            for (int i = 0; i < 11; i++) {
                buff_array[i] = recive_buff[i];
            }
        }

        if (!UnmapViewOfFile(buff)) {
            fprintf(stderr, "Could not unmap view of file\n");
            CloseHandle(MapFile);
            return 4;
        }
        CloseHandle(MapFile);
    }
    else {
        printf("You are playing as a player O.\n");

        void* buff = MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MEM_SIZE);
        if (buff == NULL) {
            fprintf(stderr, "Could not map view of file\n");
            CloseHandle(MapFile);
            return 2;
        }


        while (1) {
            while (1) {
                sleep(1);
                buff = MapViewOfFile(MapFile, FILE_MAP_ALL_ACCESS, 0, 0, MEM_SIZE);
                recive_buff = buff;
                if (recive_buff[0] == '1') {
                    printBoard(recive_buff);
                    printf("\n");
                    break;
                }
            }

            for (int i = 0; i < 11; i++) {
                buff_array[i] = recive_buff[i];
            }
            if (checkWin(buff_array) == 1 || checkWin(buff_array) == 2) {
                break;
            }

            while (1) {
                printf("Give position (1-9) to put O:");
                scanf("%d", &position);
                printf("\n");
                if (position < 10 && position > 0 && buff_array[position] != 'X' && buff_array[position] != 'O') {
                    buff_array[position] = 'O';
                    buff_array[0] = '9';
                    printf("%s\n", buff_array);
                    strcpy(buff, buff_array);
                    printBoard(buff_array);
                    printf("\n");
                    break;
                }
            }
            if (checkWin(buff_array) == 1 || checkWin(buff_array) == 2) {
                break;
            }
        }
    }
    return 0;
}
