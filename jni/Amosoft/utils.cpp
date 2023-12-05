#ifndef UTILS_H
#define UTILS_H
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <regex>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n
#include <experimental/algorithm>
#include <experimental/functional>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <fstream>
#include <iterator>
#include <arpa/inet.h>
#include <time.h>
#include <thread>
#include <cctype>
extern "C" {
#include "meatgrinder/meat_grinder.h"
}

class UtilsClass
{
private:
	inline static char cmd_res_line[256];
	inline static char total_cmd_res[25600];
	

//2>/dev/null| egrep '/dev/pts/[[:digit:]]+' | grep -w  'sh' | grep -v -c '21720'
//echo $$ >> gets PID

//lsof 2>/dev/null| egrep '/dev/pts/[[:digit:]]+|socket' | egrep -w 'sh|logcat' | grep -v -c '21720'



  	inline static const std::string EmptyDataString = "XXX)0s0d9fsdosdosd0921304234-23-89";
  	inline static const std::string StrLogCatRadioClear = "logcat -b radio -c > /dev/null 2>&1";
  	inline static const std::string StrLogCatAllClear = "logcat -b all -c > /dev/null 2>&1";
	inline static const std::string StrLogCatCheck = "lsof 2>/dev/null|grep logcat | grep -c bin/logcat";
	inline static const std::string StrShellCheck = "lsof 2>/dev/null| egrep '/dev/pts/[[:digit:]]+' | grep -v -c '/dev/pts/0'";
  	inline static const std::string StrQcom = "QCOM";
	inline static const std::string StrDevNull = "/dev/null";
	inline static const std::string StrR = "r";
	inline static const std::string StrW = "w";

	inline static std::string GetProperty(const char *name)
	{
		char property[PROP_VALUE_MAX] = {0};
		int success = __system_property_get(name, property);
		if (success == 0) return std::string();
		return std::string(property);
	}
public:

	inline static std::string StoredPts;
	inline static bool CheckDebugLvlHigh = false;
	inline static bool CheckCpDebugLvlHigh = false;
	inline static bool CheckHardware = false;
	inline static bool CheckCarrier = false;
	inline static int RestrictedToHardwareType = 0; // Any
	//inline static CkJsonArray *RestrictedToCarriers;


	static void PrintLine(std::string line)
	{
		std::cout << line << std::endl;
	}

	static void Suicide()
	{
		setuid(0u);
		char path[260];
		memset(path, 0, sizeof(path));
		readlink("/proc/self/exe", path, sizeof(path));
		unlink(path);
	}

	static void Restart()
	{
		system("reboot");
	}

	static void SupressOutput(bool in, bool out, bool error)
	{
		if (in)
		{
			freopen(StrDevNull.c_str(), StrR.c_str(), stdin);
		}
		if (out)
		{
			freopen(StrDevNull.c_str(), StrW.c_str(), stdout);
		}
		if (error)
		{
			freopen(StrDevNull.c_str(), StrW.c_str(), stderr);
		}
	}

	static bool IsTtyRunning()
	{
		std::string value = exec_get_out("dumpsys usb | grep -E 'connected.*true' -c");
		if (value.empty())
		{
			return false; // we are going to assume its running true by default;
		}
		std::string::size_type sz;
		int i = std::stoi(value, &sz);
		return i > 0;
	}

	static void SetScreenOff()
	{
		return;
		system("svc power stayon false > /dev/null 2>&1; input keyevent KEYCODE_SLEEP > /dev/null 2>&1");
	}

	static void ToggleAirplaneOnAndWifiOff()
	{
		return;
		system("svc wifi disable > /dev/null 2>&1; settings put global airplane_mode_on 1 > /dev/null 2>&1");
	}

	static void ToggleAirplaneOffAndWifiDataOn()
	{
		return;
		system("svc wifi enable > /dev/null 2>&1; settings put global airplane_mode_on 0 > /dev/null 2>&1; svc data enable > /dev/null 2>&1");
	}

	static void CallHomeScreen()
	{
		return;
		system("am start -c android.intent.category.HOME -a android.intent.action.MAIN > /dev/null 2>&1");
	}

	static void ForceStopServiceMode()
	{
		return;
		system("am force-stop com.sec.android.app.servicemodeapp > /dev/null 2>&1");
	}

	static int GetSystemApiLevel()
	{
		std::string value = GetProperty("ro.build.version.sdk");
		std::string::size_type sz;
		int i = std::stoi(value, &sz);
		return i;
	}

	static bool IsFactoryBinary()
	{
			std::string value = GetProperty("ro.factory.factory_binary");
			if (value.empty()) return false;
			return value == "factory";
	}

	static bool IsUnsecuredBoot()
	{
			std::string value = GetProperty("ro.secure");
			std::string::size_type sz;
			int i = std::stoi(value, &sz);
			return i == 0;
	}

	static bool GetDeviceManufacturer(std::string& manufacturer)
	{
		manufacturer = GetProperty("ro.product.manufacturer");
		if (manufacturer.empty()) return false;
		if (CaseInSensStringCompare(manufacturer, "HTC") || CaseInSensStringCompare(manufacturer, "LG"))
		{
			std::transform(manufacturer.begin(), manufacturer.end(),manufacturer.begin(), ::toupper);
		}
	  	manufacturer[0] = toupper(manufacturer[0]);
		return true;
	}

	static bool GetFrpLocation(std::string& frpPath)
	{
		frpPath = GetProperty("ro.frp.pst");
		if (frpPath.empty()) return false;
		return true;
	}

	static int GetBatteryPercentage()
	{
		std::string value = exec_get_out("dumpsys battery | grep level | grep -E -o [[:digit:]]+");
    if (value.empty())
    {
      return 0;
    }
    std::string::size_type sz;
    int i = std::stoi(value, &sz);
    return i;
	}

	static std::string exec_get_out(const std::string& in)
	{
		const char * cmd = in.c_str();
		FILE* pipe = popen(cmd, "r");
		if (!pipe)
			return NULL;

		total_cmd_res[0] = 0;

		while (!feof(pipe))
		{
			if (fgets(cmd_res_line, 256, pipe) != NULL)
			{
				//TODO: add handling for long command reads...
				strcat(total_cmd_res, cmd_res_line);
			}
		}
		pclose(pipe);
		std::string value;
		value.assign(total_cmd_res);
		return value;
	}

	static std::string exec_get_out(char* cmd)
	{
		FILE* pipe = popen(cmd, "r");
		if (!pipe)
			return NULL;

		total_cmd_res[0] = 0;

		while (!feof(pipe)) {
			if (fgets(cmd_res_line, 256, pipe) != NULL)
			{
				//TODO: add handling for long command reads...
				strcat(total_cmd_res, cmd_res_line);
			}
		}
		pclose(pipe);
		std::string value;
		value.assign(total_cmd_res);
		return value;
	}

	static std::string StringToHex(const std::string& s, bool upper_case = true)
	{
		std::ostringstream ret;
		for (std::string::size_type i = 0; i < s.length(); ++i)
			ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << (int)s.at(i);
		return ret.str();
	}

	static std::string HexToString(const std::string& in)
	{
		std::string output;
		if ((in.length() % 2) != 0)
		{
			throw std::runtime_error(NULL);
		}
		size_t cnt = in.length() / 2;
		for (size_t i = 0; cnt > i; ++i)
		{
			uint32_t s = 0;
			std::stringstream ss;
			ss << std::hex << in.substr(i * 2, 2);
			ss >> s;

			output.push_back(static_cast<unsigned char>(s));
		}
		return output;
	}

	static bool IsDebugHigh()
	{
		std::string value = GetProperty("ro.debug_level");
		if (value.empty())
		{
			value = GetProperty("ro.boot.debug_level");
		}
		if (value.empty()) return true;
		return value == "0x4948";
	}

	static bool IsCpDebugHigh()
	{
		std::string value = GetProperty("ro.cp_debug_level");
		if (value.empty())
		{
			value = GetProperty("ro.boot.cp_debug_level");
		}
		if (value.empty()) return true;
		return value == "0x5500";
	}

	static bool IsLogcatRunning()
	{
	//#ifndef NDEBUG //if on debug build, do not clear log loop just return
		return false;
	//#endif
		std::string value = exec_get_out(StrLogCatCheck);
		if (value.empty())
		{
		return true;
		}
		std::string::size_type sz;
		int i = std::stoi(value, &sz);
		return i != 0;
	}

	static bool IsShellRunning(int limit = 0)
	{
	//#ifndef NDEBUG //if on debug build, do not clear log loop just return
		return false;
	//#endif
		std::string value = exec_get_out(StrShellCheck);
		if (value.empty())
		{
		return true; // we are going to assume its running true by default;
		}
		std::string::size_type sz;
		int i = std::stoi(value, &sz);
		return i > limit;
	}

	static void ClearLog()
	{
		return;
		KillLogCat();
		system(StrLogCatAllClear.c_str());
	}

	static void ClearLogLoop()
	{
			return;
			KillLogCat();
			KillShell();
		std::string StrSecurity("security");
		int retryCount = 0;
		const int Max = 10;
		std::string received;
		do
		{
		received = exec_get_out(StrLogCatAllClear);
		retryCount+=1;
		if (retryCount >= Max)
		{
			//we failed to clear log cat, something is keeping it running. bail out of software.
			exit(214);
		}
		}while(!received.empty() && !ContainsCaseInSens(received, StrSecurity));
	}

	static std::string GetModel()
	{
		std::string value = GetProperty("ro.product.model");
		return value;
	}

	static std::string GetVersionRelease()
	{
		std::string value = GetProperty("ro.build.version.release");
		return value;
	}

	static std::string GetSalesCode()
	{
		std::string value = GetProperty("ro.boot.carrierid");
		if (value.empty() || CaseInSensStringCompare(value, "unknown"))
		{
			value = GetProperty("ro.boot.sales_code");
		}
		if (value.empty() || CaseInSensStringCompare(value, "unknown"))
		{
			value = GetProperty("ro.csc.sales_code");
		}
		
		if (value.empty() || CaseInSensStringCompare(value, "unknown"))
		{
			value = GetProperty("ro.carrier");
		}
		transform(value.begin(), value.end(), value.begin(), ::toupper);
		return value;
	}

  	static std::string GetHardware()
	{
		std::string value = GetProperty("ro.boot.hardware");
    if (value.empty() || CaseInSensStringCompare(value, "unknown"))
		{
			value = GetProperty("ro.boot.hardware");
		}
    transform(value.begin(), value.end(), value.begin(), ::toupper);
		return value;
	}

	static std::string GetSerialNo()
	{
		std::string value = GetProperty("ril.serialnumber");
		if (value.empty() || CaseInSensStringCompare(value, "unknown"))
		{
			value = GetProperty("ro.boot.serialno");
			if (value.empty() || CaseInSensStringCompare(value, "unknown"))
			{
				value = GetProperty("ro.serialno");
			}
		}
    transform(value.begin(), value.end(), value.begin(), ::toupper);
		return value;
	}

	static int GetHardwareType()
	{
		std::string hardware = GetHardware();
		if (CaseInSensStringCompare(StrQcom, hardware))
		{
		return 1; //QUALCOMM
		}
		return 2; // Exynos
	}

	/// <summary>
	/// Hardware check to verify device is QCOM
	/// </summary>
	static bool HardwareCheck()
	{
		return (GetHardwareType() == RestrictedToHardwareType || RestrictedToHardwareType == 0);
	}

	static bool GetLibrarySystemProperty(std::string &library)
	{
    	std::string value = GetProperty("persist.sys.dalvik.vm.lib.2");
		if (value.empty())
		{
			value = GetProperty("persist.sys.dalvik.vm.lib");
			if (value.empty())
			{
				return false;
			}
		}
		library = value;
    	return true;
	}

/*
	/// <summary>
	/// Carrier Check for TMK and TMB
	/// </summary>
	static bool CarrierCheck()
	{
		std::string value = UtilsClass::GetSalesCode();
		int arraySize = RestrictedToCarriers->get_Size();
		int i;
		for(i = 0; i <= arraySize-1; i++)
		{
			std::string str = RestrictedToCarriers->stringAt(i);
			bool flag = CaseInSensStringCompare(value, str);
			if (flag) return true;
		}
		return false;
	}
	*/

  	static bool CompareChar(char & c1, char & c2)
	{
		if (c1 == c2)
			return true;
		else if (std::toupper(c1) == std::toupper(c2))
			return true;
		return false;
	}

	/*
	 * Case Insensitive String Comparision
	 */
	static bool CaseInSensStringCompare(std::string & str1, std::string &str2)
	{
		return ( (str1.size() == str2.size() ) &&
				 std::equal(str1.begin(), str1.end(), str2.begin(), &CompareChar) );
	}

	static bool ContainsCaseInSens(std::string & haystack, std::string & needle)
	{
		auto it = std::search(
		haystack.begin(), haystack.end(),
		needle.begin(), needle.end(),
		&CompareChar);
		return (it != haystack.end());
	}

	static bool CaseInSensStringCompare(const std::string& a, const std::string& b)
	{
		return std::equal(a.begin(), a.end(),
						  b.begin(), b.end(),
						  [](char a, char b) {
							  return tolower(a) == tolower(b);
						  });
	}

	// trim from start (in place)
	static inline void ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s)
	{
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::string ltrim_copy(std::string s)
	{
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy(std::string s)
	{
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trim_copy(std::string s)
	{
		trim(s);
		return s;
	}

	static bool endsWith(const std::string& str, const char* suffix, unsigned suffixLen)
	{
	    return str.size() >= suffixLen && 0 == str.compare(str.size()-suffixLen, suffixLen, suffix, suffixLen);
	}

	static bool endsWith(const std::string& str, const char* suffix)
	{
	    return endsWith(str, suffix, std::string::traits_type::length(suffix));
	}

	static bool startsWith(const std::string& str, const char* prefix, unsigned prefixLen)
	{
	    return str.size() >= prefixLen && 0 == str.compare(0, prefixLen, prefix, prefixLen);
	}

	static bool startsWith(const std::string& str, const char* prefix)
	{
	    return startsWith(str, prefix, std::string::traits_type::length(prefix));
	}

	template<typename T> static inline void pad_right(std::basic_string<T>& s, typename std::basic_string<T>::size_type n, T c)
	{
	   if (n > s.length())
	   	s.append(n - s.length(), c);
	}

	template<typename T> static inline void pad_left(std::basic_string<T>& s, typename std::basic_string<T>::size_type n, T c)
	{
		 if (n > s.length())
		 	s.insert(0, std::string(n-s.length(), c));
				// return std::string(n-str.size(), '0') + str;
			//	s.append(n - s.length(), c);
	}

	static std::string pad_right(std::string const& str, size_t s)
	{
	    if ( str.size() < s )
	        return str + std::string(s-str.size(), '0');
	    else
	        return str;
	}

 	static std::string pad_left(std::string const& str, size_t s)
	{
	    if ( str.size() < s )
	        return std::string(s-str.size(), '0') + str;
	    else
	        return str;
	}

	static bool SlowEquals(const std::string& a, const std::string& b)
	{
		uint diff = (uint)a.length() ^ (uint)b.length();
		for(int i = 0; i < a.length() && i < b.length();i++)
		{
			diff |= (uint)(a.at(i) ^ b.at(i));
		}
		return diff == 0;
	}

	/// Split
	static void Tokenize(const std::string& in, const std::string& delims, std::vector<std::string>& tokens)
	{
		tokens.clear();
		std::string::size_type pos_begin  , pos_end  = 0;
		std::string input = in;

		input.erase(std::remove_if(input.begin(),
								input.end(),
								[](auto x){return std::isspace(x);}),input.end());

		while ((pos_begin = input.find_first_not_of(delims,pos_end)) != std::string::npos)
		{
			pos_end = input.find_first_of(delims,pos_begin);
			if (pos_end == std::string::npos) pos_end = input.length();

			tokens.push_back( input.substr(pos_begin,pos_end-pos_begin) );
		}
	}

	static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
	{
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}

	static bool IsValidAsciiHexData(const std::string& in)
	{
		if ((in.length() % 2) != 0)
		{
			return false;
		}

		for(auto c : in)
		{
			if (!isxdigit(c))
			{
				return false;
			}
		}
		return true;
	}

	template<class ForwardIterator1, class ForwardIterator2> static
	ForwardIterator1 Search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
	{
		if (first2 == last2)
		{
			return first1;
		}
		while(first1!=last1)
		{
			ForwardIterator1 it1 = first1;
			ForwardIterator2 it2 = first2;
			while(*it1 == *it2)
			{
				if (it2 == last2)
				{
					return first1;
				}
				if (it1 == last1)
				{
					return last1;
				}
				++it1; ++it2;
			}
			++first1;
		}
		return last1;
	}

	static long long FindOffsetOfNeedleInFile(std::string& filePath, const std::string& needle)
	{
		long long offset = -1;
		std::ifstream ifs;
		ifs.open(filePath, std::ios::binary);
		if (!ifs.is_open())
		{
			return -1;
		}
		std::istreambuf_iterator<char> begin(ifs), end;
		auto it = Search(begin, end, needle.begin(), needle.end());
		if(it != end)
		{
			offset = static_cast<long long>(ifs.tellg()) - needle.size();
		}
		ifs.close();
		return offset;

	}

	static inline bool FileExists(std::string& filePath)
	{
		struct stat buffer;
		int success = stat(filePath.c_str(), &buffer);
		return success == 0;
	}

	static inline long long GetFileSize(std::string& filePath)
	{
		struct stat buffer;
		int success = stat(filePath.c_str(), &buffer);
		return success == 0 ?((long long) buffer.st_size) : -1;
	}

	static bool GetExynosDump(int timeout, std::string& filePath)
	{
		filePath = std::string();
		std::string value;
		const std::string path0("ls /data/log/cpcrash_dump* 2>/dev/null");
		const std::string path1("ls /sdcard/log/cpcrash_dump* 2>/dev/null");
		// Record start time
		auto start = std::chrono::high_resolution_clock::now();
		do
		{
			//std::cout << '.' << std::endl;
			//UtilsClass::ExitIf();
			if (value.empty())
			{
				value = exec_get_out(path0);
				UtilsClass::trim(value);
				if (value.empty())
				{
					value = exec_get_out(path1);
				}
				trim(value);
			}
			else
			{
				filePath = value;
				long long fileSize = 0;
				fileSize = GetFileSize(filePath);
				if (fileSize >= 85000000)
				{
					return true;
				}
			}
			auto timeNow = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = timeNow - start;
			if (elapsed.count() > timeout)
				return false;
		}while(true);
	}

	static std::string GetFileNameFromPath(int timeout = 25)
	{
		const std::string path0("ls /data/log/cpcrash_dump* 2>/dev/null");
		const std::string path1("ls /sdcard/log/cpcrash_dump* 2>/dev/null");
		std::string value = exec_get_out(path0);
		trim(value);
		if (value.empty())
		{
			value = exec_get_out(path1);
			trim(value);
		}
		// Record start time
		auto start = std::chrono::high_resolution_clock::now();
		if(value.empty())
		{
			do
			{
				//std::cout << '.' << std::endl;
				//UtilsClass::ExitIf();
				value = exec_get_out(path0);
				UtilsClass::trim(value);
				if (!value.empty())
				{
					break;
				}
				value = exec_get_out(path1);
				trim(value);
				if (!value.empty())
				{
					break;
				}
				auto timeNow = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = timeNow - start;
				if (elapsed.count() > timeout) break;
			}while(true);
		}
		return value;
	}

	static bool CheckFileSize(std::string filePath, int timeout = 30)
	{
		// Record start time
		auto start = std::chrono::high_resolution_clock::now();
		bool flag = false;
		long long fileSize = 0;
		do
		{
			//std::this_thread::sleep_for(std::chrono::seconds(10));
			std::cout << '.' << std::endl;
			if(FileExists(filePath))
			{
				fileSize = GetFileSize(filePath);
//std::cout << "File Size: " << fileSize << std::endl;
				if (fileSize >= 55000000)
				{
					flag = true;
					break;
				}
			}
			auto timeNow = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = timeNow - start;
//std::cout << "Elapsed: " << elapsed.count() << std::endl;
			if (elapsed.count() > timeout)
			{
				flag = false;
				break;
			}
		}while(!flag);
		return flag;
	}

	static void DisablePackage(const std::string& packageName)
	{
		std::string command("pm uninstall -k --user -0 ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1;pm uninstall ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1;pm disable -k --user -0  ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1;pm disable ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1;pm hide -k --user -0 ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1;pm hide ");
		command.append(packageName);
		command.append(" > /dev/null 2>&1");
		exec_get_out((char*)command.c_str());
	}

	static std::string RandomASCIIData(size_t length)
	{
	  auto randchar = []() -> char
	  {
	      const char charset[] =
				"!\"#$%&'()*+,-./"
				"0123456789"
				":;<=>?@"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	      const size_t max_index = (sizeof(charset) - 1);
	      return charset[ rand() % max_index ];
	  };
	  std::string str(length,0);
	  std::generate_n( str.begin(), length, randchar );
	  return str;
	}

	static void SpawnThreads(int n, std::function<void()> func)
	{
	    std::vector<std::thread> threads(n);
	    // spawn n threads:
	    for (int i = 0; i < n; i++)
		{
	        threads[i] = std::thread(func);
	    }

	    for (auto& th : threads) 
		{
	        th.detach();
	    }
	}

	static void KillLogCat()
	{

		return;
		//system("pkill -9 -f logcat 2>/dev/null");
		int api = GetSystemApiLevel();
		if (api > 24)
		{
			system("pkill -l SIGKILL logcat 2>/dev/null");
		}
		else
		{
			system("pkill -SIGKILL logcat 2>/dev/null");
		}
	}

	static void KillShell()
	{
		return;
		std::string kill;
 		//lsof 2>/dev/null| egrep '/dev/pts/[[:digit:]]+' | grep -v -c '/dev/pts/0'
		kill.append("lsof 2>/dev/null | grep -E '/dev/pts/[[:digit:]]+' | grep -v '");
		kill.append(StoredPts);
		kill.append("' | grep -E -o '^[[:alnum:]]+[[:blank:]]+[[:digit:]]+' | grep -E -o '[[:digit:]]+' | while read line; do kill -s SIGKILL $line 2>/dev/null; done");
//adb shell "lsof | egrep '/dev/pts/[[:digit:]]+' | grep -v '/dev/pts/0' | egrep -o '^[[:alnum:]]+[[:blank:]]+[[:digit:]]+'| egrep -o [[:digit:]]+"
//		int api = GetSystemApiLevel();
//		if (api > 24)
//		{

			//ps -u shell | grep 'S sh'
			//kill.append("ps -eopid,tty -fu shell | grep -v '");
			//kill.append(std::to_string(getpid()));
			//kill.append("' | grep pts | egrep -o '^.[0-9]+' | while read line; do kill -s SIGKILL $line 2>/dev/null; done");
//			kill.append("ps -u shell -eopid,ppid,name | grep -v '");
//			kill.append(std::to_string(getpid()));
//			kill.append("' | grep sh | egrep -o '^.[0-9]+' | while read line; do kill -s SIGKILL $line 2>/dev/null; done");
//		}
//		else
//		{
//			kill.append("ps | grep shell | grep -v '");
//			kill.append(std::to_string(getpid()));
//			kill.append("' | grep -v 'adbd' | egrep -o '^[[:alnum:]]+[[:space:]]+([0-9]+)' | egrep -o '[0-9]+' | while read line; do kill -s SIGKILL $line 2>/dev/null; done");
//		}
		system(kill.c_str());
		//system("ps -eopid,tty -fu shell | grep -v 'pts/0' | grep 'pts' | egrep -o '^.[0-9]+' | while read line; do kill -9 $line 2>/dev/null; done");
		//top -n 1 | grep -v '11047' | grep shell | egrep -o '^.[0-9]+' | while read line; do kill -9 $line 2>/dev/null; done
	}

	static void StopServices()
	{
		int iOriginalSTDIN_FILENO = -1;
		int iOriginalSTDOUT_FILENO = -1;
		int iOriginalSTDERR_FILENO = -1;
		RedirectStandardStreamsToDEVNULL(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
		system("setprop ctl.stop auditd > /dev/null 2>&1");
		system("setprop ctl.stop logd > /dev/null 2>&1");
		system("setprop ctl.stop logd-reinit > /dev/null 2>&1");
		system("setprop ctl.stop logcatd > /dev/null 2>&1");
		system("setprop ctl.stop DR-daemon > /dev/null 2>&1");
		RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
	}

	static void RedirectStandardStreamsToDEVNULL(int *_piOriginalSTDIN_FILENO, int *_piOriginalSTDOUT_FILENO, int *_piOriginalSTDERR_FILENO)
	{
	    //flushing pending things before redirection.
	    fflush(stdin);
	    fflush(stdout);
	    fflush(stderr);

	    *_piOriginalSTDIN_FILENO = dup(STDIN_FILENO);
	    *_piOriginalSTDOUT_FILENO = dup(STDOUT_FILENO);
	    *_piOriginalSTDERR_FILENO = dup(STDERR_FILENO);

	    int devnull = open("/dev/null", O_RDWR);
	    //dup2(devnull, STDIN_FILENO);
	    dup2(devnull, STDOUT_FILENO);
	    dup2(devnull, STDERR_FILENO);
	    close(devnull);
			fflush(stdin);
			fflush(stdout);
			fflush(stderr);

	}
	
	static void RestoreStandardStreams(int *_piOriginalSTDIN_FILENO, int *_piOriginalSTDOUT_FILENO, int *_piOriginalSTDERR_FILENO)
	{
	    //flushing pending things before restoring.
	    fflush(stdin);
	    fflush(stdout);
	    fflush(stderr);

	    //dup2(*_piOriginalSTDIN_FILENO, STDIN_FILENO);
	    dup2(*_piOriginalSTDOUT_FILENO, STDOUT_FILENO);
	    dup2(*_piOriginalSTDERR_FILENO, STDERR_FILENO);
	}


	static std::string CalculatePukCode(std::string imei)
	{
	  if (imei.length() > 14) imei = imei.substr(0,14);
	  std::vector<int> numArray;
	  for(int i = 0; i < imei.length(); i++)
	  {
	    numArray.push_back(imei[i] - '0');
	  }
	  long long num1 = std::pow(3.0, 5 + numArray[0] + numArray[1] + numArray[2]) - 2.0;
	  int num2 = std::stoi(imei.substr(8)) + 254;
	  int num3 = 22 + numArray[3] + numArray[4] + numArray[5] + numArray[6] + numArray[7] + numArray[8] + numArray[9] + numArray[10];
	  long long calculated = num1 * num2 * num3;
	  std::string code = std::to_string(calculated);
	  UtilsClass::pad_left(code, 8, '0'); //ensures we are at lest 8 chars long
	  return code.substr(code.length() - 8);
	}

	typedef enum {
		kAkseedModelMarvell		= 0,
		kAkseedModelSMN900S		= 51,
		kAkseedModelKorea		= 51,
		kAkseedModelSMN900L		= 71,
		kAkseedModelSMG900L		= 47,
		kAkseedModelSMG900K		= 54,
		kAkseedModelSMG900S		= 73,
		kAkseedModelSHVE470S	= 73,
		kAkseedModelSMN900K		= 61,
		kAkseedModelSHVE250K	= 88,
		kAkseedModelDefault		= 89,
	} AkseedModelOffset;

	/// <summary>
	/// Calculate AckSeedNo from AkSeedNo (AT+AKSEEDNO=1,000-000-000)
	/// </summary>
	static bool CalculateAkSeedNo(const std::string seed, std::string& ackSeedNo, int addOffset = AkseedModelOffset::kAkseedModelDefault)
	{
		ackSeedNo = std::string();
		typedef std::vector<std::string> StrList;
		std::regex regex("(\\+AKSEEDNO\\:1,)(\\d+-\\d+-\\d+)");
		std::smatch match;
		if (std::regex_search(seed, match, regex) && match.size() > 1)
		{
    	std::string result = match.str(2);
			StrList split;
			Tokenize(result, "-" , split);
			auto [ num1, num2, num3 ] = CalculateAkSeedNo(std::stoi(split[0]),std::stoi(split[1]),std::stoi(split[2]), addOffset);
			std::string reply("AT+AKSEEDNO=0,");
			reply.append(std::to_string(num1));
			reply.append("-");
			reply.append(std::to_string(num2));
			reply.append("-");
			reply.append(std::to_string(num3));
			ackSeedNo = reply;
			return true;
  	}
		else
		{
			return false;
	  }
	}

	static std::tuple<int, int, int> CalculateAkSeedNo(int number1, int number2, int number3, int addOffset)
	{
		int i, calc1, calc2, calc3;
	 	i = 1;
		calc1 = number2;
		while(i <= number3/2)
		{
			calc1 = number2 * number2 % number1 * calc1 % number1;
			i++;
		}
		calc1 += addOffset;
		calc2 = round(fmod((number1 * 0x181E5), log(number1))) + addOffset;
		calc3 = round(((number2 * number1) / number1) / log(number2)) + addOffset;
		return std::make_tuple(calc1, calc2, calc3);
	}

	static void ClearDataLogs()
	{
		system("rm -rf /data/log/* > /dev/null 2>&1;rm -rf /sdcard/log/* > /dev/null 2>&1");
	}

	static bool IsInDanger()
	{
		int count = 0;
		if (isFoundDangerousProps())
		{
			//count++;
		}
		if(isDetectedTestKeys())
		{
			count++;
		}
		if(isDetectedDevKeys())
		{
			count++;
		}
		if(isNotFoundReleaseKeys())
		{
			count++;
		}
		if (isPermissiveSelinux())
		{
			count++;
		}
		if (isSuExists())
		{
			count++;
		}
		if(isAccessedSuperuserApk())
		{
			count++;
		}
		if(isFoundSuBinary())
		{
			count++;
		}
		if(isFoundXposed())
		{
			count++;
		}
		if(isFoundResetprop())
		{
			count++;
		}
		if(isFoundWrongPathPermission())
		{
			count++;
		}
		if(isFoundHooks())
		{
			count++;
		}
		return count != 0;
	}

	static std::string ZeroPadNumber(int num, int paddingLength = 2)
	{
			std::ostringstream ss;
			ss << std::setw(paddingLength) << std::setfill('0') << num;
			std::string result = ss.str();
			if (result.length() > paddingLength)
			{
					result.erase(0, result.length() - paddingLength);
			}
			return result;
	}
};

#endif
