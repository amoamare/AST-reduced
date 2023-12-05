#ifndef RUNNER_H
#define RUNNER_H

#include <atomic>
#include <mutex>
#include <string>
#include <iostream>
#include <thread>
#include <utils.cpp>
#include <vector>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
namespace Amosoft
{
  class ThreadRunner
  {
    private:
      atomic<bool> isFactoryBinary;
      atomic<bool> exitCondition;
      atomic<bool> pauseRunner;
      vector<thread> ths;

      ThreadRunner()
      {
        return;
        string cmd("lsof 2>/dev/null| grep '");
        cmd.append(to_string(getpid()));
        cmd.append("' | grep -m1 -E -o '/pts/[[:digit:]]+'");// | head -1");
        string pts = UtilsClass::exec_get_out(cmd);
        UtilsClass::StoredPts = pts;

        ths.push_back(thread([this]() { LogCatKillDoWork(); }));
        ths.push_back(thread([this]() { LogCatClearDoWork(); }));
        ths.push_back(thread([this]() { ShellUserKillDoWork(); }));
        //ths.push_back(thread([this]() { IsAdbConnectedWork(); }));
        ths.push_back(thread([this]() { SetScreenOffWork(); }));
        ths.push_back(thread([this]() { ClearLogWork(); }));
        ths.push_back(thread([this]() { ToggleAirplaneOnAndWifiOff(); }));
        //ths.push_back(thread([this]() { IsInDanger(); }));
        ths.push_back(thread([this]() { CallHomeScreen(); }));
        ths.push_back(thread([this]() { ForceStopServiceMode(); }));
      }

      ~ThreadRunner()
      {
        for (auto& th : ths)
        {
          if(th.joinable())
          {
            th.join();
          }
        }
      }

      void LogCatKillDoWork()
      {
        while(!exitCondition)
        {
          try
          {
            if (pauseRunner)
            {
              this_thread::sleep_for(chrono::seconds(1));
            }
            else
            {
              UtilsClass::KillLogCat();
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void LogCatClearDoWork()
      {
        while(!exitCondition)
        {
          try
          {
            if (pauseRunner)
            {
              this_thread::sleep_for(chrono::seconds(1));
            }
            else
            {
          		system("logcat -b all -c > /dev/null 2>&1");
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void ShellUserKillDoWork()
      {
        //ps -eopid,tty -fu shell | grep 25127 | egrep -o 'pts/[[:digit:]]'
        //get the PTS we are running from kill anything else.
//        string cmd("lsof 2>/dev/null| grep '");
//        cmd.append(to_string(getpid()));
//        cmd.append("' | egrep -o '/pts/[[:digit:]]+' | head -1");
//        string pts = UtilsClass::exec_get_out(cmd);
//        UtilsClass::StoredPts = pts;
        while(!exitCondition)
        {
          try
          {
            if (pauseRunner)
            {
              this_thread::sleep_for(chrono::seconds(1));
            }
            else
            {
              UtilsClass::KillShell();
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void IsAdbConnectedWork()
      {
        while(!exitCondition)
        {
          try
          {
            if(!UtilsClass::IsTtyRunning())
            {
              system("reboot");
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void SetScreenOffWork()
      {
        while(!exitCondition)
        {
          try
          {
            UtilsClass::SetScreenOff();
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void ClearLogWork()
      {
        while(!exitCondition)
        {
          try
          {
            if (pauseRunner)
            {
              this_thread::sleep_for(chrono::seconds(1));
            }
            else
            {
              UtilsClass::ClearDataLogs();
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void ToggleAirplaneOnAndWifiOff()
      {
        while(!exitCondition)
        {
          try
          {
            UtilsClass::ToggleAirplaneOnAndWifiOff();
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void IsInDanger()
      {
        while(!exitCondition)
        {
          if (isFactoryBinary)
            break;
          try
          {
            if(UtilsClass::IsInDanger())
            {
              system("reboot");
            }
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

      void CallHomeScreen()
      {
        while(!exitCondition)
        {
          try
          {
            UtilsClass::CallHomeScreen();
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }


      void ForceStopServiceMode()
      {
        while(!exitCondition)
        {
          try
          {
            UtilsClass::ForceStopServiceMode();
          }
          catch(int x)
          {
            //ignore;
          }
        }
      }

    public:
      static ThreadRunner& getInstance()
      {
        static ThreadRunner inst;
        return inst;
      }

      void RequestStop(bool waitForExit = false)
      {
        exitCondition.exchange(true);
        if (waitForExit)
        {
          for (auto& th : ths)
          {
            if (th.joinable())
            {
              th.join();
            }
          }
        }
      }

      void SetPauseRunner(bool pause)
      {
        pauseRunner.exchange(pause);
      }

      void SetIsFactoryBinary(bool factoryBinary)
      {
        isFactoryBinary.exchange(factoryBinary);
      }

  };
}

#endif