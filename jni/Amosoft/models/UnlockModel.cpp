#ifndef UNLOCKMODEL_H
#define UNLOCKMODEL_H
#include "rapidjson/document.h"
#include <string>
#include <utils.cpp>

using namespace rapidjson;
namespace Amosoft
{
  class UnlockModel
  {
    private:
      Document json;
    public:
      bool Success = false;
      std::string BlobData;
      UnlockModel()
      {

      }
      UnlockModel(const std::string & jsonData)
      {
        if(!json.Parse(jsonData.c_str()).HasParseError())
        {
          Success = true;
        }
        if (!Success)
        {
          return;
        }
        if (!json.HasMember("BlobData"))
        {
          Success = false;
        }
        if (!Success)
        {
          return;
        }
        std::string temp = json["BlobData"].GetString();
        Success = UtilsClass::IsValidAsciiHexData(temp);
        if (Success)
        {
          BlobData = UtilsClass::HexToString(temp);
        }
        temp.clear();
      }
  };
}
#endif
