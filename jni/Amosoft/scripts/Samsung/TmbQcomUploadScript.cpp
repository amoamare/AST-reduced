#ifndef SAMSUNGTMBQCOMUPLOADSCRIPT_H
#define SAMSUNGTMBQCOMUPLOADSCRIPT_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class TmbQcomUploadScript : public ISamsungScript
    {
        private:
            void OperationTmbUpload()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
				const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				InvokeCommand(unlockModel.BlobData, false, false, false);
				RebootUpload();
			}

      

        public:
        
			TmbQcomUploadScript(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationTmbUpload();
            }

        protected:

    };
}

#endif