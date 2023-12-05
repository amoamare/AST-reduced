#ifndef READIMEI_H
#define READIMEI_H

#include <iostream>

#include "IScript.cpp"

namespace Amosoft::Scripts
{
    class ReadIMEI : public IScript
    {
        private:

			void ReadImei()
			{
			}


        public:        
			ReadIMEI(InitilizeHandle& handle) : IScript(handle) 
            {

            }

            void RunScript()
            {
                ReadImei();
            }

            string GetImei()
            {
                string imei = GetRilConnection()->Imei;
				if (imei.empty()) imei.append("00000000000000");
                return imei;
            }

            
			IRilConnection *GetRilConnection(bool forceJvm = true)
			{
                return _rilJvm;
			}

        protected:

    };
}

#endif