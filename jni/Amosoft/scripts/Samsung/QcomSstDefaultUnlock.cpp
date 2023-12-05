#ifndef SAMSUNGQCOMSSTDEFAULTUNLOCK_H
#define SAMSUNGQCOMSSTDEFAULTUNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class QcomSstDefaultUnlock : public ISamsungScript
    {
        private:        

			void OperationSstDefaultUnlock()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
				const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}

				//const char freezeCode[20] = {	0x04, 0x13, 0x00, 0x14, 0x07, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,	0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
				const string freezeCode(UtilsClass::HexToString("0413001407003030303030303030000000000000"));
				InvokeCommand(freezeCode);

				ResetModem(0);
				int count = 20;
				bool flag = false;
				Print("[*] Unlocking");
				do
				{
					flag = InvokeCommand(unlockModel.BlobData);
					if (flag)
					{
						Print("Unlocking:OK");
						break;
					}
					count--;
				}while(count != 0);
				if (!flag)
				{
					Print("Unlocking:FAIL");
				}
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
        
			QcomSstDefaultUnlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationSstDefaultUnlock();
            }

        protected:

    };
}

#endif