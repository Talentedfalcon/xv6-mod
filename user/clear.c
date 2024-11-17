#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void clear(){
    printf("\033[H\033[J");
}

int main(int argc,char** argv){
    clear();
}