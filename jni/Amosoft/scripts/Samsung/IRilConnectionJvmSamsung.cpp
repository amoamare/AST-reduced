#ifndef RILCONNECTIONJVMSAMSUNG_H
#define RILCONNECTIONJVMSAMSUNG_H

#include "models/AuthModel.cpp"

#include <arpa/inet.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n
#include <sstream>
#include <arpa/inet.h>
#include <time.h>
#include <vector>
#include <random>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <iostream>

#include <future>
#include <thread>
#include <chrono>
#include <pthread.h>



#include <utils/String8.h>
#include <utils/String16.h>
#include <binder/Binder.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#include <binder/TextOutput.h>

#include "models/AuthModel.cpp"
#include "IRilConnection.cpp"
#include "ThreadRunner.cpp"
#include "../Jni/JniInvocation.h"
#include "IRilConnectionSamsung.cpp"


using namespace std::chrono_literals;

#define B_PACK_CHARS(c1, c2, c3, c4) \
    ((((c1)<<24)) | (((c2)<<16)) | (((c3)<<8)) | (c4))

using namespace android;

class IRilConnection;

namespace Amosoft
{
	class IRilConnectionJvmSamsung : public Amosoft::IRilConnectionSamsung
	{
    private:
			inline static String16 Phone;
			inline static String16 DESCRIPTOR;
			inline static String16 USBDESCRIPTOR = String16("android.hardware.usb.IUsbManager");
			inline static String16 AndroidPhone;
			int invokeCode;
			int sendRequestCode;
			int deviceCode;
			int usbMangerCode;
			bool isSendToRilquest;
			sp<IServiceManager> sm;
			sp<IBinder> phone;		
			sp<IBinder> usbsvc;	
			JniInvocation jni;
			

			//On latest models invoke is patched.
			//we will test invoke if it returns false on activation date we will assume its patched.
			//if patched we set the flag for IsSendToRilRequest to true.
			void CheckIfInvokePatched()
			{
				if (isSendToRilquest) return; // if already set to true no need to check
				const std::string command(UtilsClass::HexToString("11A00004"));
				bool success = InvokeCommand(command);
				isSendToRilquest = !success;
				#ifndef NDEBUG
				std::cout << "IsSendToRilRequest: " << isSendToRilquest << std::endl;
				#endif
			}

			bool InitializeServiceManagerBySamsung(bool findCode = false)
			{
				int errorNo = 0;
				bool success = false;
				if (findCode)
				{
					jni.Init(nullptr);
					success = jni.FindCode(&invokeCode, &deviceCode, &sendRequestCode, &usbMangerCode);					
					jni.~JniInvocation(); //close libraries no longer needed.
					if (!success)
					{
						return false;
					}
					else if (invokeCode <= 0 && sendRequestCode <= 0)
					{
						return false;
					}				
					else if (deviceCode < 0)
					{
						return false;
					}
					isSendToRilquest = (invokeCode <=0 && sendRequestCode > 0);
					#ifndef NDEBUG
					std::cout << "Invoke Code: " << invokeCode << std::endl << "Device Code " << deviceCode << std::endl <<
					"SendToRilCode: " << sendRequestCode << std::endl << "IsSendToRilRequest: " << isSendToRilquest << std::endl;
					#endif
			  	}
				sm = defaultServiceManager();
				if (sm == NULL)
				{
					return false;
				}
				phone = sm->getService(Phone);
				if (phone == NULL)
				{
					return false;
				}
				usbsvc = sm->getService(String16("usb"));
				if (errorNo == 0)
				{
					CheckIfInvokePatched();
				//	if (UtilsClass::GetSystemApiLevel() > 21)
				//	{
						//CheckIfInvokePatched();
						return ReadImeiData();
				//	}
					//return true;
				}
				return false;
			}

			/// <summary>
			/// Initialize service manager & phone service and Check compatibility
			/// </summary>
			bool InitializeServiceManager(bool findCode = false)
			{
				
				string manufacturer;
				if(UtilsClass::GetDeviceManufacturer(manufacturer))
				{
					std::cout << manufacturer << std::endl;
				}
				if (UtilsClass::CaseInSensStringCompare(manufacturer, "samsung"))
				{
					return InitializeServiceManagerBySamsung(findCode);
				}

				return false;
			}

    public:
			#ifdef NDEBUG
			IRilConnectionJvmSamsung(const AuthModel& authModel_) : IRilConnection(authModel_)
			{
				DESCRIPTOR = String16(authModel.Descriptor.c_str());
				Phone = String16(authModel.Phone.c_str());
				AndroidPhone = String16(authModel.AndroidPhone.c_str());
				_initialized = InitializeServiceManager(true);
			}
			#else
			IRilConnectionJvmSamsung()
			{
				DESCRIPTOR = String16("com.android.internal.telephony.ITelephony");
				Phone = String16("phone");
				AndroidPhone = String16("com.android.phone");
				_initialized = InitializeServiceManager(true);
			}
			#endif
			
			~IRilConnectionJvmSamsung()
			{
				jni.~JniInvocation();
			}

			bool Initialized()
			{
				return _initialized;
			}

			/// <summary>
			/// Read IMEI data using RIL
			/// </summary>
			bool ReadImeiData()
			{
				if (deviceCode <=0)return true;
				Parcel parcel;
				Parcel reply;
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeString16(AndroidPhone);
				//UtilsClass::ClearLogLoop();
				int status = phone->transact(deviceCode, parcel, &reply, 0);
				//UtilsClass::ClearLogLoop();
				if (status == NO_ERROR)
				{
					int32_t exceptionCode = reply.readExceptionCode();
					if(!exceptionCode)
					{
						Imei.assign(String8(reply.readString16()));
						if (Imei.length() > 14) Imei.substr(0, 14);
						return true;// !Imei.empty();
					}
				}
				return true;
			}

			bool InvokeOemRilRequestCommand(char* command, int size, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				Parcel parcel, reply;
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeInt32(size); //subscriber ID
				parcel.write(command, size);
				parcel.writeInt32(1024);
				if (clearLogBefore)	UtilsClass::ClearLogLoop();
				int status = phone->transact(invokeCode, parcel, &reply, 0);
				if (clearLogAfter) UtilsClass::ClearLogLoop();
				if (!read) return true;
				if (status == OK)
				{
					if (reply.readExceptionCode() == OK)
					{
						if (reply.readInt32() == OK)
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			
			bool InvokeSendToRilRequestCommand(char* command, int size, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{
				Parcel parcel, reply;			
				unsigned char buffer[4096]{};
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeByteArray(size, reinterpret_cast<const unsigned char*>(command));				
				parcel.writeByteArray(4096, reinterpret_cast<const unsigned char*>(&buffer));
				parcel.writeInt32(6); // 1 or 6 hmm 								
				if (clearLogBefore)	UtilsClass::ClearLogLoop();
				int status = phone->transact(sendRequestCode, parcel, &reply, 0);
				if (clearLogAfter) UtilsClass::ClearLogLoop();
				if (!read) return true;
				if (status == OK)
				{
					if (reply.readExceptionCode() == OK)
					{
						if (reply.readInt32() == OK)
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			bool InvokeCommand(std::string command, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true)
			{	
				bool success;
				isSendToRilquest = true;
				if (isSendToRilquest)
				{					
					success = InvokeSendToRilRequestCommand((char*)command.c_str(), command.length(), clearLogBefore, clearLogAfter, read);
				}
				else
				{
					success = InvokeOemRilRequestCommand((char*)command.c_str(), command.length(), clearLogBefore, clearLogAfter, read);
				}
				return success;
			}
			
			int WriteParcel(Parcel& parcel, sp<IBinder>& phone, int requestCode)
			{
				return phone->transact(sendRequestCode, parcel, nullptr, 0);
			}

			void SetFunction()
			{
				std::cout << "DM CODE: " << usbMangerCode << std::endl;
				Parcel parcel;
				Parcel reply;
				parcel.writeInterfaceToken(USBDESCRIPTOR);
				parcel.writeString16(String16("dm"));	
		    	parcel.writeInt32(1); // transact command
				usbsvc->transact(usbMangerCode, parcel, &reply, 0);
			}

			void ResetModem1()
			{
				Parcel parcel;
				const std::string command(UtilsClass::HexToString("11670004"));
				unsigned char* buffer[0]{};
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeByteArray(command.length(), reinterpret_cast<const unsigned char*>(command.c_str()));	
				parcel.writeByteArray(0, reinterpret_cast<const unsigned char*>(&buffer));
		    	parcel.writeInt32(6); // transact command
				phone->transact(sendRequestCode, parcel, nullptr, 0);
			}

			int InvokeCommandWithRead(std::string command, Parcel& reply, bool clearLogBefore = true, bool clearLogAfter = true)
			{
				Parcel parcel;			
				unsigned char buffer[1024]{};
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeByteArray(command.length(), reinterpret_cast<const unsigned char*>(command.c_str()));		
				if (isSendToRilquest)
				{			
					parcel.writeByteArray(1024, reinterpret_cast<const unsigned char*>(&buffer));
					parcel.writeInt32(6); // 1 or 6 hmm 
				}
				else
				{
					parcel.writeInt32(1024);
				}
										
				if (clearLogBefore)	UtilsClass::ClearLogLoop();
				int status = phone->transact(sendRequestCode, parcel, &reply, 0);
				if (clearLogAfter) UtilsClass::ClearLogLoop();
				return status;
			}

			bool ReadSpc(std::string& spc, std::string& otksl)
			{
				spc = std::string();
				otksl = std::string();
				const std::string command(UtilsClass::HexToString("5102000501"));
				Parcel reply;
				int status = InvokeCommandWithRead(command, reply);
				if (status == OK)
				{
					int32_t exceptionCode = reply.readExceptionCode();
					if(exceptionCode == NO_ERROR)
					{
						if (isSendToRilquest)
						{
							int32_t strSize = reply.readInt32();
							std::cout << strSize << std::endl;
							if (strSize > 0)
							{
								int bufferSize = reply.readInt32();
								const char* data = reply.readCString();								
								std::string str;
								str.assign(data);
								spc = str.substr(0,6);
								otksl = str.substr(6,6);
								return true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							if (reply.readExceptionCode() == OK)
							{
								int32_t strSize = reply.readInt32();
								//std::cout << "strSize: " << strSize << std::endl;
								int32_t bufferSize = reply.readInt32();
								//std::cout << "unkown: " << unkown << std::endl;
								const char* data = reply.readCString();
								//std::cout << "data: " << data << std::endl;
								if (strSize > 11)
								{
									std::string str;
									str.assign(data);
									spc = str.substr(0,6);
									otksl = str.substr(6,6);
									return true;
								}
							}
							else
							{
								return false;
							}
						}
					}
					else
					{
						return false;
					}					
				}
				else
				{
					return false;
				}
				return false;
			}

			/*

			int invokeCommand(std::string command, Parcel& reply, bool clearLogBefore = true, bool clearLogAfter = true, bool readResponse = true)
			{
				Parcel parcel;
				unsigned char* buffer[0]{};
				std::cout << phone->getInterfaceDescriptor() << std::endl;
				parcel.writeInterfaceToken(phone->getInterfaceDescriptor());
				parcel.writeInt32(0);
				parcel.writeInt32(0);
				parcel.writeByteArray(command.length(), reinterpret_cast<const unsigned char*>(command.c_str()));
				if (isSendToRilquest)
				{
					if (readResponse)
					{
				//		parcel.writeByteArray(0, reinterpret_cast<const unsigned char*>(&buffer));
					}
					else
					{
				//		parcel.writeInt32(-1);
					}
					//insert transact command.
				//	parcel.writeInt32(6); // transact command

					


					
					auto first = std::thread([this, &p = parcel]() { WriteParcel(p, phone, sendRequestCode); });
					pthread_setname_np(first.native_handle(), "com.android.phone");
					first.detach();
					first.~thread();

					std::this_thread::sleep_for(5s);
					return 1;
					
					//return phone->transact(sendRequestCode, parcel, nullptr, 0  | FLAG_CLEAR_BUF);	
				return 1;				
				}
				else
				{
					parcel.writeByteArray(sizeof(buffer), reinterpret_cast<const unsigned char*>(&buffer));				
					return phone->transact(invokeCode, parcel, &reply, 0);
				}
				
			}

			
			



			/// <summary>
			/// Read SPC from raw ril
			/// </summary>
			bool ReadSpc(std::string& spc, std::string& otksl)
			{
				isSendToRilquest = true;
				spc = std::string();
				otksl = std::string();
				//const std::string command(UtilsClass::HexToString("5102000501"));
				const std::string command = CreateAtRequest("AT+MSLSECUR=1,1");	
				Parcel reply;
				int status;
				
				for(int x = 0; x < 100; x++)				{
					std::cout << "COUNT: " << x << std::endl;
				status = invokeCommand(command, reply);
				std::cout << "st: " << status << std::endl;
				if (x % 5 == 0)
				{

					std::cout << "Restting modem" <<std::endl;
			//ResetModem1();		
					//std::this_thread::sleep_for(10s);
//phone = nullptr;
//sm = nullptr;
					std::cout << "Service manager" <<std::endl;
//InitializeServiceManager(false);

					std::this_thread::sleep_for(10s);
				}
				//std::this_thread::sleep_for(2000ms);
				};
				std::string str;
				std::cout << "Status: " << status << std::endl;
				if (status == NO_ERROR)
				{
					int32_t exceptionCode = reply.readExceptionCode();
				std::cout << "exceptionCode: " << exceptionCode << std::endl;
					if(!exceptionCode)
					{
						int32_t strSize = reply.readInt32();
				std::cout << "strSize: " << strSize << std::endl;
						int32_t unkown = reply.readInt32();
				std::cout << "unkown: " << unkown << std::endl;
				
						const char* data = reply.readCString();
				std::cout << "data: " << data << std::endl;
						if (strSize > 11)
						{
							str.assign(data);
							spc = str.substr(0,6);
							otksl = str.substr(6,6);
							return true;
						}
					}
				}	
							
				return false;




				
				const int length = command.length();
				Parcel parcel, reply;
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeInt32(length);
				parcel.write(command.c_str(), length);
				parcel.writeInt32(64);
				UtilsClass::ClearLogLoop();
				int status = phone->transact(invokeCode, parcel, &reply, 0);
				UtilsClass::ClearLogLoop();
				std::string str;
				std::cout << "Status: " << status << std::endl;
				if (status == NO_ERROR)
				{
					int32_t exceptionCode = reply.readExceptionCode();
				std::cout << "exceptionCode: " << exceptionCode << std::endl;
					if(!exceptionCode)
					{
						int32_t strSize = reply.readInt32();
				std::cout << "strSize: " << strSize << std::endl;
						int32_t unkown = reply.readInt32();
				std::cout << "unkown: " << unkown << std::endl;
						const char* data = reply.readCString();
				std::cout << "data: " << data << std::endl;
						if (strSize > 11)
						{
							str.assign(data);
							spc = str.substr(0,6);
							otksl = str.substr(6,6);
							return true;
						}
					}
				}					
				return false;
				
			}
			*/
			

			bool SendAtComamnd(const std::string in)
			{
				std::string request = CreateAtRequest(in);
				InvokeCommand(request, true, false, false);
				request.clear();
				return true;
			}

			bool RunAtCommand(const std::string command, std::string & message)
			{
				message = std::string();
				std::string filter = CreateLogCatFilter(command);
				ThreadRunner::getInstance().SetPauseRunner(true);
				if(!SendAtComamnd(command))
				{
					message = "Eror on sending command!";
					return false;
				}

				std::string received;
				for(int i = 3; i != 0; --i)
				{
					std::string receivedEx = UtilsClass::exec_get_out(filter);
					if (!receivedEx.empty() || i < 2)
					{
						ThreadRunner::getInstance().SetPauseRunner(false);
						received = receivedEx;
						break;
					}
					if (receivedEx.empty() && i > 1)
					{
						std::this_thread::sleep_for(std::chrono::seconds(1));
						continue;
					}
				}
				ThreadRunner::getInstance().SetPauseRunner(false);
				message = MatchAtCommand(received);
				UtilsClass::ClearLogLoop();
				return !message.empty();
			}

			bool IsRadioOn(bool notused = false)
			{
				//const char buffer[5]={0x11, 0x11, 0x00, 0x05, 0x00};
				const std::string command(UtilsClass::HexToString("11670004"));
				const int length = command.length();
				Parcel parcel, reply;
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeInt32(length);
				parcel.write(command.c_str(), length);
				parcel.writeInt32(128);
				UtilsClass::ClearLogLoop();
				int status = phone->transact(invokeCode, parcel, &reply, 0);
				UtilsClass::ClearLogLoop();
				if (status == OK)
				{
					if (reply.readExceptionCode() == OK)
					{
						int t = reply.readInt32();
						if (t == OK)
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			bool ResetModem()
			{
				//const char buffer[4]={0x11, 0x67, 0x00, 0x04};
				const std::string command(UtilsClass::HexToString("11670004"));
				bool reset = InvokeCommand(command);
				if (!reset && UtilsClass::GetHardwareType() == 2)
				{
					reset = true;
				}
				return reset;
			}

			void CrashMe()
			{
				//const char buffer[4]={0x07, 0x17, 0x00, 0x04};
				const std::string command(UtilsClass::HexToString("07170004"));
				InvokeCommand(command);
			}

			bool CpCrash()
			{
				//const char buffer[4]={0x07, 0x17, 0x00, 0x04};
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

#pragma region Samsung Sprint Commands

			bool GetSprintNonce(std::string& nonce)
			{
				nonce = std::string(); //5102000501 //51180004
				const std::string command(UtilsClass::HexToString("5102000501"));
				Parcel reply;
				int status = InvokeCommandWithRead(command, reply);
				if (status == OK)
				{
					int32_t exceptionCode = reply.readExceptionCode();
					if(exceptionCode == NO_ERROR)
					{
						if (isSendToRilquest)
						{
							int32_t strSize = reply.readInt32();
							std::cout << strSize << std::endl;
							if (strSize > 0)
							{
								int bufferSize = reply.readInt32();
								std::cout << bufferSize  << std::endl;
								const char* data = reply.readCString();								
								std::string str;
								str.assign(data);
								nonce.assign(data);// = str.substr(0,6);
								return true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							if (reply.readExceptionCode() == OK)
							{
								int32_t strSize = reply.readInt32();
								std::cout << "strSize: " << strSize << std::endl;
								int32_t unkown = reply.readInt32();
								std::cout << "unkown: " << unkown << std::endl;
								const char* data = reply.readCString();
								std::cout << "data: " << data << std::endl;
								if (strSize > 11)
								{
									std::string str;
									str.assign(data);
									nonce = str.substr(0,6);
									//otksl = str.substr(6,6);
									return true;
								}
							}
							else
							{
								return false;
							}
						}
					}
					else
					{
						return false;
					}					
				}
				else
				{
					return false;
				}
				return false;
			}

			int GetSprintUnlockState()
			{
				const std::string command(UtilsClass::HexToString("51420004"));
				const int length = command.length();
				Parcel parcel, reply;
				parcel.writeInterfaceToken(DESCRIPTOR);
				parcel.writeInt32(length);
				parcel.write(command.c_str(), length);
				parcel.writeInt32(64);
				UtilsClass::ClearLogLoop();
				int status = phone->transact(invokeCode, parcel, &reply, 0);
				UtilsClass::ClearLogLoop();
				if (status == NO_ERROR)
				{
					int32_t exceptionCode = reply.readExceptionCode();
					if(!exceptionCode)
					{
						int32_t strSize = reply.readInt32();
						int32_t unkown = reply.readInt32();
						if (strSize > 0)
						{
							int32_t readState = reply.readInt32();
							return static_cast<int>(readState);
						}
					}
				}
				return -1;
			}

			bool SetSprintUnlockState(int state = 2)
			{
				const std::string command(UtilsClass::HexToString("51410005" + UtilsClass::ZeroPadNumber(state)));
				return InvokeCommand(command);
			}
			
#pragma endregion 

			bool QcWipe()
			{
				//const char mslReset1[8] = { 0x01,0x01,0x00,0x08,0x04,0x01,0x27,0x00 };
				//const char mslReset2[7] = { 0x01,0x03,0x00,0x07,0x04,0x31,0x00 };
				//const char mslReset3[7] = { 0x01,0x03,0x00,0x07,0x04,0x32,0x00 };
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

			bool QcReset()
			{
				//const char mslReset1[8] = { 0x01,0x01,0x00,0x08,0x04,0x01,0x27,0x00 };
				//const char mslReset2[7] = { 0x01,0x03,0x00,0x07,0x04,0x33,0x00 };
				const std::string command1(UtilsClass::HexToString("0101000804012700"));
				const std::string command2(UtilsClass::HexToString("01030007043300"));
				InvokeCommand(command1);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				InvokeCommand(command2);
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return true;
			}

			bool SprRtn()
			{
				//const char rtn[5] = {	0x0c, 0x01, 0x00, 0x05, 0x02 };
				const std::string command(UtilsClass::HexToString("0C01000502"));
				InvokeCommand(command);
				return true;
			}

			bool ExynosDump(std::string blobData, std::string& response)
			{
				response = std::string();
				unsigned char SK[5] ={0x11, 0x63, 0x00, 0x05, 0x01};
				unsigned char UM[4]={0x07, 0x17, 0x00, 0x04};
				Parcel data, reply;
				data.writeInterfaceToken(DESCRIPTOR);
				data.writeInt32(sizeof(SK));
				data.write(SK, sizeof(SK));
				data.writeInt32(32);
				phone->transact(invokeCode, data, &reply, 0);
				system("logcat -b all -c");
				system("logcat -b all -c");
				for (int i=0; i<25;i++)
				{
			    system("logcat -b all -c");
				  Parcel data2, reply2;
			    data2.writeInterfaceToken(DESCRIPTOR);
			    data2.writeInt32(blobData.length());
			    data2.write((char*)blobData.c_str(), blobData.length());
			    data2.writeInt32(32);
			    phone->transact(invokeCode, data2, &reply2, 0);
				}
				system("logcat -b all -c");
				Parcel data3, reply3;
				data3.writeInterfaceToken(DESCRIPTOR);
				data3.writeInt32(sizeof(UM));
				data3.write(UM, sizeof(UM));
				data3.writeInt32(32);
				phone->transact(invokeCode, data3, &reply3, 0);
				std::string filePath = UtilsClass::GetFileNameFromPath();
				if (filePath.empty())
				{
					return false;
				}

				const std::string rawPattern(UtilsClass::HexToString("8000000001000000020000000500000003"));
				long long offset = UtilsClass::FindOffsetOfNeedleInFile(filePath, rawPattern);
				while(offset < 0)
				{
					// Record start time
					auto start = std::chrono::high_resolution_clock::now();
					offset = UtilsClass::FindOffsetOfNeedleInFile(filePath, rawPattern);
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

  };
}


#endif