#include <branding.h>
#include <utils.cpp>
#include <Amosoft/ThreadRunner.cpp>
#include <Amosoft/Ast.cpp>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <linux/elf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>



using namespace std;
using namespace Amosoft;

inline static const string ComSecPhone = "com.sec.phone";
inline static const string ComExpWay = "com.jio.myjio";// "com.expway.embmsserver";
inline static const string ComEmiddle = "com.jio.emiddleware";

static void PrintHeader()
{
#ifndef NDEBUG  
    UtilsClass::PrintLine("###################################################");
    UtilsClass::PrintLine("#!!!!!!!!!!WARNING THIS IS A DEBUG BUILD!!!!!!!!!!#");
    UtilsClass::PrintLine("#!!!!!!!!!!!!!DO NOT RELEASE BUILD!!!!!!!!!!!!!!!!#");
    UtilsClass::PrintLine("###################################################");
#endif
    UtilsClass::PrintLine("[*] Android Service Tool @Copyright Justin Davis www.androidservicetool.com 2014-2019");
}

static int execv_cpp(const string &path,  const vector<string> &argv)
{
    /* Convert arguments to C-style and call execv. If it returns
     * (fails), clean up and pass return value to caller. */

    if (argv.size() == 0)
    {
        errno = EINVAL;
        return -1;
    }

    vector<char *> vec_cp;
    vec_cp.reserve(argv.size() + 1);
    for (auto s : argv)
        vec_cp.push_back(strdup(s.c_str()));
    vec_cp.push_back(NULL);

    int retval = execv(path.c_str(), vec_cp.data());

    int save_errno = errno;
    for (auto p : vec_cp)
        free(p);
    errno = save_errno;
    return retval;
}

static void execv_cpp(const vector<string> &argv)
{
    if (argv.size() == 0) {
        throw invalid_argument(
            "At least one argument must be provided, being the path of "
            "the binary to execute");
          }
    execv_cpp(argv[0], argv);
}

void Setup()
{
  //on release builds, we want to suicide our script, exit with function suicide and exit if errors.
#ifdef NDEBUG
  UtilsClass::Suicide();
	atexit(UtilsClass::Suicide);
	UtilsClass::SupressOutput(false, false, true); // suppress errors
  Amosoft::ThreadRunner::getInstance();
#endif
    std::set_terminate([]() -> void {
        std::cerr << "terminate called after throwing an instance of ";
        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception &ex)
        {
           // std::cerr << typeid(ex).name() << std::endl;
            std::cerr << "  what(): " << ex.what() << std::endl;
        }
        catch (...)
        {
           // std::cerr << typeid(std::current_exception()).name() << std::endl;
            std::cerr << " ...something, not an exception, dunno what." << std::endl;
        }
        std::cerr << "errno: " << errno << ": " << std::strerror(errno) << std::endl;
        std::abort();
    });
  setreuid(0, 1001); // attempt anyways
  PrintHeader();
}


#if APPTYPE == APPTYPE_SAMSUNG
int main(int argc, char **argv)
{
  try
  {
    string entryName = UtilsClass::IsUnsecuredBoot() ? ComSecPhone : ComExpWay;
    if (argc >= 1 && strcmp(argv[0], entryName.c_str()) != 0)
    {
      execv_cpp(argv[0], {entryName.c_str()});
      return -1;
    }
    Setup();
    try
    {   
      Ast ast;
    } 
    catch(const std::exception& ex)
    {   
      UtilsClass::PrintLine("hmm");
    }
    catch(int x)
    {

    }
    return 0;
  }
  catch(const std::exception& ex)
  {
    std::cout << "FUCK YOU " << std::endl;
  }
  catch(int x)
  {
    return 0;
  }
}

#else



int main(int argc, char **argv)
{
  try
  { string entryName = "com.motorola.android.internal.telephony";
    if (argc >= 1 && strcmp(argv[0], entryName.c_str()) != 0)
    {
      execv_cpp(argv[0], {entryName.c_str()});
      return -1;
    }
    Setup();
    try
    {   
      Ast ast;
    } 
    catch(const std::exception& ex)
    {   
      UtilsClass::PrintLine("hmm");
    }
    catch(int x)
    {

    }
    return 0;
  }
  catch(const std::exception& ex)
  {
    std::cout << "FUCK YOU " << std::endl;
  }
  catch(int x)
  {
    return 0;
  }
}
#endif
