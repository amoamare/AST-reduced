#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <utils.cpp>
#include <tinyaes/TinyAESWrapper.cpp>

namespace Amosoft
{
  class Encryption
  {

    private:
      Amosoft::TinyAESWrapper::VBytes key;
      Amosoft::TinyAESWrapper aesTiny;

      inline void GenerateAesKey()
      {
        int index = 0;
        do {
          char value = (char)(214719 * (index + 985471) % 255);
          key.push_back(value);
          index++;
        } while (index != 256/8);
      }

      void Initialize()
      {
        GenerateAesKey();
        #ifndef NDEBUG
//        std::cout << "############### DEBUG PARAMETERS ###############" << std::endl;
//        std::cout << "# AesKey HEX: " << UtilsClass::StringToHex(AesKey) << std::endl;
//        std::cout << "############### DEBUG PARAMETERS ###############" << std::endl;
//        std::cout << std::endl;
        #endif
      }

    public:

      Encryption()
      {
    	   key.reserve(256/8);
         Initialize();
      }

      const std::string DecryptInput(const std::string command)
      {
        return DecryptStringFromHex(command);
      }

      void SetAesKey(const std::string aesKey)
      {
        if (aesKey.empty()) return;
        key = aesTiny.from_hex_string(aesKey);
      }

      const void EncryptAndPrint(const std::string message)
      {
        std::cout << "output:" << EncryptStringToHex(message) << std::endl;
      }

      const std::string EncryptStringToHex(const std::string& data)
      {
        TinyAESWrapper::VBytes iv;
        aesTiny.generate_random_iv(iv);
        return aesTiny.to_hex_string(iv) + aesTiny.cbc_encrypt_hex(data, key, iv);
      }

      const std::string DecryptStringFromHex(const std::string& data)
      {
        TinyAESWrapper::VBytes iv = aesTiny.from_hex_string(data.substr(0, 32));
        return aesTiny.cbc_decrypt_hex(data.substr(32), key, iv);

      }

      const std::string GetRandomChallenge()
      {
        TinyAESWrapper::VBytes random;
        aesTiny.generate_random(random, 32);
        return aesTiny.to_hex_string(random);
      }
  };
}

#endif