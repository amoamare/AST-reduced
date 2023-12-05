#ifndef ICOOLPADSCRIPT_H
#define ICOOLPADSCRIPT_H

#include <utils.cpp>
#include "../IScript.cpp"


using std::cout;
using std::cin;
using std::string;

namespace Amosoft
{
    class ICoolpadScript : public IScript
    {
        private:

        protected:
			ICoolpadScript(InitilizeHandle& handle) : IScript(handle) 
            {

            }
            
			IRilConnectionCoolpad *GetRilConnection(bool forceJvm = false)
			{
				return reinterpret_cast<IRilConnectionJvmCoolpad *>(_rilJvm);
			}

        public:            
            virtual ~ICoolpadScript() = default;

    };
}

#endif