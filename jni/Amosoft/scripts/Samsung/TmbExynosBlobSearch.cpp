#ifndef SAMSUNGEXYNOSBLOBSEARCH_H
#define SAMSUNGEXYNOSBLOBSEARCH_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class TmbExynosBlobSearch : public ISamsungScript
    {
        private: 
			void OperationExynosBlobSearch()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
				const UnlockModel unlockModel(DecryptInput(str));
				if (!unlockModel.Success)
				{
					return;
				}
				Print("[*] Reading Security Data");
                UtilsClass::DisablePackage("com.sec.app.RilErrorNotifier");
				UtilsClass::ClearDataLogs();
				ThreadRunner::getInstance().RequestStop(true);
				string response;
				bool flag = ExynosDump(unlockModel.BlobData, response);
				UtilsClass::ClearDataLogs();
				if (!flag)
				{
					Print("Reading Security Data:FAIL");
					return;
				}
				Print("Reading Security Data:OK");
				Print(response);
			}

        public:
        
			TmbExynosBlobSearch(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationExynosBlobSearch();
            }

        protected:

    };
}

#endif