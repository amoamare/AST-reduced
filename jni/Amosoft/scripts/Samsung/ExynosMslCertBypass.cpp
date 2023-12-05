#ifndef SAMSUNGEXYNOSMSLCERTBYPASS_H
#define SAMSUNGEXYNOSMSLCERTBYPASS_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class ExynosMslCertBypass : public ISamsungScript
    {
        private:
			void OperationMslCertBypassExynos()
			{
				Print("[*] Executing");
				bool success = false;
				do
				{
					PrintInput();
					string str;
					getline (cin, str);
					if(!ValidateInputData(str))
					{
						return;
					}

					const string command = DecryptInput(str);
					const string StrSet("set");
					if (UtilsClass::CaseInSensStringCompare(command, StrSet))
					{
						if (!success)
						{
								Print("Executed:FAIL");
								return;
						}

						if (WriteCalculatedDataLock())
						{
							Print("Executed:OK");
							PrintInput();
							getline (cin, str);
							if(!ValidateInputData(str))
							{
								return;
							}
							const UnlockModel unlockModel(DecryptInput(str));
							if (!unlockModel.Success)
							{
								return;
							}

							this_thread::sleep_for(chrono::seconds(2));
							const string freezeCode(UtilsClass::HexToString("0413001407003030303030303030000000000000"));
							InvokeCommand(freezeCode);
							this_thread::sleep_for(chrono::seconds(2));
							bool flag = UnlockCommand(unlockModel.BlobData);
							ResetModem(5);
							if (flag)
							{
                                PrintOperationStatusOkay();
							}
							else
							{
                                PrintOperationStatusFail();
							}
						}
						else
						{
							Print("Executed:FAIL");
						}

						return;
					}
					else
					{
						const string rawCommand = UtilsClass::HexToString(command);
						for(int i = 0; i < 3; i++)
						{
								success = InvokeCommand(rawCommand);
								if (success) break;
								this_thread::sleep_for(chrono::seconds(3)); // we will try for 3 attempts sleeping 3 sec between each one.
						}
						if (success)
						{
							Print("SENT:OK");
						}
						else
						{
							Print("SENT:FAIL");
						}
					}
				}while(success);
			}


        public:        
			ExynosMslCertBypass(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationMslCertBypassExynos();
            }

        protected:

    };
}

#endif