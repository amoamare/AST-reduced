#ifndef SAMSUNGEXYNOSATI5UNLOCK_H
#define SAMSUNGEXYNOSATI5UNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class ExynosATI5Unlock : public ISamsungScript
    {
        private:

			void OperationAti5Unlock()
			{
				Print("[*] Initializing Data");
				if (!SendAtComamnd("ATI5"))
				{
					Print("Initializing Data:FAIL");
					return;
				}
				this_thread::sleep_for(chrono::seconds(1));
				Print("Initializing Data:OK");
				if(!WriteCalculatedDataLock())
				{
					return;
				}
                string str;
                if(!GetSanitizedHexInput(str)) return;
                const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				//const char freezeCode[20] = {	0x04, 0x13, 0x00, 0x14, 0x07, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
				const string freezeCode(UtilsClass::HexToString("0413001407003030303030303030000000000000"));
				InvokeCommand(freezeCode);
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


        public:        
			ExynosATI5Unlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationAti5Unlock();
            }

        protected:

    };
}

#endif