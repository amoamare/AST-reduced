#ifndef RILCONNECTIONDIRECTSAMSUNG_H
#define RILCONNECTIONDIRECTSAMSUNG_H

#include <string>
#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <atomic>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable


#include "models/AuthModel.cpp"
#include "IRilConnectionSamsung.cpp"

namespace Amosoft
{
	class IRilConnectionDirectSamsung : public Amosoft::IRilConnectionSamsung
	{
    	private:
			#define RIL_REQUEST_OEM_HOOK_RAW 59
			#define REQ_OEM_HOOK_RAW RIL_REQUEST_OEM_HOOK_RAW
			#define RIL_REQUEST_OEM_HOOK_STRINGS 60
			#define REQ_OEM_HOOK_STRINGS RIL_REQUEST_OEM_HOOK_STRINGS

			#define RIL_UNSOL_SIM_SMS_STORAGE_AVAILALE 11015
			#define RIL_UNSOL_HSDPA_STATE_CHANGED 11016
			#define RIL_UNSOL_WB_AMR_STATE 11017
			#define RIL_UNSOL_TWO_MIC_STATE 11018
			#define RIL_UNSOL_DHA_STATE 11019
			#define RIL_UNSOL_UART 11020
			#define RIL_UNSOL_RESPONSE_HANDOVER 11021
			#define RIL_UNSOL_IPV6_ADDR 11022
			#define RIL_UNSOL_NWK_INIT_DISC_REQUEST 11023
			#define RIL_UNSOL_RTS_INDICATION 11024
			#define RIL_UNSOL_OMADM_SEND_DATA 11025
			#define RIL_UNSOL_DUN 11026
			#define RIL_UNSOL_SYSTEM_REBOOT 11027
			#define RIL_UNSOL_VOICE_PRIVACY_CHANGED 11028
			#define RIL_UNSOL_UTS_GETSMSCOUNT 11029
			#define RIL_UNSOL_UTS_GETSMSMSG 11030
			#define RIL_UNSOL_UTS_GET_UNREAD_SMS_STATUS 11031
			#define RIL_UNSOL_MIP_CONNECT_STATUS 11032

			#define RIL_REQUEST_DEVICE_IDENTITY 98


			void* mSecRilLibHandle;

			struct RilClient {
			    void *prv;
			};

			typedef struct RilClient* HRilClient;
			HRilClient mClient;

			/* Callbacks */
			typedef void (*RilOnComplete)(HRilClient handle, const void *data, int datalen);
			typedef void (*RilOnUnsolicited)(HRilClient handle, const void *data, int datalen);
			typedef void (*RilOnError)(void* data, int error);

			/* RILD functions */
			typedef HRilClient ((*t_OpenClient_RILD)(void));
			typedef int ((*t_CloseClient_RILD)(HRilClient client));
			typedef int ((*t_Connect_RILD)(HRilClient client));
			typedef int ((*t_Disconnect_RILD)(HRilClient client));
			typedef int ((*t_isConnected_RILD)(HRilClient client));
			typedef int ((*t_RegisterRequestCompleteHandler)
				     (HRilClient client, uint32_t id, RilOnComplete callback));
			typedef int ((*t_RegisterUnsolicitedHandler)
				     (HRilClient client, uint32_t id, RilOnUnsolicited callback));
			typedef int ((*t_RegisterErrorCallback)
				     (HRilClient client, RilOnError callback, void *data));
			typedef int ((*t_InvokeOemRequestHookRaw)
				     (HRilClient client, char *data, size_t size));


			typedef int ((*t_ModemAPI_Send_request)
				     (HRilClient client, char *buffer, size_t unknown, size_t buffer_length, size_t id));

			/* Global symbols */
			#define DEF(n) t_##n n = NULL
			DEF(CloseClient_RILD);
			DEF(Connect_RILD);
			DEF(isConnected_RILD);
			DEF(InvokeOemRequestHookRaw);
			DEF(OpenClient_RILD);
			DEF(Disconnect_RILD);
			DEF(RegisterErrorCallback);
			DEF(RegisterRequestCompleteHandler);
			DEF(RegisterUnsolicitedHandler);
			DEF(ModemAPI_Send_request);
			#undef DEF

			void *resolve(void *lib, const char *name)
			{
			  void *sym;
			  sym = dlsym(lib, name);
			  //printf("[D] Symbol %s is at %p\n", name, sym);
			  assert(sym);
			  return sym;
			}

			inline static std::vector<char> Reply;
			inline static std::atomic<int> RilError = 0;
			inline static std::atomic<bool> IsAtCommand = false;
			inline static std::atomic<bool> IsCallbacks = false;
#ifdef NDEBUG
			bool InitializeRilD()
			{
				mSecRilLibHandle = dlopen(authModel.Handle.c_str(), RTLD_LAZY);
				if (!mSecRilLibHandle)
				{
					return false;
				}
				CloseClient_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, authModel.CloseClientRild.c_str());
				Connect_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, authModel.ConnectRild.c_str());
				isConnected_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, authModel.IsConnectedRild.c_str());
				InvokeOemRequestHookRaw = (int (*)(HRilClient client, char *data, size_t len)) dlsym(mSecRilLibHandle, authModel.InvokeOemRequestHookRaw.c_str());
				OpenClient_RILD = (HRilClient (*)(void)) dlsym(mSecRilLibHandle, authModel.OpenClientRild.c_str());
				Disconnect_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, authModel.DisconnectRild.c_str());
				RegisterErrorCallback = (int (*)(HRilClient client, RilOnError callback, void *data)) dlsym(mSecRilLibHandle, authModel.RegisterErrorCallback.c_str());
				RegisterRequestCompleteHandler = (int (*)(HRilClient client, uint32_t id, RilOnComplete callback)) dlsym(mSecRilLibHandle, authModel.RegisterRequestCompleteHandler.c_str());
				RegisterUnsolicitedHandler = (int (*)(HRilClient client, uint32_t id, RilOnUnsolicited callback)) dlsym(mSecRilLibHandle, authModel.RegisterUnsolicitedHandler.c_str());
				ModemAPI_Send_request = (int (*)(HRilClient client, char *buffer, size_t unknown, size_t buffer_length, size_t id)) dlsym(mSecRilLibHandle, authModel.ModemApiSendRequest.c_str());

				if (!CloseClient_RILD  || !Connect_RILD || !isConnected_RILD || !InvokeOemRequestHookRaw || !OpenClient_RILD || !Disconnect_RILD )
				{
          dlclose(mSecRilLibHandle);
          mSecRilLibHandle = NULL;
					return false;
	      }
				else
				{
          mClient = OpenClient_RILD();
          if (!mClient)
					{
            dlclose(mSecRilLibHandle);
            mSecRilLibHandle = NULL;
						return false;
          }
	      }
				if (RegisterErrorCallback(mClient, cb_RequestError, NULL) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				if (RegisterRequestCompleteHandler(mClient, RIL_REQUEST_OEM_HOOK_RAW, cb_RequestComplete) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				if (RegisterUnsolicitedHandler(mClient, RIL_UNSOL_UART, cb_RequestUnsolicited) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false; //11008
				}
				if (RegisterUnsolicitedHandler(mClient, 11008, cb_RequestUnsolicited) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false; //11008
				}
				IsCallbacks = true;
				return SanityCheck();
			}
			#else
			bool InitializeRilD()
			{
				mSecRilLibHandle = dlopen("libsecril-client.so", RTLD_LAZY);
				if (!mSecRilLibHandle)
				{
				std::cout << "`0" << std::endl;
					return false;
				}
				std::cout << "`1" << std::endl;
				CloseClient_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, "CloseClient_RILD");
				Connect_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, "Connect_RILD");
				isConnected_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, "isConnected_RILD");
				InvokeOemRequestHookRaw = (int (*)(HRilClient client, char *data, size_t len)) dlsym(mSecRilLibHandle, "InvokeOemRequestHookRaw");
				OpenClient_RILD = (HRilClient (*)(void)) dlsym(mSecRilLibHandle, "OpenClient_RILD");
				Disconnect_RILD = (int (*)(HRilClient)) dlsym(mSecRilLibHandle, "Disconnect_RILD");
				RegisterErrorCallback = (int (*)(HRilClient client, RilOnError callback, void *data)) dlsym(mSecRilLibHandle, "RegisterErrorCallback");
				RegisterRequestCompleteHandler = (int (*)(HRilClient client, uint32_t id, RilOnComplete callback)) dlsym(mSecRilLibHandle, "RegisterRequestCompleteHandler");
				RegisterUnsolicitedHandler = (int (*)(HRilClient client, uint32_t id, RilOnUnsolicited callback)) dlsym(mSecRilLibHandle, "RegisterUnsolicitedHandler");
				ModemAPI_Send_request = (int (*)(HRilClient client, char *buffer, size_t unknown, size_t buffer_length, size_t id)) dlsym(mSecRilLibHandle, "ModemAPI_Send_request");

				std::cout << "`2" << std::endl;
				if (!CloseClient_RILD  || !Connect_RILD || !isConnected_RILD || !InvokeOemRequestHookRaw || !OpenClient_RILD || !Disconnect_RILD )
				{
          			dlclose(mSecRilLibHandle);
          			mSecRilLibHandle = NULL;
					return false;
				}
				else
				{
				std::cout << "`4" << std::endl;
         			mClient = OpenClient_RILD();
          			if (!mClient)
					{
           				dlclose(mSecRilLibHandle);
            			mSecRilLibHandle = NULL;
						return false;
          			}
	      		}
				if (RegisterErrorCallback(mClient, cb_RequestError, NULL) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				if (RegisterRequestCompleteHandler(mClient, RIL_REQUEST_OEM_HOOK_RAW, cb_RequestComplete) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				if (RegisterUnsolicitedHandler(mClient, RIL_UNSOL_UART, cb_RequestUnsolicited) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false; //11008
				}
				if (RegisterUnsolicitedHandler(mClient, 11008, cb_RequestUnsolicited) != RIL_CLIENT_ERR_SUCCESS)
				{
					return false; //11008
				}
				IsCallbacks = true;
				return SanityCheck();
			}
			#endif

			void UnloadRilD()
			{
				RegisterErrorCallback(mClient, NULL, NULL);
				RegisterRequestCompleteHandler(mClient, RIL_REQUEST_OEM_HOOK_RAW, NULL);
				RegisterUnsolicitedHandler(mClient, RIL_UNSOL_UART, NULL);
				Disconnect_RILD(mClient);
				CloseClient_RILD(mClient);
				mClient = NULL;
			}

			bool SanityCheck(bool reconnect = false)
			{
				int iOriginalSTDIN_FILENO, iOriginalSTDOUT_FILENO, iOriginalSTDERR_FILENO = -1;
				UtilsClass::RedirectStandardStreamsToDEVNULL(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
				int state = isConnected_RILD(mClient);
				if (state != 0 && state != 1)
				{
					UtilsClass::RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
					//invalid or unkown state
					return false;
				}
				if (state == 1)
				{
					UtilsClass::RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
					//we are already connected.
					return true;
				}

				int success = setreuid(0, 1001u);
				auto start = std::chrono::high_resolution_clock::now();
				do
				{
					state = Connect_RILD(mClient);
					if (state == RIL_CLIENT_ERR_SUCCESS )
					{
						UtilsClass::RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
						return true;
					}
					auto timeNow = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = timeNow - start;
					if (elapsed.count() > 30000) //30 secs
					{
						UtilsClass::RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
						return false;
					}
					std::this_thread::sleep_for(std::chrono::seconds(1));
				} while(reconnect);
				UtilsClass::RestoreStandardStreams(&iOriginalSTDIN_FILENO, &iOriginalSTDOUT_FILENO, &iOriginalSTDERR_FILENO);
				return false;
			}

			inline static std::mutex mtx;
			inline static std::condition_variable cv;

			static void cb_RequestError(void *data, int error)
			{
				RilError = error;
				cv.notify_all();
			}

			static void cb_RequestComplete(HRilClient client, const void *data, int datalen)
			{
				Reply = std::vector<char>((char*)data, (char*)data + datalen);
				cv.notify_all();
			}

			static void cb_RequestUnsolicited(HRilClient client, const void *data, int datalen)
			{
				if (!IsAtCommand)
				{
					Reply = std::vector<char>((char*)data, (char*)data + datalen);
					return;
				}
//std::cout << (char*)data << std::endl;
				std::vector<char> reply = std::vector<char>((char*)data, (char*)data + datalen);
				Reply.insert(Reply.end(), reply.begin(), reply.end());
			}

			std::string ReceiveAtCommand(int timeoutMs = 5)
			{
				auto start = std::chrono::high_resolution_clock::now();
				while(true)
				{
					std::string received{ Reply.begin(), Reply.end() };
					auto timeNow = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = timeNow - start;
					if (elapsed.count() > timeoutMs || UtilsClass::endsWith(received, "\r\nOK\r\n") || UtilsClass::endsWith(received, "\r\nERROR\r\n"))
					{
						Reply.clear();
						IsAtCommand.exchange(false);
						return received;
					}
				}
			}


			RIL_Errno ReceiveError(int timeout = 55)
			{
				int errorNo = 0;
				std::unique_lock<std::mutex> lck(mtx);
				cv.wait_for(lck,std::chrono::seconds(timeout));
				errorNo = RilError;
				RilError = 0;
				return (RIL_Errno)errorNo;
			}


			std::tuple<int, std::vector<char>> ReceiveBuffer(int timeout = 15)
			{
				int errorNo = 0;
				std::unique_lock<std::mutex> lck(mtx);
				cv.wait_for(lck, std::chrono::seconds(timeout));
				errorNo = RilError;
				std::vector<char> reply(Reply.size());
				std::copy(Reply.begin(), Reply.end(), reply.begin());
				Reply.clear();
				RilError = 0;
				return std::make_tuple(errorNo, reply);
			}

			int InvokeOemRequestHookRawClear(HRilClient client, char *data, size_t size)
			{
				int result = InvokeOemRequestHookRaw(client, data, size);
				UtilsClass::ClearLogLoop();
				return result;
			}

    public:
	#ifdef NDEBUG
			IRilConnectionDirectSamsung(const AuthModel& authModel_) : IRilConnectionSamsung(authModel_)
			{
				_initialized = InitializeRilD();
			}
			#else
			IRilConnectionDirectSamsung() : IRilConnectionSamsung()
			{
				_initialized = InitializeRilD();
			}
			#endif

			~IRilConnectionDirectSamsung()
			{
				UnloadRilD();
			}

			bool Initialized()
			{
				std::cout << "DIRECT: " << _initialized << std::endl;
				return _initialized;
			}

      		bool ReadSpc(std::string& spc, std::string& otksl)
      		{
				spc = std::string();
				otksl = std::string();
				if (!_initialized) return false; // debating on print errors;
				const char CFG_CMD = 0x51;
				const char CFG_MSL_INFO = 0x02;
				if (!SanityCheck(true))
				{
					return false;
				}
				char data[5] = {0,};
				data[0] = CFG_CMD;
				data[1] = CFG_MSL_INFO;
				data[2] = 0;
				data[3] = 5;
				data[4] = 1;

				int ret = InvokeOemRequestHookRaw(mClient, data, sizeof(data));
				if (ret != RIL_CLIENT_ERR_AGAIN && ret != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				auto [ errorNo, buffer ] = ReceiveBuffer();
				UtilsClass::ClearLogLoop();
				if (errorNo != 0)
				{
					buffer.clear();
					return false;
				}

				if (buffer.size() > 0)
				{
					std::string str{ buffer.begin(), buffer.end() };
					spc = str.substr(0,6);
					otksl = str.substr(6,6);
					buffer.clear();
					return true;
				}
				else
				{
					buffer.clear();
					return false;
				}
      		}

			bool SendAtComamnd(const std::string in)
			{
				//int totalSize;
				std::string request = CreateAtRequest(in);
				if (!SanityCheck())
				{
					return false;
				}
				IsAtCommand.exchange(true);
				bool success = InvokeOemRequestHookRawClear(mClient, (char*)request.c_str(), request.length());
				//free(request);
				request.clear();
				return true;
			}

			bool RunAtCommand(const std::string command, std::string & message)
			{
				message = std::string();
				std::string filter = CreateFilter(command);
				if(!SendAtComamnd(command))
				{
					message = "Eror on sending command!";
					return false;
				}
				std::string received = ReceiveAtCommand();
				//std::string received{ buffer.begin(), buffer.end() };
				message = MatchAtCommand(received);
				//UtilsClass::ClearLogLoop();
				//UtilsClass::ExitIf();
				return true;
			}

			bool IsRadioOn(bool reconnect = false)
			{
				if (!_initialized) return false; // debating on print errors;
				const char CFG_CMD = 0x11;
				const char CFG_MSL_INFO = 0x11;
				if (!SanityCheck(reconnect))
				{
					return false;
				}
				return true;
				/*
				char data[5] = {0,};
				data[0] = CFG_CMD;
				data[1] = CFG_MSL_INFO;
				data[2] = 0;
				data[3] = 5;
				data[4] = 0;

				int ret = InvokeOemRequestHookRawClear(mClient, data, sizeof(data));
				if (ret != RIL_CLIENT_ERR_AGAIN && ret != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				int errorNo = (int)ReceiveError();
				return errorNo == 0;
				*/
			}

			bool ResetModem()
			{
				if (!_initialized) return false; // debating on print errors;
				if (!SanityCheck())
				{
					return false;
				}

				const std::string command(UtilsClass::HexToString("11670004"));
				//const char buffer[4]={0x11, 0x67, 0x00, 0x04};
				int ret = InvokeOemRequestHookRaw(mClient, (char*)command.c_str(), command.length());
				if (ret != RIL_CLIENT_ERR_AGAIN && ret != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				int errorNo = (int)ReceiveError();
				return errorNo == RIL_E_SUCCESS || errorNo == 4;
			}

			bool InvokeOemRilRequestCommand(char* command, int size, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				if (!SanityCheck())
				{
					return false;
				}
				if (clearLogBefore)	UtilsClass::ClearLogLoop();
				int status = InvokeOemRequestHookRaw(mClient, command, size);
				if (!read) return true;

				auto [ errorNo, buffer ] = ReceiveBuffer();
				if (clearLogAfter) UtilsClass::ClearLogLoop();
				if (status == RIL_CLIENT_ERR_SUCCESS && errorNo == RIL_E_SUCCESS)
				{
					return buffer.size() == 0;
				}
				return false;
			}

			bool InvokeCommand(std::string command, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				return InvokeOemRilRequestCommand((char*)command.c_str(), command.length(), clearLogBefore, clearLogAfter, read);
			}

			void CrashMe()
			{
				const std::string command(UtilsClass::HexToString("07170004"));
				InvokeCommand(command);
			}

			bool CpCrash()
			{
				const std::string command(UtilsClass::HexToString("07170004"));
				bool flag = InvokeCommand(command, command.length(), false);
				if (flag)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			bool CpCrashByAt()
			{
				return SendAtComamnd("AT+FAILDUMP=0,1");
			}

			int GetSprintUnlockState()
			{
				if (!_initialized) return false; // debating on print errors;
				const char CFG_CMD = 0x51;
				const char CFG_MSL_INFO = 0x42;
				int ret;
				if (!SanityCheck())
				{
					return false;
				}
				char data[4] = {0,};
				data[0] = CFG_CMD;
				data[1] = CFG_MSL_INFO;
				data[2] = 0;
				data[3] = 4;

				ret = InvokeOemRequestHookRawClear(mClient, data, sizeof(data));
				if (ret != RIL_CLIENT_ERR_AGAIN && ret != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				auto [ errorNo, buffer ] = ReceiveBuffer();
				int state = -1;
				if (buffer.size() > 0)
				{
					state = buffer[0];
				}
				buffer.clear();
				return state;
			}

			bool SetSprintUnlockState(int state = 2)
			{
				const char CFG_CMD = 0x51;
				const char CFG_MSL_INFO = 0x41;

				if (!SanityCheck())
				{
					return false;
				}
				char data[5] = {0,};
				data[0] = CFG_CMD;
				data[1] = CFG_MSL_INFO;
				data[2] = 0;
				data[3] = 5;
				data[4] = state;

				int ret = InvokeOemRequestHookRaw(mClient, data, sizeof(data));
				if (ret != RIL_CLIENT_ERR_AGAIN && ret != RIL_CLIENT_ERR_SUCCESS)
				{
					return false;
				}
				auto [ errorNo, buffer ] = ReceiveBuffer();
				return errorNo == RIL_E_SUCCESS || errorNo ==4;
			}

			bool QcWipe()
			{
				const std::string command1(UtilsClass::HexToString("0101000804012700"));
				const std::string command2(UtilsClass::HexToString("01030007043100"));
				const std::string command3(UtilsClass::HexToString("01030007043200"));
				InvokeCommand(command1);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				InvokeCommand(command2);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				InvokeCommand(command3);
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return true;
			}

			bool SprRtn()
			{
				const std::string command(UtilsClass::HexToString("0C01000502"));
				InvokeCommand(command);
				return true;
			}

			bool QcReset()
			{
				const std::string command1(UtilsClass::HexToString("0101000804012700"));
				const std::string command2(UtilsClass::HexToString("01030007043300"));
				InvokeCommand(command1);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				InvokeCommand(command2);
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return true;
			}

			bool ExynosDump(std::string blobData, std::string& response)
			{
				response = std::string();
				if (!SanityCheck())
				{
					return false;
				}
				char UM[4]={0x07, 0x17, 0x00, 0x04};
				char SK[5] ={0x11, 0x63, 0x00, 0x05, 0x01};
				InvokeOemRequestHookRaw(mClient, SK, sizeof(SK));
        		system("logcat -b all -c");
				system("logcat -b all -c");
				for (int i=0; i<25;i++)
				{
			    	system("logcat -b all -c");
					InvokeOemRequestHookRaw(mClient, (char*)blobData.c_str(), blobData.length());
				}
				system("logcat -b all -c");
				InvokeOemRequestHookRaw(mClient, UM, sizeof(UM));
				//usleep(500000);

				std::string filePath = UtilsClass::GetFileNameFromPath();
				if (filePath.empty())
				{
					return false;
				}

				const char rawPattern[17] = {0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,0x05, 0x00, 0x00, 0x00, 0x03};
				std::string pattern;
				pattern.assign(rawPattern, 17);
				long long offset = UtilsClass::FindOffsetOfNeedleInFile(filePath, pattern);
				while(offset < 0)
				{
					// Record start time
					auto start = std::chrono::high_resolution_clock::now();
					offset = UtilsClass::FindOffsetOfNeedleInFile(filePath, pattern);
					auto timeNow = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = timeNow - start;
					if (elapsed.count() > 35)
						break;
					if (offset > 0)
						break;
				}

				if (offset < 0)
				{
					return false;
				}


				std::ifstream fileStream;
				fileStream.open(filePath, std::ios::binary);
				if(!fileStream)
				{
					return false;
				}

				std::vector<char> result(1024);
				fileStream.seekg(offset, fileStream.beg);
				fileStream.read(&result[0], 1024);
				std::string readData(result.data(), 1024);
				response.append("raw:");
				response.append(UtilsClass::StringToHex(readData, true));
				return true;
			}

			bool SetCsc(const std::string salesCode, const std::string salesCountry = std::string())
			{
				if (!SanityCheck())
				{
					return false;
				}
				std::string request = CreateCscRequest(salesCode, salesCountry);
				bool flag = InvokeCommand(request);
				if (flag)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			bool GetSprintNonce(std::string& nonce){
				return true;
			}
  };
}

#endif