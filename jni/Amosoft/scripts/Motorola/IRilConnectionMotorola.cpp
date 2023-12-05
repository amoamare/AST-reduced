#ifndef IRILCONNECTIONMOTOROLA_H
#define IRILCONNECTIONMOTOROLA_H
#include "../../IRilConnection.cpp"

namespace Amosoft
{
	class IRilConnectionMotorola : public IRilConnection
	{
		private:
			#ifdef NDEBUG
			IRilConnection();
			#endif

		public:
			#ifdef NDEBUG
			IRilConnectionMotorola(const AuthModel& authModel_) : IRilConnection(authModel_) {}
			#else
			IRilConnectionMotorola(){}
			#endif
			
			virtual ~IRilConnectionMotorola() = default;
			
			//Methods

			virtual bool ReadImeiData() = 0;
		
			virtual bool GetSprintNonce(std::string& nonce) = 0;
			virtual bool ReadSpc(std::string& spc, std::string& otksl) = 0;

		protected:
  };
}

#endif