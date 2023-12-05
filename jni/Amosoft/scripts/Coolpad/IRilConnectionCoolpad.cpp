#ifndef IRILCONNECTIONCOOLPAD_H
#define IRILCONNECTIONCOOLPAD_H
#include "../../IRilConnection.cpp"

namespace Amosoft
{
	class IRilConnectionCoolpad : public IRilConnection
	{
		private:
			#ifdef NDEBUG
			IRilConnectionCoolpad();
			#endif

		public:
			#ifdef NDEBUG
			IRilConnectionCoolpad(const AuthModel& authModel_) : IRilConnection(authModel_) {}
			#else
			IRilConnectionCoolpad(){}
			#endif
			
			virtual ~IRilConnectionCoolpad() = default;
			
			//Methods

			virtual bool ReadImeiData() = 0;
		
			virtual bool GetSprintNonce(std::string& nonce) = 0;
			virtual bool ReadSpc(std::string& spc, std::string& otksl) = 0;

		protected:
  };
}

#endif