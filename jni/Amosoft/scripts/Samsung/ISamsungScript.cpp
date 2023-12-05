#ifndef ISAMSUNGSCRIPT_H
#define ISAMSUNGSCRIPT_H

#include <utils.cpp>
#include "../IScript.cpp"

#include "IRilConnectionDirectSamsung.cpp"
#include "IRilConnectionJvmSamsung.cpp"

namespace Amosoft
{
    class ISamsungScript : public IScript
    {
        private:
            //RilConnectionDirect *_rilDirect;

        protected:
			ISamsungScript(InitilizeHandle& handle) : IScript(handle) 
            {

            }

			typedef enum
			{
				Success = 0,
				ImeiSign = 1,
				Msl = 2,
				MslId = 3,
				KeyId = 4,
				CpId = 5,
				LockType = 6
			} SignStep;
			
			IRilConnectionSamsung *GetRilConnection(bool forceJvm = false)
			{
				if (_forceJvm || forceJvm) return reinterpret_cast<IRilConnectionSamsung *>(_rilJvm);
				return _rilDirect->Initialized() ? reinterpret_cast<IRilConnectionSamsung *>(_rilDirect) : 
                                                   reinterpret_cast<IRilConnectionSamsung *>(_rilJvm);
			}
			
            

			bool SendAtComamnd(const string command)
			{
				return GetRilConnection()->SendAtComamnd(command);
			}

			bool RunAtCommand(const string command, string& message)
			{
				return GetRilConnection()->RunAtCommand(command, message);
			}

			bool IsRadioOn(bool reconnect = false)
			{
				return GetRilConnection()->IsRadioOn(reconnect);
			}

			bool InvokeCommand(char* command, int size, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				return GetRilConnection()->InvokeOemRilRequestCommand(command, size, clearLogBefore, clearLogAfter, read);
			}

			bool InvokeCommand(string command, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				return InvokeCommand((char*)command.c_str(), command.length(), clearLogBefore, clearLogAfter, read);
			}

			bool CpCrash()
			{
				return GetRilConnection()->CpCrash();
			}

			bool CpCrashByAt()
			{
				return GetRilConnection()->CpCrashByAt();
			}

			int GetSprintUnlockState()
			{
				return GetRilConnection()->GetSprintUnlockState();
			}

			bool SetSprintUnlockState(int state)
			{
				return GetRilConnection()->SetSprintUnlockState(state);
			}

			bool QcWipe()
			{
				return GetRilConnection()->QcWipe();
			}

			bool QcReset()
			{
				return GetRilConnection()->QcReset();
			}

			bool SprRtn()
			{
				return GetRilConnection()->SprRtn();
			}

			bool ExynosDump(string blobData, string& response)
			{
				return GetRilConnection()->ExynosDump(blobData, response);
			}

			bool SetCsc(const string salesCode, const string salesCountry = string())
			{
				return GetRilConnection()->SetCsc(salesCode, salesCountry);
			}

			bool ResetModem(int sleepTime = 0)
			{
				Print("[*] Resetting Modem");
				bool flag = GetRilConnection()->ResetModem();
				if (!flag)
				{
					this_thread::sleep_for(chrono::seconds(sleepTime));
					flag = GetRilConnection()->ResetModem();
					if (!flag)
					{
						Print("Resetting Modem:FAIL");
						return false;
					}
				}
				Print("Resetting Modem:OK");
				Print("[*] Waiting For Modem");
				int retry = 0;
				while(IsRadioOn() && retry < 3)
				{
					this_thread::sleep_for(chrono::seconds(5));
					retry++;
				}
				this_thread::sleep_for(chrono::seconds(5));
				retry = 0;
				while(!IsRadioOn(true) && retry < 3)
				{
					this_thread::sleep_for(chrono::seconds(5));;
					retry++;
				}
				system("setprop ctl.start at_distributor 2>/dev/null");
				this_thread::sleep_for(chrono::seconds(sleepTime));
				Print("Waiting For Modem:OK");
				return true;
			}
			
			/// <summary>
			/// Validate input data contains AT+ and =
			/// </summary>
			bool IsAtCommand(const string command, string& message)
			{
				message = string();
				if(command.empty() || command.length() < 4 ||
						command.find("AT+", 0) != 0 ||
							command.find("=") == string::npos)
						{
							message = "Invalid AT Command";
							return false;
						}
				return true;
			}

			//AT+FUNCTION=EXYNOSDUMP
			void RunFunctionCommand(const string command, string & message)
			{
				message = string();
				string filter;
				filter.assign(command);
				filter.erase(0, 12); //erase AT+FUNCTION=
				string QcWipe("QCWIPE");
				if (UtilsClass::CaseInSensStringCompare(filter, QcWipe))
				{
					//QcWipe();
				}
			}

			bool CheckAndExecuteCommand(const string command, string& message)
			{
				message = string();
				//Verify all commands have a AT+ and = example AT+OEM=, AT+FUNCTION=, AT+COMMAND=
				if (!IsAtCommand(command, message))
				{
					return false;
				}
				else if(command.find("AT+FUNCTION=", 0) == 0) // AT+FUNCTION=
				{
					RunFunctionCommand(command, message);
					return true;
				}
				return RunAtCommand(command, message); // AT+
			}

			/// <summary>
			/// Validate input data has hexadecimal, exit or length
			/// </summary>
			bool ValidateInputData(const string command)
			{
				const string StrExit("exit");
				if (command.empty() || UtilsClass::CaseInSensStringCompare(command, StrExit))
				{
					return false;
				}
				if(!UtilsClass::IsValidAsciiHexData(command))
				{
					return false;
				}
				if (command.length() <= 16)
				{
					return false;
				}
				return true;
			}


            void RebootUpload()
			{
				Print("[*] Reboot Upload");
				Print("Reboot Upload:OK");
				CpCrash();
				Print("Operation Status:OK");
			}

			void RebootUploadByAt()
			{
				Print("[*] Reboot Upload");
				Print("Reboot Upload:OK");
				CpCrash();
				Print("Operation Status:OK");
			}

			bool UnlockCommand(const string command)
			{
				Print("[*] Unlocking");
				bool success = InvokeCommand(command);
				if (success)
				{
					Print("Unlocking:OK");
					return true;
				}
				else
				{
					Print("Unlocking:FAIL");
					return false;
				}
			}

			bool CheckSprUnlockState()
			{
				Print("[*] Verify Unlock State");
				int state = GetSprintUnlockState();
				bool success = state == 2;
				int retry = 3;
				while(!success)
				{
					retry--;
					state = GetSprintUnlockState();
					success = state == 2;
					if (success) break;
					if (retry < 0) break;
					this_thread::sleep_for(chrono::seconds(1));
				}
				string str("Unlock State:");
				if (!success)
				{
					str.append("FAIL");
				}
				else
				{
					str.append("OK");
				}
				Print(str);
				return success;
			}

			bool TryWriteCalculatedDataLock(int retry = 3)
			{
				if (retry == 3)
				{
					Print("[*] Reading Data");
				}
				if (retry <= 0)
				{
					Print("Reading Data:FAIL");
					return false;
				}
				bool flag = false;
				try
				{
					flag = WriteCalculatedDataLock();
					if (flag)
					{
						Print("Reading Data:OK");
						return true;
					}
					return TryWriteCalculatedDataLock(retry--);
				}
				catch(int error)
				{
					return TryWriteCalculatedDataLock(retry--);
				}
			}

			bool WriteCalculatedDataLock()
			{
				const string zeros("00000000");
				string pukCode = UtilsClass::CalculatePukCode(imei);
				//AT+DETALOCK=2,pukCode,pukCode,pukCode,pukCode,pukCode,pukCode
				string command("AT+DETALOCK=2,");
				command.append(zeros); // Master key (aka freeze code)
				command.append(",");
				command.append(zeros); // Network lock
				command.append(",");
				command.append(zeros); // Subset lock
				command.append(",");
				command.append(zeros); // SP Lock
				command.append(",");
				command.append(zeros); // CP Lock
				command.append(",");
				command.append(zeros); // SIM Lock
				string test(command);
				int retry = 3;
				string message;
				do
				{
					if(RunAtCommand(test, message))
					{
						if (!message.empty())
						{
							break;
						}
					}
					message.clear();
					this_thread::sleep_for(chrono::seconds(2));
					retry--;
				} while(retry != 0);
				retry = 3;
				if (message.empty())
				{
					return false;
				}
				bool flag = (message.find("DETALOCK:2,OK") != string::npos);
				if (flag)
				{
					return true;
				}
				return false;
			}

			void OperationQcEfsClearAndRestore()
			{
				Print("[*] QC Reset & Clear");
				if (UtilsClass::GetHardwareType() != 1)
				{
					Print("[!] Device is not Qualcomm");
					return;
				}
				QcReset();
				Print("QC Reset:OK");
				ResetModem(5);
			}

			bool BypassAndSetMsl(bool reset = true)
			{
				Print("[*] Initializing Data");
				string command;
				string message;
				int retry = 3;
				do
				{
					if(ReadMslAddress(message))
					{
						break;
					}
					retry--;
				} while(retry != 0);
				retry = 3;
				string serialNo;
				if (!GetMslAddress(message, serialNo))
				{
			  	Print("Initializing Data:FAIL");
			  	return false;
				}

				do
				{
					if(ReadMslCpId(message))
					{
						break;
					}
					retry--;
				} while(retry != 0);
				retry = 3;

				string mslCode;
				if (!GetMslCode(message, mslCode))
				{
			  	Print("Initializing Data:FAIL");
			  	return false;
				}

				do
				{
					if(WriteMslCode(serialNo, mslCode, message))
					{
						break;
					}
					this_thread::sleep_for(chrono::seconds(2));
					retry--;
				} while(retry != 0);
				retry = 3;
				bool checkFlag = CheckMslCode(message);
				if (!checkFlag && !reset)
				{
			  	Print("Initializing Data:FAIL");
					return false;
				}
				if(!checkFlag && reset)
				{
			  	Print("Initializing Data:FAIL");
					Print("[*] Reset Data");
					OperationQcEfsClearAndRestore();
					return BypassAndSetMsl(false);
				}

				if(!ReadCalculateSendAkSeedNo(message))
				{
			  	Print("Initializing Data:FAIL");
					return false;
				}
				Print("Initializing Data:OK");
				return true;
			}

			bool SetRfByCode(const string code)
			{
				string command("AT+RFBYCODE=2,1,");
				command.append(code);
				string message;
				if (!RunAtCommand(command, message))
				{
					return false;
				}
				return (message.find("RFBYCODE:2,OK") != string::npos);
			}

			bool GetRfByCode(string& rfByCode)
			{
				rfByCode = string();
				string message;
				if (!RunAtCommand("AT+RFBYCODE=1,1,0", message))
				{
					return false;
				}
				if (!(message.find("RFBYCODE:1,") != string::npos))
				{
					return false;
				}
				size_t index = message.find_last_of(",")+1; //index of ,
				if (index < 0)
				{
					return false;
				}
				message.erase(0, index);
				if (message.length() < 3)
				{
					return false;
				}
				rfByCode = message;
				return true;
			}

			bool TryResetEfsAndClear(bool reset = true, int retry = 3, int modemTimeout = 5)
			{
				if (retry == 3)
				{
					Print("[*] Reading Data");
				}
				if (retry <= 0)
				{
					Print("Reading Data:FAIL");
					return false;
				}

				try
				{
					RtnResetEfsAndClear();
					Print("Reading Data:OK");
					if (reset) ResetModem(modemTimeout);
					return true;
				}
				catch(int x)
				{
					return TryResetEfsAndClear(reset, retry--);
				}
			}

			void RtnResetEfsAndClear()
			{
				SprRtn();
				QcReset();
			}


#pragma region MSL Commands
			
			bool ReadMslAddress(string& message)
			{
				if (!RunAtCommand("AT+MSLSECUR=1,0,0", message))
				{
					return false;
				}
				return true;
			}

			bool ReadMslCpId(string& message)
			{
				if (!RunAtCommand("AT+MSLSECUR=1,1,0", message))
				{
					return false;
				}
				return true;
			}

			bool WriteMslCode(string serialNo, string mslCode, string& message)
			{
				string command = "AT+MSLSECUR=2,";
				command.append(serialNo);
				command.append("_");
				command.append(mslCode);
				if (!RunAtCommand(command, message))
				{
					return false;
				}
				return true;
			}

			bool WriteMslCodeTest(string serialNo, string mslCode, string& message)
			{
				string command = "AT+MSLSECUR=2,";
				command.append("R58M34N9CJ");
				command.append("_");
				command.append("DD04379F4225D8E7BD3CAC54BAFE4C4");
				if (!RunAtCommand(command, message))
				{
					return false;
				}
				return true;
			}

			bool GetMslAddress(string command, string& mslAddress)
			{
				mslAddress = string();
				if (!(command.find("MSLSECUR:1,") != string::npos))
				{
					return false;
				}
				size_t index = command.find(",")+1; //index of ,
				if (index < 0)
				{
					return false;
				}
				command.erase(0, index);
				if (command.length() < 10)
				{
					return false;
				}
				mslAddress = command.substr(0, 10);//(message.find('\n')-1));
				return true;
			}

			bool GetMslCode(string command, string& mslCode)
			{
				mslCode = string();
				if (!(command.find("MSLSECUR:1,") != string::npos))
				{
					return false;
				}
				size_t index = command.find_last_of("_")+1; //index of ,
				if (index < 0)
				{
					return false;
				}
				command.erase(0, index);
				if (command.length() < 32)
				{
					return false;
				}
				mslCode = command.substr(0, 32);
				return true;
			}

			bool CheckMslCode(const string message)
			{
				if (message.find("MSLSECUR:2,OK") != string::npos)
				{
					return true;
				}
				return false;
			}

			bool ReadAkSeedNo(string& message)
			{
				if (!RunAtCommand("AT+AKSEEDNO=1,0,0", message))
				{
					return false;
				}
				return (message.find("AKSEEDNO:1,") != string::npos);
			}

			bool WriteAkSeedNo(string akSeedNo, string& message)
			{
				if (!RunAtCommand(akSeedNo, message))
				{
					return false;
				}
				return true;
			}

			bool ReadCalculateSendAkSeedNo(string& message, int attempts = 3)
			{
				string command;
				int retry = 3;
				do
				{
					if(ReadAkSeedNo(message))
					{
						break;
					}
					retry--;
				} while(retry != 0);
				retry = 3;

				if(!UtilsClass::CalculateAkSeedNo(message, command))
				{
			  	return false;
				}

				do
				{
					if(WriteAkSeedNo(command, message))
					{
						break;
					}
					retry--;
				} while(retry != 0);
				bool flag = (message.find("AKSEEDNO:0,OK") != string::npos);
				if (flag)
				{
					return true;
				}
				if (attempts == 0)
				{
					return false;
				}
				return ReadCalculateSendAkSeedNo(message, attempts--);
			}

#pragma endregion 


			
#pragma region Samsung Spr

			void GetSprintNonce()
			{

			}
#pragma endregion 

        public:            
            virtual ~ISamsungScript() = default;

    };
}

#endif