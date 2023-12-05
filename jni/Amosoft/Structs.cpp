#ifndef STRUCTS_H
#define STRUCTS_H

#include "Encryption.cpp"
#include "IRilConnection.cpp"

struct InitilizeHandle
{
    Amosoft::IRilConnection *RilDirect;
    Amosoft::IRilConnection	*RilJvm;
    Amosoft::Encryption Encryption;
};

struct JvmCodeHandle
{
    int InvokeCode;
    int DeviceCode;
    int SendRequestCode;
    bool IsSendRequest = SendRequestCode > 0;
};

#endif