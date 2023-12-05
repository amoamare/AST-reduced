#ifndef IRILCONNECTIONSAMSUNG_H
#define IRILCONNECTIONSAMSUNG_H
#include "../../IRilConnection.cpp"

namespace Amosoft
{
	class IRilConnectionSamsung : public IRilConnection
	{
		private:
			#ifdef NDEBUG
			IRilConnection();
			#endif

		public:
			#ifdef NDEBUG
			IRilConnectionSamsung(const AuthModel& authModel_) : IRilConnection(authModel_) {}
			#else
			IRilConnectionSamsung(){}
			#endif
			
			virtual ~IRilConnectionSamsung() = default;
			
			//Methods
			
			virtual bool ReadSpc(std::string& spc, std::string& otksl) = 0;
			virtual bool SendAtComamnd(const std::string in) = 0;
			virtual bool RunAtCommand(const std::string command, std::string & message) = 0;
			virtual bool IsRadioOn(bool reconnect) = 0;
			virtual bool ResetModem() = 0;
			virtual bool InvokeOemRilRequestCommand(char* command, int size, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true) = 0;
			virtual bool InvokeCommand(std::string command, bool clearLogBefore = true, bool clearLogAfter = true, bool read = true) = 0;
			virtual bool CpCrash() = 0;
			virtual bool CpCrashByAt() = 0;
			virtual void CrashMe() = 0;
			virtual int GetSprintUnlockState() = 0;
			virtual bool SetSprintUnlockState(int state = 2) = 0;
			virtual bool QcWipe() = 0;
			virtual bool QcReset() = 0;
			virtual bool SprRtn() = 0;
			virtual bool ExynosDump(std::string blobData, std::string& response) = 0;
			virtual bool SetCsc(const std::string csc, const std::string salesCountry = std::string()) = 0;

			virtual bool GetSprintNonce(std::string& nonce) = 0;

		protected:
			inline static const std::string Filter = "logcat -b radio -d -v raw | grep -i -A 2 '^\\%ATCMD%';logcat -b radio -d -v raw | egrep -i '\\[RIL > ATD\\].*\\%ATCMD%|\\[RIL > ATD\\].*<\\('";

			std::string GetFilter(const std::string& command)
			{
				std::string filter = UtilsClass::ReplaceAll(Filter, "%ATCMD%", command);
				return filter;
			}

			std::string CreateFilter(const std::string command)
			{
				std::string filter;
				filter.assign(command);
				filter.erase(0,2); //erase AT keep +COMMAND=data
				size_t index = filter.rfind("="); //get index of =
				filter.erase(filter.begin()+index, filter.end()); //erase =and everything after
				filter.append(":");
				return filter;
			}

			std::string CreateLogCatFilter(const std::string command)
			{
				std::string filter = CreateFilter(command);
				return GetFilter(filter);
			}

			std::string MatchAtCommand(const std::string& command)
			{
				std::string outputStr;
				std::regex regex("<(.*?)>.*");
				std::sregex_iterator next(command.begin(), command.end(), regex);
				std::sregex_iterator end;
				const std::string format="";
				while (next != end)
				{
					std::smatch match = *next;
					if (match.size() > 0)
					{
						std::string value = match.str(1);// regex_replace(match.str(1), replaceEx, format);
						std::string replace = UtilsClass::ReplaceAll(value, "(\\r)", format);
						std::string replaceTwo = UtilsClass::ReplaceAll(replace, "(\\n)", "\r\n");

						if(!replaceTwo.empty() && replaceTwo[replaceTwo.size() -1] == '\r')
							replaceTwo.erase(replaceTwo.size() -1);

	#ifndef NDEBUG
	std::cout << "[D] - Recived Command: " << replaceTwo.length() << std::endl;
	#endif

						if (outputStr.empty() && replaceTwo.length() == 2)
						{
							next++;
							continue;
						}
						if (!replaceTwo.empty()) outputStr.append(replaceTwo);
					}
					next++;
				}
				if (outputStr.empty()) outputStr.assign(command);
				return outputStr;
			}

			std::string CreateAtRequest(const std::string command)
			{
				int totalSize = sizeof(struct OEMRequestRawHeader) + command.length();
				struct OEMRequestRawHeader header
				{
				.main_cmd = 0x12,
				.sub_cmd = 0x0D,
				.length = static_cast<unsigned short>(htons(totalSize)),
				.cmdLength = static_cast<unsigned short>(htons(command.length()))
				};
				std::string request;
				request.assign(reinterpret_cast<char*>(&header), sizeof(struct OEMRequestRawHeader));
				request.append(command);
				return request;
			}

			std::string CreateCscRequest(const std::string salesCode, const std::string salesCountry = std::string())
			{
				int structSize = sizeof(struct OEMRequestRawHeader);
				int totalSize = structSize;
				totalSize += salesCode.length();
				if(!salesCountry.empty()) totalSize += salesCountry.length() + 1;//we add an additional 1 for null termination

				struct OEMRequestRawHeader header
				{
				.main_cmd = 0x06,
				.sub_cmd = 0x01,
				.length = static_cast<unsigned short>(htons(totalSize)),
				.cmdLength = static_cast<unsigned short>(htons(0))
				};

				std::string request;
				request.assign(reinterpret_cast<char*>(&header), structSize);
				request.append(salesCode);
				if (!salesCountry.empty())
				{
					const char raw[1] = {0x00};
					request.append(raw,1);
					request.append(salesCountry);
				}
				return request;
			}
  };
}

#endif