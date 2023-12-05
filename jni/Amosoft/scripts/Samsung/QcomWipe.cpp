#ifndef SAMSUNGQCOMWIPE_H
#define SAMSUNGQCOMWIPE_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class QcomWipe : public ISamsungScript
    {
        private:

			void OperationQcWipe()
			{
				Print("[*] QC Wipe");
				if (UtilsClass::GetHardwareType() != 1)
				{
				 	Print("[!] Device is not Qualcomm");
					return;
				}
				QcWipe();
				Print("QC Wipe:OK");
				ResetModem(5);
			}
            

        public:        
			QcomWipe(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationQcWipe();
            }

        protected:

    };
}

#endif