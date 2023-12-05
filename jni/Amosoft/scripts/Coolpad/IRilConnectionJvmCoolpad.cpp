#ifndef IRILCONNECTIONJVMCOOLPAD_H
#define IRILCONNECTIONJVMCOOLPAD_H

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
#include "IRilConnectionCoolpad.cpp"


using namespace std::chrono_literals;
using namespace android;

class IRilConnection;

namespace Amosoft
{
	class IRilConnectionJvmCoolpad : public Amosoft::IRilConnectionCoolpad
	{
    private:
			inline static String16 Phone;
			inline static String16 DESCRIPTOR;
			inline static String16 AndroidPhone;
			int invokeCode;
			int deviceCode;
			sp<IServiceManager> sm;
			sp<IBinder> phone;		
			JniInvocation jni;	

            
			/// <summary>
			/// Initialize service manager & phone service and Check compatibility
			/// </summary>
			bool InitializeServiceManager()
			{
				int errorNo = 0;
				bool success = false;			
                jni.Init(nullptr);
                success = jni.FindCode1(&invokeCode, &deviceCode);                  			
                jni.~JniInvocation(); //close libraries no longer needed.
                if (!success)
                {
                    return false;
                }
                else if (invokeCode <= 0 )
                {
                    return false;
                }				
                else if (deviceCode < 0)
                {
                    return false;
                }

                #ifndef NDEBUG
                std::cout << "Invoke Code: " << invokeCode << std::endl << "Device Code " << deviceCode << std::endl;
                #endif
			  	
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
				if (errorNo == 0)
				{
                    return ReadImeiData();
				}
				return true;
			}


    public:
			#ifdef NDEBUG
			IRilConnectionJvmCoolpad(const AuthModel& authModel_) : IRilConnection(authModel_)
			{
				DESCRIPTOR = String16(authModel.Descriptor.c_str());
				Phone = String16(authModel.Phone.c_str());
				AndroidPhone = String16(authModel.AndroidPhone.c_str());
				_initialized = InitializeServiceManager(true);
			}
			#else
			IRilConnectionJvmCoolpad()
			{
				DESCRIPTOR = String16("com.android.internal.telephony.ITelephony");
				Phone = String16("phone");
				AndroidPhone = String16("com.android.phone");
				_initialized = InitializeServiceManager();
			}
			#endif
			
			~IRilConnectionJvmCoolpad()
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

            
			bool ReadSpc(std::string& spc, std::string& otksl)
            {
                spc = std::string();
                otksl = std::string();
				Parcel parcel, reply;
				const std::string command(UtilsClass::HexToString("160000021100000000000000303030303030601f000000000000000000000000000000"));
    	
				unsigned char buffer[1024]{};
				parcel.writeInterfaceToken(DESCRIPTOR);
                parcel.writeInt32(0); //subid
				parcel.writeByteArray(command.length(), reinterpret_cast<const unsigned char*>(command.c_str()));	
				parcel.writeByteArray(1024,  reinterpret_cast<const unsigned char*>(buffer));	
			
										
				int status = phone->transact(invokeCode, parcel, &reply, 0);
		
				return status;
            }


			bool GetSprintNonce(std::string& nonce)
			{				
                Parcel data, reply;
                data.writeInterfaceToken(DESCRIPTOR);
                data.writeInt32(1);
                data.write(0x00, 1);
                data.writeInt32(255);
                int status = phone->transact(invokeCode, data, &reply, 0);
				return false;
			}


  };
}


#endif