#ifndef SAMSUNGREADSPCSCRIPT_H
#define SAMSUNGREADSPCSCRIPT_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class ReadSpcScript : public ISamsungScript
    {
        private:

            void ReadSpc()
			{
				Print("[*] Reading SPC Information");
				string spc;
				string otksl;
				bool flag = GetRilConnection()->ReadSpc(spc, otksl);
				if (flag)
				{
					Print("Reading SPC Information:OK");
					string strSpc("SPC:[");
					strSpc.append(spc);
					strSpc.append("]");
					string strOtksl("OTKSL:[");
					strOtksl.append(otksl);
					strOtksl.append("]");
					Print(strSpc);
					Print(strOtksl);
					PrintOperationStatusOkay();
				}
				else
				{
					Print("Reading SPC Information:FAIL");
					PrintOperationStatusFail();
				}
			}


        public:
        
			ReadSpcScript(InitilizeHandle& handle) : ISamsungScript(handle) {}

            void RunScript()
            {
                ReadSpc();
            }

        protected:

    };
}

#endif