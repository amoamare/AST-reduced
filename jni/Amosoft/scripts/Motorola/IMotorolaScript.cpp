#ifndef IMOTOROLASCRIPT_H
#define IMOTOROLASCRIPT_H

#include <utils.cpp>
#include "../IScript.cpp"


using std::cout;
using std::cin;
using std::string;

namespace Amosoft
{
    class IMotorolaScript : public IScript
    {
        private:

        protected:
			IMotorolaScript(InitilizeHandle& handle) : IScript(handle) 
            {

            }
            
			IRilConnectionMotorola *GetRilConnection(bool forceJvm = false)
			{
				return reinterpret_cast<IRilConnectionJvmMotorola *>(_rilJvm);
			}

        public:            
            virtual ~IMotorolaScript() = default;

    };
}

#endif