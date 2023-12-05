#ifndef SAMSUNGQCOMS10CODEUNLOCK_H
#define SAMSUNGQCOMS10CODEUNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class QcomS10CodeUnlock : public ISamsungScript
    {
        private:

			void OperationS10CodeUnlockBypass()
			{
				/// RTN and reset the device security right off the bat.
				if(!TryResetEfsAndClear(true, 3, 15)) //AT&T doesn't reset fast enough for some reason.
				{
					return;
				}
				string command;
				string message;
				if (!BypassAndSetMsl())
				{
					return;
				}

				if(!TryWriteCalculatedDataLock())
				{
					return;
				}
				PrintInput();

                string str;
                if(!GetSanitizedHexInput(str)) return;
                const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				const string freezeCode(UtilsClass::HexToString("0413001407003030303030303030000000000000"));
				//const char freezeCode[20] = {	0x04, 0x13, 0x00, 0x14, 0x07, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
				InvokeCommand(freezeCode);
				bool flag = UnlockCommand(unlockModel.BlobData);
				ResetModem(5);
                if(flag)
                {
                    PrintOperationStatusOkay(); 
                } 
                else 
                {
                    PrintOperationStatusFail();
                }
			}

        public:
        
			QcomS10CodeUnlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {
            }

            void RunScript()
            {
            }

        protected:

    };
}

#endif