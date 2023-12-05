#ifndef AUTH_H
#define AUTH_H
#include <string>
#include <utils.cpp>
#include "rapidjson/document.h"

using namespace rapidjson;

namespace Amosoft
{
  class AuthModel
  {
    private:
      Document json;

    public:
      typedef enum
      {
        None = -1,
        AT = 0,
        ReadInfoOnly = 1,
        QcWipe = 2,
        ReadSpc = 3,
        TmbQcomUpload = 4,
        TmbQcomUnlock = 5,
        SprSstUnlock = 6,
        SstDefaultUnlock = 7,
        ExynosBlobSearch = 8,
        SprCfgUnlock = 9,
        MslCertBypassExynos = 10,
        SetSoftwareConfiguration = 11,
        S10CodeUnlock = 12,
        S10SprUnlock = 13,
        ExynosAtUnlock = 14,
        S10VzwUnlock = 15,
        Ati5Exploit = 16,
        BypassRemoveFrp = 17,
        SprByMslCert = 18
      } OperationType;

      typedef enum
      {
         Direct,
         Jvm
      } RilConnectionType;

      bool Success = false;
      std::string Random;
      std::string Descriptor;
      std::string Phone;
      std::string AndroidPhone;


      std::string Handle;
      std::string CloseClientRild;
      std::string ConnectRild;
      std::string IsConnectedRild;
      std::string InvokeOemRequestHookRaw;
      std::string OpenClientRild;
      std::string DisconnectRild;
      std::string RegisterErrorCallback;
      std::string RegisterRequestCompleteHandler;
      std::string RegisterUnsolicitedHandler;
      std::string ModemApiSendRequest;

      std::string AesKey;
      std::string Imei;
      bool CheckDebugLvlHigh = true;
      bool CheckCpDebugLvlHigh = true;
      bool CheckHardware = true;
      bool CheckCarrier = true;
      bool RebootToUpload = false;
      bool ForceJvm = false;
      bool AllowFactoryBinary = false;
      bool IsEngineeringRequired = false;
      bool AllowSu = false;
      OperationType Operation = OperationType::None;
      int RestrictedToHardwareType = 0; // Any
    	//inline static CkJsonArray *RestrictedToCarriers;

      AuthModel()
      {

      }
      ~AuthModel()
      {
        //delete RestrictedToCarriers;
      }
      AuthModel(const std::string & jsonData)
      {
        if(!json.Parse(jsonData.c_str()).HasParseError())
        {
          Success = true;
        }
        if (!Success)
        {
          exit(313);
        }
        if (!json.HasMember("Random"))
        {
          Success = false;
        }
        if (!json.HasMember("Descriptor"))
        {
          Success = false;
        }
        if (!json.HasMember("Phone"))
        {
          Success = false;
        }
        if (!json.HasMember("AndroidPhone"))
        {
          Success = false;
        }
        if (!json.HasMember("Imei"))
        {
          //Success = false;
        }
        if (!Success)
        {
          return;
        }

        Random = json["Random"].GetString();
        Descriptor = json["Descriptor"].GetString();
        Phone = json["Phone"].GetString();
        AndroidPhone = json["AndroidPhone"].GetString();

        Handle = json["Handle"].GetString();
        CloseClientRild = json["CloseClientRild"].GetString();
        ConnectRild = json["ConnectRild"].GetString();
        IsConnectedRild = json["IsConnectedRild"].GetString();
        InvokeOemRequestHookRaw = json["InvokeOemRequestHookRaw"].GetString();
        OpenClientRild = json["OpenClientRild"].GetString();
        DisconnectRild = json["DisconnectRild"].GetString();
        RegisterErrorCallback = json["RegisterErrorCallback"].GetString();
        RegisterRequestCompleteHandler = json["RegisterRequestCompleteHandler"].GetString();
        RegisterUnsolicitedHandler = json["RegisterUnsolicitedHandler"].GetString();
        ModemApiSendRequest = json["ModemApiSendRequest"].GetString();
        if (json.HasMember("Imei"))
        {
          Imei = json["Imei"].GetString();
          if (Imei.length() > 14) Imei.substr(0, 14);
        }

        if (json.HasMember("CheckDebugLvlHigh"))
        {
          CheckDebugLvlHigh = json["CheckDebugLvlHigh"].GetBool();
          UtilsClass::CheckDebugLvlHigh = CheckDebugLvlHigh;
        }
        if (json.HasMember("CheckCpDebugLvlHigh"))
        {
          CheckCpDebugLvlHigh = json["CheckCpDebugLvlHigh"].GetBool();
          UtilsClass::CheckCpDebugLvlHigh = CheckCpDebugLvlHigh;
        }
        if (json.HasMember("CheckHardware"))
        {
          CheckHardware = json["CheckHardware"].GetBool();
          UtilsClass::CheckHardware = CheckHardware;
        }
        if (json.HasMember("CheckCarrier"))
        {
          CheckCarrier = json["CheckCarrier"].GetBool();
          UtilsClass::CheckCarrier = CheckCarrier;
        }
        if (json.HasMember("RestrictedToHardwareType"))
        {
          RestrictedToHardwareType = json["RestrictedToHardwareType"].GetInt();
          UtilsClass::RestrictedToHardwareType = RestrictedToHardwareType;
        }
        if (json.HasMember("RestrictedToCarriers"))
        {
      //    RestrictedToCarriers = json.ArrayOf("RestrictedToCarriers");
      //    UtilsClass::RestrictedToCarriers = RestrictedToCarriers;
        }

        if (json.HasMember("AesKey"))
        {
          AesKey = json["AesKey"].GetString();
        }
        if (json.HasMember("RebootToUpload"))
        {
          RebootToUpload = json["RebootToUpload"].GetBool();
        }
        if (json.HasMember("Operation"))
        {
          Operation = static_cast<OperationType>(json["Operation"].GetInt());
        }


        if (json.HasMember("ForceJvm"))
        {
          ForceJvm = json["ForceJvm"].GetBool();
        }
        if (json.HasMember("AllowFactoryBinary"))
        {
          AllowFactoryBinary = json["AllowFactoryBinary"].GetBool();
        }
        if (json.HasMember("IsEngineeringRequired"))
        {
          IsEngineeringRequired = json["IsEngineeringRequired"].GetBool();
        }
        if (json.HasMember("AllowSu"))
        {
          AllowSu = json["AllowSu"].GetBool();
        }

        Success = true;
      }
 
  };
}
#endif
