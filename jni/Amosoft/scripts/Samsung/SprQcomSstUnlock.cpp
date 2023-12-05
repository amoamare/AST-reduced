#ifndef SAMSUNGSPRQCOMSSTUNLOCK_H
#define SAMSUNGSPRQCOMSSTUNLOCK_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class SprQcomSstUnlock : public ISamsungScript
    {
        private:
			void OperationSprSstUnlock()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
				const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				bool flag = UnlockCommand(unlockModel.BlobData);
				ResetModem(5);
				CheckSprUnlockState();
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
			SprQcomSstUnlock(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationSprSstUnlock();
            }

        protected:

    };
}

#endif