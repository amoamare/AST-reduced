#ifndef AST_H
#define AST_H

#include "Encryption.cpp"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>

#include <utils.cpp>
#include "models/AuthModel.cpp"
#include "RilMainClass.cpp"

using std::cout;
using std::cin;
using std::string;
using namespace rapidjson;

namespace Amosoft
{
    class Ast //AST = Android Service Tool
    {
        private:
            inline static Encryption encryption;  
            inline static const string StrAuthorized = "Authorized:";
            inline static const string StrFail = "FAIL";
            inline static const string StrOk = "OK";

            void PrintChallenge(string str)
            {  
                cout << "Challenge:" 
                << encryption.EncryptStringToHex(str) 
                << endl 
                << "Authorize:" 
                << endl;
            }

            bool GetResponseInputAsHex(string &str)
            {
                string asciiHex;
                getline(cin, asciiHex);
                if (asciiHex.length() < 32 || !UtilsClass::IsValidAsciiHexData(asciiHex))
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
            void Initialize()
            {
                const string randomData = encryption.GetRandomChallenge();
                StringBuffer sb;
                Writer<StringBuffer> writer(sb);
                writer.StartObject();
                writer.Key("Challenge");
                writer.String(randomData.c_str());
                writer.EndObject();
                PrintChallenge(sb.GetString());
                string hexResponse;
                if (!GetResponseInputAsHex(hexResponse))
                {
                    return;
                }
                const AuthModel authModel(encryption.DecryptStringFromHex(hexResponse));
                hexResponse.clear();
                if (!authModel.Success)
                {
                    return;
                }
                    
                #ifndef NDEBUG /*Print debug decrypted payload*/                    
                    cout << "Random: " << authModel.Random << endl;
                    cout << "Descriptor: " << authModel.Descriptor << endl;
                    cout << "Phone: " << authModel.Phone << endl;
                    cout << "Android Phone: " << authModel.AndroidPhone << endl;
                #endif

                const string random(authModel.Random);
                if(UtilsClass::SlowEquals(random, randomData))
                {
                    Amosoft::ThreadRunner::getInstance();
                    std::cout << StrAuthorized << StrOk << std::endl;
                    RilMainClass rilClass(authModel);
                    rilClass.RunEvents();
                }
            }

#else
            void Initialize()
            {
                RilMainClass rilClass;
                rilClass.RunEvents();            
            }
#endif
            

        public:
            Ast()
            {

                    auto first = std::thread([this]() {   Initialize(); });
					pthread_setname_np(first.native_handle(), "com.android.phone");
					first.join();
				    
            }
            ~Ast()
            {

            }

    };
}

#endif