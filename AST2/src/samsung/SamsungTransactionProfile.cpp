#include "ast2/samsung/SamsungTransactionProfile.hpp"

namespace ast2::samsung {

SamsungTransactionProfile SamsungTransactionProfile::N975UAndroid12() {
    SamsungTransactionProfile profile;
    profile.profileName = "SM-N975U Android 12";
    profile.serviceName = "isemtelephony";
    profile.descriptor = "com.android.internal.telephony.ISemTelephony";
    profile.sendRequestToRil = 14;
    profile.invokeOemRilRequestRawForSubscriber = 28;
    return profile;
}

} // namespace ast2::samsung
