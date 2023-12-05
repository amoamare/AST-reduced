#ifndef SAMSUNGSETSOFTWARECONFIGURATION_H
#define SAMSUNGSETSOFTWARECONFIGURATION_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class SetSoftwareConfiguration : public ISamsungScript
    {
        private:

			void OperationSetSoftwareConfiguration()
			{
                string str;
                if(!GetSanitizedHexInput(str)) return;
				const SoftwareConfigModel softwareConfigModel(DecryptInput(str));
				if (!softwareConfigModel.Success)
				{
					return;
				}
				Print("[*] Setting Software Config");
				bool flag = InvokeCommand(softwareConfigModel.Command);
				if (flag)
				{
					Print("Setting Software Config:OK");
                    PrintOperationStatusOkay();
				}
				else
				{
					Print("Setting Software Config:FAIL");
                    PrintOperationStatusFail();
				}

			}

        public:        
			SetSoftwareConfiguration(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationSetSoftwareConfiguration();
            }

        protected:

    };
}

#endif