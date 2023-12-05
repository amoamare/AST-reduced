#ifndef SAMSUNGTMBQCOMUNLOCK_H
#define SAMSUNGTMBQCOMUNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class TmbQcomUnlock : public ISamsungScript
    {
        private:
        
            void OperationTmbUnlock()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
                const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				Print("[*] Unlocking");
				bool flag = false;
				for(int i = 0; i < 3; i++)
				{
					bool succes = InvokeCommand(unlockModel.BlobData);
					if (succes)
					{
						flag = true;
					}
					this_thread::sleep_for(chrono::milliseconds(2500));;
				}
				if(flag)
				{
					Print("Unlocking:OK");
				}
				else
				{
					Print("Unlocking:FAIL");
				}
				Print("[*] Finalizing");
				this_thread::sleep_for(chrono::seconds(15));;
				Print("Finalizing:OK");
				ResetModem(15);
				if (flag)
				{
					Print("Operation Status:OK");
				}
				else
				{
					Print("Operation Status:FAIL");
				}
			}        

        public:
        
			TmbQcomUnlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {
            }

            void RunScript()
            {
				OperationTmbUnlock();
            }

        protected:

    };
}

#endif