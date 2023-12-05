#ifndef SAMSUNGSPRUNLOCKBYNONCE_H
#define SAMSUNGSPRUNLOCKBYNONCE_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class SprUnlockByNonce : public ISamsungScript
    {
        private:

			void UnlockByNonce()
			{
				Print("[*] Input Sprint Nonce Reponse");
				string str;
                if(!GetSanitizedHexInput(str)) return;
				const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
			}

            void GetNonce()
			{
				Print("[*] Reading Sprint Nonce");
				std::string nonce;
				bool flag = GetRilConnection(true)->GetSprintNonce(nonce);
				if (flag)
				{
					Print("Reading Nonce Information:OK");
					string strSpc("Nonce:[");
					strSpc.append(nonce);
					strSpc.append("]");
					Print(strSpc);
					PrintOperationStatusOkay();
					UnlockByNonce();
				}
				else
				{
					Print("Reading Nonce Information:FAIL");
					PrintOperationStatusFail();
					
					UnlockByNonce();
				}
			}


        public:
        
			SprUnlockByNonce(InitilizeHandle& handle) : ISamsungScript(handle) {}

            void RunScript()
            {
                GetNonce();
            }

        protected:

    };
}

#endif