#ifndef SAMSUNGSPRQCOMCFGUNLOCK_H
#define SAMSUNGSPRQCOMCFGUNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class SprQcomCfgUnlock : public ISamsungScript
    {
        private:
			void OperationSprCfgUnlock()
			{
				Print("[*] Unlocking");
				bool success = SetSprintUnlockState(2);
				if (success)
				{
					Print("Unlocking:OK");
				}
				else
				{
					Print("Unlocking:FAIL");
				}
				ResetModem(5);
				success = CheckSprUnlockState();
				if (success)
				{
                    PrintOperationStatusOkay();
				}
				else
				{
                    PrintOperationStatusFail();
				}
			}

        public:        
			SprQcomCfgUnlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationSprCfgUnlock();
            }

        protected:

    };
}

#endif