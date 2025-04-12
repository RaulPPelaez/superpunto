#include<stdio.h>
#include<iostream>
#include"App.h"

int main(int argc, char *argv[]){
  using System = superpunto::System;
  auto sys = std::make_shared<System>(argc, argv);
  {
    superpunto::App app(sys, argc, argv);
    app.run();
  }
  sys->log<System::MESSAGE>("Gracefully exiting");
  return 0;
}
