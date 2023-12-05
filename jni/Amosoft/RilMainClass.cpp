#ifndef RILMAINCLASS_H
#define RILMAINCLASS_H
#include "../branding.h"

#pragma region include Models and Ril Classes
#include <utils.cpp>
#include "models/AuthModel.cpp"
#include "models/SoftwareConfigModel.cpp"
#include "models/UnlockModel.cpp"
#include "Structs.cpp"
#include "Encryption.cpp"
#include "IRilConnection.cpp"
#include "scripts/IScript.cpp"
#pragma endregion 

#include "scripts/RemoveFrp.cpp"
#include "scripts/ReadIMEI.cpp"


#if APPTYPE == APPTYPE_SAMSUNG
#include "scripts/Samsung/IRilConnectionJvmSamsung.cpp"
#include "scripts/Samsung/IRilConnectionDirectSamsung.cpp"
#include "scripts/Samsung/ExynosMslCertBypass.cpp"
#include "scripts/Samsung/ExynosATI5Unlock.cpp"
#include "scripts/Samsung/ReadInfoScript.cpp"
#include "scripts/Samsung/ReadSpcScript.cpp"
#include "scripts/Samsung/ChangeCscScript.cpp"
#include "scripts/Samsung/QcomSstDefaultUnlock.cpp"
#include "scripts/Samsung/QcomWipe.cpp"
#include "scripts/Samsung/QcomS10CodeUnlock.cpp"
#include "scripts/Samsung/TmbQcomUploadScript.cpp"
#include "scripts/Samsung/TmbQcomUnlock.cpp"
#include "scripts/Samsung/TmbExynosBlobSearch.cpp"
#include "scripts/Samsung/SprQcomSstUnlock.cpp"
#include "scripts/Samsung/SprQcomCfgUnlock.cpp"
#include "scripts/Samsung/SprQcomS10Unlock.cpp"
#include "scripts/Samsung/OperationAT.cpp"
#include "scripts/Samsung/SetSoftwareConfiguration.cpp"
#include "scripts/Samsung/SprUnlockByNonce.cpp"

#elif APPTYPE == APPTYPE_MOTOROLA
#include "scripts/Motorola/IRilConnectionJvmMotorola.cpp"
#include "scripts/Motorola/ReadSpcScript.cpp"

#elif APPTYPE == APPTYPE_COOLPAD
#include "scripts/Coolpad/IRilConnectionJvmCoolpad.cpp"
#include "scripts/Coolpad/ReadSpcScript.cpp"
#endif


using std::atomic;
using std::thread;
using std::endl;
using namespace Amosoft::Scripts;

namespace Amosoft
{
	class RilMainClass
	{
		private:
			atomic<bool> _forceJvm = false;
			IRilConnection *rilDirect;
			IRilConnection	*rilJvm;
			
			#ifdef NDEBUG
			const AuthModel& authModel;
			#endif

			Encryption encryption;
			string imei;
			inline static const string StrFail = "FAIL";
			inline static const string StrOk = "OK";

#ifdef NDEBUG
			bool Initialize()
			{
				const string operation("Initialized:");
				cout << "[*] Initializing" << endl;
				rilJvm = new RilConnectionJvm(authModel);
				bool jvmInitialized = rilJvm->Initialized();
				_forceJvm = authModel.ForceJvm;
				_forceJvm = true;
/* if we force JVM we still want to see if we can register to call backs. This way we do not need CP or Debug HIGH */

				rilDirect = new RilConnectionDirect(authModel);
				bool directInitialized = rilDirect->Initialized();
//cout << "Type: " << directInitialized << endl;
				if (!directInitialized && !jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}
				if (directInitialized && !jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}
				if (!jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}

				cout << operation << StrOk << endl;
				return true;
			}
#endif
			


#if APPTYPE == APPTYPE_SAMSUNG
			bool Initialize()
			{
				const string operation("Initialized:");
				cout << "[*] Initializing" << endl;	
				rilJvm = new IRilConnectionJvmSamsung();
				bool jvmInitialized = rilJvm->Initialized();			
				rilDirect = new IRilConnectionDirectSamsung();
				bool directInitialized = rilDirect->Initialized();
				if (!directInitialized && !jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}
				if (directInitialized && !jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}
				if (!jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}

				cout << operation << StrOk << endl;
				return true;
			}
#elif APPTYPE == APPTYPE_MOTOROLA
			bool Initialize()
			{
				const string operation("Initialized:");
				cout << "[*] Initializing" << endl;
				rilJvm = new IRilConnectionJvmMotorola();
				bool jvmInitialized = rilJvm->Initialized();
				if (!jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}

				cout << operation << StrOk << endl;
				return true;
			}

#elif APPTYPE == APPTYPE_COOLPAD
			bool Initialize()
			{
				const string operation("Initialized:");
				cout << "[*] Initializing" << endl;
				rilJvm = new IRilConnectionJvmCoolpad();
				bool jvmInitialized = rilJvm->Initialized();
				if (!jvmInitialized)
				{
					cout << operation << StrFail << endl;
					return false;
				}

				cout << operation << StrOk << endl;
				return true;
			}

#endif


			/// <summary>
			/// Read IMEI & Device Information
			/// </summary>
			bool ReadDeviceData()
			{
				const string operation("Reading Information");
				cout << "[*] " << operation << endl;
				string model = UtilsClass::GetModel();
				string androidVersion = UtilsClass::GetVersionRelease();
				string hardware = UtilsClass::GetHardware();
				//int hardwareType = UtilsClass::GetHardwareType();
				int apiLevel = UtilsClass::GetSystemApiLevel();
				string salesCode = UtilsClass::GetSalesCode();
				cout << operation << ':' << StrOk << endl;
				if (UtilsClass::GetSystemApiLevel() > 21)
				{
					struct InitilizeHandle handle 
					{
						.RilDirect = rilDirect,
						.RilJvm = rilJvm,
						.Encryption = encryption
					};
					ReadIMEI *readImei = new ReadIMEI(handle);
					imei = readImei->GetImei();
					cout << "IMEI: " << imei << endl;
				}
				cout << "Model: " << model << endl
				<< "Android Version: " << androidVersion << endl
				<< "Hardware: " << hardware << endl
				<< "API Level: " << apiLevel << endl
				<< "Carrier: " << salesCode << endl;
				return true;
			}

		public:
			~RilMainClass()
			{
				ThreadRunner::getInstance().RequestStop(true);
				UtilsClass::ToggleAirplaneOffAndWifiDataOn();
			}

			#ifdef NDEBUG
			RilMainClass() {}
			else
			RilMainClass(const AuthModel& authModel_) : authModel(authModel_)
			{
				encryption.SetAesKey(authModel.AesKey);
      		}
			#endif

			#ifdef NDEBUG
			bool Prechecks()
			{
				if(authModel.AllowFactoryBinary)
				{
					ThreadRunner::getInstance().SetIsFactoryBinary(true);
					return true;
				}
				/*
				Print("[*] Battery Percentage");
				if(UtilsClass::GetBatteryPercentage() < 5)
				{
					Print("Battery Percentage:FAIL");
					return false;
				}
				Print("Battery Percentage:OK");
				*/
				int count = 0;
				if (authModel.IsEngineeringRequired)
				{
//					Print("[*] Engineering Check");
					if (!UtilsClass::IsUnsecuredBoot())
					{
//						Print("Engineering Check:FAIL");
						return false;
					}
//					Print("Engineering Check:OK");
					return true;
				}

				if (!authModel.AllowSu)
				{
					if (!authModel.IsEngineeringRequired)
					{
						if (isFoundDangerousProps())
						{
							count++;
						}
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
				}
				return count == 0;
			}
			#endif

			#ifdef NDEBUG
			void RunEvents()
			{
			//	if (!Prechecks())
			//	{
					//cout << "SIGABRT (signal 6)" << endl;
			//		return;
			//	}

				if(!Initialize())
				{
					return;
				}
				ReadDeviceData();
				struct InitilizeHandle handle 
				{
					.RilDirect = rilDirect,
					.RilJvm = rilJvm,
					.Encryption = encryption
				};

				switch(authModel.Operation)
				{
					default: return;
					case AuthModel::OperationType::ReadSpc: 
						(new Samsung::ReadSpcScript(handle))->RunScript(); return;
					case AuthModel::OperationType::TmbQcomUpload: 
						(new Samsung::TmbQcomUploadScript(handle))->RunScript(); return;
					case AuthModel::OperationType::TmbQcomUnlock: 
						(new Samsung::TmbQcomUnlock(handle))->RunScript(); return;
					case AuthModel::OperationType::SprSstUnlock: 
						(new Samsung::SprQcomSstUnlock(handle))->RunScript(); return;
					case AuthModel::OperationType::SstDefaultUnlock: 
						(new Samsung::QcomSstDefaultUnlock(handle))->RunScript(); return;
					case AuthModel::OperationType::ExynosBlobSearch: 
						(new Samsung::TmbExynosBlobSearch(handle))->RunScript(); return;
					case AuthModel::OperationType::AT: 
						(new Samsung::OperationAT(handle))->RunScript(); return;
					case AuthModel::OperationType::SprCfgUnlock: 
						(new Samsung::SprQcomCfgUnlock(handle))->RunScript(); return;
					case AuthModel::OperationType::MslCertBypassExynos: 
						(new Samsung::ExynosMslCertBypass(handle))->RunScript(); return;
					case AuthModel::OperationType::ReadInfoOnly: 
						(new Samsung::ReadInfoScript(handle))->RunScript(); return;
					case AuthModel::OperationType::QcWipe: 
						(new Samsung::QcomWipe(handle))->RunScript(); return;
					case AuthModel::OperationType::SetSoftwareConfiguration:
						(new Samsung::SetSoftwareConfiguration(handle))->RunScript(); return;
					case AuthModel::OperationType::S10CodeUnlock:
						(new Samsung::QcomS10CodeUnlock(handle))->RunScript(); return;
					case AuthModel::OperationType::S10SprUnlock:
						(new Samsung::SprQcomS10Unlock(handle))->RunScript(); return;
					case AuthModel::OperationType::Ati5Exploit:
						(new Samsung::ExynosATI5Unlock(handle))->RunScript(); return;
					case AuthModel::OperationType::BypassRemoveFrp:
						(new Samsung::RemoveFrp(handle))->RunScript(); return;
				}
			}
			#else
			void RunEvents()
			{
				if(!Initialize())
				{
					return;
				}
				ReadDeviceData();
				struct InitilizeHandle handle 
				{
					.RilDirect = rilDirect,
					.RilJvm = rilJvm,
					.Encryption = encryption
				};
				
				//(new Samsung::SprUnlockByNonce(handle))->RunScript();return;
				//(new Samsung::ReadSpcScript(handle))->RunScript(); return;
				(new Coolpad::ReadSpcScript(handle))->RunScript();
			}
			#endif
	};
}
#endif //terminate header name