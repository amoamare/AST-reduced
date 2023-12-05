#ifndef SOFTWARECONFIGMODEL_H
#define SOFTWARECONFIGMODEL_H
#include "rapidjson/document.h"
#include <string>
#include <utils.cpp>

using namespace rapidjson;
namespace Amosoft
{
  class SoftwareConfigModel
  {
    private:
      Document json;
    public:
      bool Success = false;
      std::string Command;
      SoftwareConfigModel()
      {

      }
      SoftwareConfigModel(const std::string jsonData)
      {
        if(!json.Parse(jsonData.c_str()).HasParseError())
        {
          Success = true;
        }
        if (!Success)
        {
          return;
        }
        if (!json.HasMember("Command"))
        {
          Success = false;
        }
        if (!Success)
        {
          return;
        }
        std::string temp = json["Command"].GetString();
        Success = UtilsClass::IsValidAsciiHexData(temp);
        if (Success)
        {
          Command = UtilsClass::HexToString(temp);
        }
        temp.clear();
      }
  };
}
#endif
