#ifndef SAMSUNGCHANGECSCSCRIPT_H
#define SAMSUNGCHANGECSCSCRIPT_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class ChangeCscScript : public ISamsungScript
    {
        private:

            void ChangeCsc()
			{
				Print("[*] Changing CSC Information");
				bool flag = GetRilConnection(true)->SetCsc("XAA", "XAA");
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
        
			ChangeCscScript(InitilizeHandle& handle) : ISamsungScript(handle) {}

            void RunScript()
            {
                ChangeCsc();
            }

        protected:

    };
}

#endif