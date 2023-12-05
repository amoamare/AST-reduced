#ifndef SAMSUNGREADINFOSCRIPT_H
#define SAMSUNGREADINFOSCRIPT_H

#include <iostream>

#include "ISamsungScript.cpp"
#include "ReadSpcScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class ReadInfoScript : public ISamsungScript
    {
        private:
            InitilizeHandle& _handle;

            bool ReadSignatureData()
            {
                const string operation("Reading Hardware Information");
                cout << "[*] " << operation << endl;
                SignStep step = SignStep::ImeiSign;

                string imeiSign("Cert Status:[");
                string mslAddress("MSL Address:[");
                string mslId("MSL ID:[");
                string keyId("Key ID:[");
                string cpId("CPID:[");
                string lockType("Lock Type:[");
                do
                {
                    string command;
                    switch(step)
                    {
                        case SignStep::ImeiSign:
                            command = string("AT+IMEISIGN=0,1,0");
                            break;
                        case SignStep::Msl:
                            command = string("AT+MSLSECUR=1,0");
                            break;
                        case SignStep::MslId:
                            command = string("AT+MSLSECUR=1,1");
                            break;
                        case SignStep::KeyId:
                            command = string("AT+IMEISIGN=1,1,0");
                            break;
                        case SignStep::CpId:
                            command = string("AT+IMEISIGN=1,3,0");
                            break;
                        case SignStep::LockType:
                            command = string("AT+RFBYCODE=1,1,0");
                            break;
                        default:
                            cout << operation << ':' << StrFail << endl;
                            return false;
                    }

                    string message;

                    if (!RunAtCommand(command, message))
                    {
                        cout << operation << ':' << StrFail << " (" << message << ')' << endl;
                        return false;
                    }

                    switch(step)
                    {
                        case SignStep::ImeiSign:
                            if (!(message.find("IMEISIGN:0,OK") != string::npos))
                            {
                                imeiSign.append("FAIL]");
                            }
                            else
                            {
                                imeiSign.append("PASS]");
                            }
                            step = SignStep::Msl;
                            break;
                        case SignStep::Msl:
                            if (!(message.find("MSLSECUR:1,") != string::npos))
                            {
                                mslAddress.append("FAIL]");
                            }
                            else
                            {
                                size_t index = message.find(",")+1; //index of ,
                                message.erase(0, index);
                                string mslAdd = message.substr(0, (message.find('\n')-1));
                                mslAddress.append(mslAdd);
                                mslAddress.append("]");
                            }
                            step = SignStep::MslId;
                            break;
                        case SignStep::MslId:
                            if (!(message.find("MSLSECUR:1,") != string::npos))
                            {
                                mslId.append("FAIL]");
                            }
                            else
                            {
                                size_t index = message.find(",")+1; //index of ,
                                message.erase(0, index);
                                string msl = message.substr(0, (message.find('\n')-1));
                                size_t found = message.find_last_of("_");
                                if (found < 100)
                                {
                                    //mslId.append(msl);
                                    mslId.append(msl.substr(found+1));
                                    mslId.append("]");
                                }
                            }
                            step = SignStep::KeyId;
                            break;
                        case SignStep::KeyId:
                            if (!(message.find("IMEISIGN:1,") != string::npos))
                            {
                                keyId.append("FAIL]");
                            }
                            else
                            {
                                //cerr << str << '\n';
                                size_t index = message.find(",")+1; //index of ,
                                message.erase(0, index);
                                //str.erase(remove(str.begin(), str.end(), '\n'), str.end());
                                string strKeyId = message.substr(0, (message.find('\n')-1));
                                keyId.append(strKeyId);
                                keyId.append("]");
                            }
                            step = SignStep::CpId;
                            break;
                        case SignStep::CpId:
                            if (!(message.find("IMEISIGN:1,") != string::npos))
                            {
                                cpId.append("FAIL]");
                            }
                            else
                            {
                                //cerr << str << '\n';
                                size_t index = message.find(",")+1; //index of ,
                                message.erase(0, index);
                                //str.erase(remove(str.begin(), str.end(), '\n'), str.end());
                                string cpid = message.substr(0, (message.find('\n')-1));
                                cpId.append(cpid);
                                cpId.append("]");
                            }
                            step = SignStep::LockType;
                            break;
                        case SignStep::LockType:
                            if (!(message.find("+RFBYCODE:1,") != string::npos))
                            {
                                lockType.append("FAIL]");
                            }
                            else
                            {
                                //cerr << str << '\n';
                                size_t index = message.find(",")+1; //index of ,
                                message.erase(0, index);
                                //str.erase(remove(str.begin(), str.end(), '\n'), str.end());
                                string strLockType = message.substr(0, (message.find('\n')-1));
                                lockType.append(strLockType);
                                lockType.append("]");
                            }
                            step = SignStep::Success;
                            break;
                        default:
                            cout << operation << ':' << StrFail << endl;
                            return false;
                    }
                    if (step == SignStep::Success)
                    {
                        break;
                    }
                } while(true);
                cout << operation << ':' << StrOk << endl
                << imeiSign << endl
                << mslAddress << endl
                << mslId << endl
                << keyId << endl
                << cpId << endl
                << lockType << endl;
                return true;
            }


        public:
        
			ReadInfoScript(InitilizeHandle& handle) : _handle(handle), ISamsungScript(handle) 
            {
            }

            void RunScript()
            {
                if(!ReadSignatureData()) return;                             
                (new ReadSpcScript(_handle))->RunScript();
            }

        protected:

    };
}

#endif