#pragma once

#include "Core/Log.h"
#include "Core/Application.h"

#ifdef PHOENIX_WINDOWS

  // #define _DEBUG
  #ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #include <stdlib.h>
  #endif

  #ifdef _DEBUG
    #define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
  #endif

#endif

extern Phoenix::Application* Phoenix::CreateApp();

int main(int argc, char** argv)
{
#ifdef PHOENIX_WINDOWS
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
  Phoenix::Log::Init();

  ENGINE_INFO("Welcome to the Motherland!");

  auto app = Phoenix::CreateApp();
  app->Run();
  delete app;

  Phoenix::Log::Shutdown();

  return 0;
}
