#include<stdio.h>
#include<iostream>
#include"App.h"

int main(int argc, char *argv[]){
  App app(argc, argv);
  app.init();
  app.run();
  return 0;
}

