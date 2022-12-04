// #if 0
#pragma once

#include "Core/Log.h"
#include "Core/Application.h"

#ifdef PHOINIX_WINDOWS

   //#define _DEBUG
   #ifdef _DEBUG
      #define _CRTDBG_MAP_ALLOC
      #include <crtdbg.h>
      #include <stdlib.h>
   #endif

   #ifdef _DEBUG
      #define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
   #endif

#endif

extern Phoinix::Application* Phoinix::CreateApp();

int main(int argc, char** argv)
{
#ifdef PHOINIX_WINDOWS
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
   Phoinix::Log::Init();

   ENGINE_INFO("Welcome to the Motherland!");

   auto app = Phoinix::CreateApp();
   app->Run();
   delete app;

   Phoinix::Log::Shutdown();

   return 0;
}

// #endif