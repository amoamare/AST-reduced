#ifndef SAMSUNGOPERATIONAT_H
#define SAMSUNGOPERATIONAT_H

#include <iostream>

#include "ISamsungScript.cpp"

namespace Amosoft::Scripts::Samsung
{
    class OperationAT : public ISamsungScript
    {
        private:
			void OperationAt()
			{
				bool flag = true;
				do
				{
					PrintInput();
					string str;
					getline (cin, str);
					if(!ValidateInputData(str))
					{
						continue;
					}
					const string command = DecryptInput(str);
					if (command.compare("exit") == 0)
					{
						return; // each input needs to be an encrypted json of CommandModel, if its not discard;
					}
					int retry = 5;
					string message;
					do
					{
						message.clear();
						flag = CheckAndExecuteCommand(command, message);
						if (flag) break;
						retry--;
						message.insert(0, "FAIL");
						this_thread::sleep_for(chrono::seconds(2));
					}while(retry != 0);
					Print(message);
				}while(true);
			}

        public:        
			OperationAT(InitilizeHandle& handle) : ISamsungScript(handle) 
            {

            }

            void RunScript()
            {
                OperationAt();
            }

        protected:

    };
}

#endif