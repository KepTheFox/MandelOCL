#include <iostream>
#include <stdio.h>
#include "clFramework.h"

int main(int argc, char* argv[]){
    clInfo *cli = new clInfo;
    cli->printInfo();
    cli->loadKernelsFromFile("kernels.cl");
}