#ifndef REMOVEFRP_H
#define REMOVEFRP_H

#include <iostream>

#include "IScript.cpp"

using namespace std;

namespace Amosoft::Scripts::Samsung
{
    class RemoveFrp : public IScript
    {
        private:

			void BypassFrp()
			{
				Print("[*] Bypassing FRP");
				// There is a problem when system isn't fully booted, we will get a error 'Error while accessing settings provider', we should probably check for this error a few times and fail if it doesn't work.
				//This should basically suppport any and all android's so long as we have adb. Of course we wouldn't even be able to run this if we didn't have adb lolz.
				int retry = 3;
				do
				{
					system("input keyevent KEYCODE_WAKEUP;input keyevent KEYCODE_MENU");
					const string command("settings put global setup_wizard_has_run 1;"
					"settings put secure user_setup_complete 1;"
					"content insert --uri content://settings/secure --bind name:s:DEVICE_PROVISIONED --bind value:i:1;"
					"content insert --uri content://settings/secure --bind name:s:user_setup_complete --bind value:i:1;"
					"content insert --uri content://settings/secure --bind name:s:INSTALL_NON_MARKET_APPS --bind value:i:1;"
					"am start -c android.intent.category.HOME -a android.intent.action.MAIN;"
					"am start -n com.android.settings/com.android.settings.Settings");
					string response = UtilsClass::exec_get_out(command);

					if (UtilsClass::CaseInSensStringCompare(response, "error"))
					{
						this_thread::sleep_for(chrono::seconds(1));
						retry--;
						continue;
					}
					break;
				} while(retry != 0);
				if (retry == 0)
				{
					Print("Bypassing FRP:FAIL");
					Print("Operation Status:FAIL");
					return;
				}
				Print("Bypassing FRP:OK");
				Print("Operation Status:OK");
			}

			bool ErasePartition(string name, string partition)
			{
				string printValue("[*] Erasing ");
				printValue.append(partition);
				Print(printValue);

				if (!partition.empty())
				{
					string eraseCommand;
					eraseCommand.assign("dd if=/dev/zero of=");
					eraseCommand.append(partition);
					eraseCommand.append(" bs=512 2>&1");

					//string patternCommand;
					//patternCommand.assign("dd: ");
					//patternCommand.append(partition);
					//patternCommand.append(": No space left on device");

					string response = UtilsClass::exec_get_out(eraseCommand);
					if(UtilsClass::CaseInSensStringCompare(response, "no space left on device"))
					{
						Print("Erasing:OK");
						return true;
					}
					else
					{
						Print("Erasing:FAIL");
						return false;
					}
				}
				else
				{
					Print("Erasing:FAIL");
					return false;
				}
			}

			bool GetSteady(string& path)
			{
				path = string();
				Print("[*] Searching Reactivation Lock");
				const char* paths[4] =
				{
					"ls /dev/block/platform/*/by-name/STEADY",
					"ls /dev/block/platform/*/*/by-name/STEADY",
					"ls /dev/block/platform/*/by-name/steady",
					"ls /dev/block/platform/*/*/by-name/steady"
				};
				for(int i = 0; i < 4; i++)
				{
					string value = UtilsClass::exec_get_out(reinterpret_cast<char*>(path[i]));
					if (!value.empty())
					{
						Print("Searching Reactivation Lock:OK");
						path = value;
						return true;
					}
				}
				path = string();
				Print("Searching Reactivation Lock:FAIL");
				return false;
			}

			bool GetPersData(string& path)
			{
				path = string();
				Print("[*] Searching EE Lock");
				const char* paths[4] =
				{
					"ls /dev/block/platform/*/by-name/PERSDATA",
					"ls /dev/block/platform/*/*/by-name/PERSDATA",
					"ls /dev/block/platform/*/by-name/persdata",
					"ls /dev/block/platform/*/*/by-name/persdata"
				};
				for(int i = 0; i < 4; i++)
				{
					string value = UtilsClass::exec_get_out(reinterpret_cast<char*>(path[i]));
					if (!value.empty())
					{
						Print("Searching EE Lock:OK");
						path = value;
						return true;
					}
				}
				path = string();
				Print("Searching EE Lock:FAIL");
				return false;
			}

			bool GetPersistent(string& path)
			{
				path = string();
				Print("[*] Searching FRP Lock");
				string frpLocation;
				if (!UtilsClass::GetFrpLocation(frpLocation))
				{
					const char* paths[4] =
					{
						"ls /dev/block/platform/*/by-name/PERSISTENT",
						"ls /dev/block/platform/*/*/by-name/PERSISTENT",
						"ls /dev/block/platform/*/by-name/persistent",
						"ls /dev/block/platform/*/*/by-name/persistent"
					};
					for(int i = 0; i < 4; i++)
					{
						string value = UtilsClass::exec_get_out(reinterpret_cast<char*>(path[i]));
						if (!value.empty())
						{
							Print("Searching FRP Lock:OK");
							path = value;
							return true;
						}
					}
					Print("Searching FRP Lock:FAIL");
					return false;
				}
				Print("Searching FRP Lock:OK");
				path = frpLocation;
				return true;
			}

			bool EraseSamsungLocks()
			{
				Print("[*] Removing Locks");
                system("input keyevent KEYCODE_WAKEUP;input keyevent KEYCODE_MENU");
				string path;
				bool flag = false;
				if (GetPersistent(path))
				{
					if(ErasePartition("FRP", path))
					{
						flag = true;
					}
				}
				if (GetPersData(path))
				{
					if(ErasePartition("EE Lock", path))
					{
						flag = true;
					}
				}
				if (GetSteady(path))
				{
					if(ErasePartition("Samsung Reactivation Lock", path))
					{
						flag = true;
					}
				}
				return flag;
			}

			bool EraseLocks(const string manufacturer)
			{
				if (UtilsClass::CaseInSensStringCompare(manufacturer, "Samsung"))
				{
					return EraseSamsungLocks();
				}
				return false;
			}

			void OperationRemoveFrp()
			{
				Print("[*] Read Manufacturer");
				string manufacturer;
				if(!UtilsClass::GetDeviceManufacturer(manufacturer))
				{
					Print("Read Manufacturer:FAIL");
					return;
				}
				Print("Read Manufacturer:OK");
				if (UtilsClass::IsUnsecuredBoot())
				{
					if (setreuid(0, 1001u) == 0)
					{
						if(EraseLocks(manufacturer))
						{
                            PrintOperationStatusOkay();
							return;
						}
					}
				}
				BypassFrp();
			}

        public:
        
			RemoveFrp(InitilizeHandle& handle) : IScript(handle) 
            {
            }

            void RunScript()
            {
                OperationRemoveFrp();
            }

        protected:

    };
}

#endif