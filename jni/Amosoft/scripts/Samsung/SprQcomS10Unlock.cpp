#ifndef SAMSUNGSPRQCOMS10UNLOCK_H
#define SAMSUNGSPRQCOMS10UNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class SprQcomS10Unlock : public ISamsungScript
    {
        private:
            inline static const string XaaCsc = "XAA";
        
			void OperationS10SprUnlock()
			{
				/// RTN and reset the device security right off the bat.
				if(!TryResetEfsAndClear(false))
				{
					return;
				}
				SetCsc(XaaCsc, XaaCsc);
				ResetModem(10);

				string originalRfCode;
				int retry = 6;
				do
				{
					if(GetRfByCode(originalRfCode))
					{
						break;
					}
					this_thread::sleep_for(chrono::seconds(5));
					retry--;
				} while(retry != 0);
				retry = 3;
				if (originalRfCode.empty())
				{
					return;
				}

				if (!BypassAndSetMsl())
				{
					// if we can not bypass and set, lets request actually request for it and have user input it
					//
					return;
				}

				if (!SetRfByCode(XaaCsc))
				{
					/// RTN and reset the device security right off the bat.
					TryResetEfsAndClear();
					return;
				}
				ResetModem(10);
				Print("[*] Unlocking");
				bool success;
				do
				{
				  success = SetSprintUnlockState(2);
					if(success)
					{
						break;
					}
					retry--;
				} while(retry != 0);
				retry = 3;
				if (success)
				{
					Print("Unlocking:OK");
				}
				else
				{
					Print("Unlocking:FAIL");
				}

				if (!BypassAndSetMsl())
				{
					TryResetEfsAndClear();
					return;
				}

				if (!SetRfByCode(originalRfCode))
				{
					TryResetEfsAndClear();
					return;
				}

				SetCsc(XaaCsc, XaaCsc);
				ResetModem(10);
				success = CheckSprUnlockState();
				if (success)
				{
                    PrintOperationStatusOkay();
				}
				else
				{
					SprRtn();
					this_thread::sleep_for(chrono::seconds(3));
					QcReset();
					this_thread::sleep_for(chrono::seconds(3));
                    PrintOperationStatusFail();
				}
			}

        public:
        
			SprQcomS10Unlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {
            }

            void RunScript()
            {
                OperationS10SprUnlock();
            }

        protected:

    };
}

#endif