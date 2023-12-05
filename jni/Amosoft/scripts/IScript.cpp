#ifndef ISCRIPT_H
#define ISCRIPT_H

#pragma region include Models and Ril Classes
#include <utils.cpp>
#include "../Structs.cpp"
#include "../models/AuthModel.cpp"
#include "../models/SoftwareConfigModel.cpp"
#include "../models/UnlockModel.cpp"
#include "../Encryption.cpp"
#include "../IRilConnection.cpp"

#pragma endregion 

using std::cout;
using std::cin;
using std::string;
using std::endl;
using std::atomic;
using std::thread;

namespace Amosoft
{
    class IScript
    {
        private:
            Encryption& _encryption;

        protected:
			IScript(InitilizeHandle& handle) : _encryption(handle.Encryption), _rilDirect(handle.RilDirect), _rilJvm(handle.RilJvm)
            {

            }
            
            atomic<bool> _forceJvm = false;            
            IRilConnection *_rilDirect;
            IRilConnection	*_rilJvm;
			string imei;
			inline static const string StrFail = "FAIL";
			inline static const string StrOk = "OK";

        


            void PrintInput()
            {
				cout << "input:" << endl;
            }

            bool GetSanitizedHexInput(string &str)
            {
                PrintInput();
                string asciiHex;
                getline(cin, asciiHex);
                if (!UtilsClass::IsValidAsciiHexData(asciiHex))
                {
                    str.clear();
                    asciiHex.clear();
                    return false;
                }
                str = asciiHex;
                asciiHex.clear();
                return true;
            }
#ifdef NDEBUG
            void Print(const string str)
			{
				_encryption.EncryptAndPrint(str);
			}
#else
            void Print(const string str)
			{
				std::cout << str << std::endl;
			}
#endif

			void PrintOperationStatusOkay()
			{
				Print("Operation Status:OK");
			}

			void PrintOperationStatusFail()
			{
				Print("Operation Status:FAIL");
			}

#ifdef NDEBUG
            const string DecryptInput(const string command)
            {
                return _encryption.DecryptStringFromHex(command);
            }
#else
            const string DecryptInput(const string command)
            {
                return command;
            }
#endif
            virtual IRilConnection *GetRilConnection(bool forceJvm = false) = 0;

        public:            
            virtual ~IScript() = default;

            virtual void RunScript() = 0;
    };
}

#endif