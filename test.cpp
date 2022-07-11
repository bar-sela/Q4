
#include "stack.hpp"
#include <iostream>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include<fcntl.h>

int main() {

    int file_desc = open("text.txt", O_WRONLY | O_APPEND);

   push("EDEN HAZARD");
   push("LEO MESSI");
   assert(strcmp(top(), "LEO MESSI") == 0);
   pop();
   assert(strcmp(top(), "EDEN HAZARD") == 0);
   pop();

    int f_org = 1;
    if (int first = fork() != 0) {
        system("./server");
        dup2(file_desc, 1);
    }

    if (fork() == 0) {
        f_org = 0;
        sleep(1);
        push("SHON BAR SELA");

    }
    if (fork() == 0) {
        f_org = 0;
        sleep(1);
        push("SHANI VAHAV");
    }
    if (fork() == 0) {
        f_org = 0;
        sleep(1);
        push("LEO MESSI");
    }
    for(int i = 0 ; i < 3 ; i ++) {
        int b = (strcmp(top(), "SHON BAR SELA"));
        int b2 = (strcmp(top(), "SHANI VAHAV"));
        int b3 = (strcmp(top(), "LEO MESSI"));
        int mul = b & b2;
        mul = mul & b3;
        if (f_org != 0) {
            assert(mul == 0);
        }
        pop();
    }
    assert(top() == NULL);
    exit(0);
}
