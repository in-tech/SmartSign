#include "CryptoUtils.h"
#include <Crypto.h>
#include "CwaQRCode.h"
#include "Log.h"

String CryptoUtils::Sha256(const String& input)
{
    SHA256 hasher;
    hasher.doUpdate(input.c_str());

    byte hash[SHA256_SIZE];
    hasher.doFinal(hash);
    
    String result;
    result.reserve(SHA256_SIZE * 2);
    for (byte i=0; i < SHA256_SIZE; i++)
    {
        if (hash[i]<0x10)
        {
            result += "0";
        }
        result += String(hash[i], 16);
    }
    result.toUpperCase();

    return result;
}

String CryptoUtils::RandomString(const String& alphabet, const int length)
{
    String result;
    result.reserve(length);
    for (int i = 0; i < length; i++)
    {
        result += alphabet[esp_random() % alphabet.length()];
    }
    return result;
}

String CryptoUtils::CreateCwaEventCode(const ScheduleItem& item, const String& address)
{
    String result = "";

    GOOGLE_PROTOBUF_VERIFY_VERSION;    

    QRCodePayload payload;
    payload.set_version(1);

    TraceLocation &locationData = *payload.mutable_locationdata();
    locationData.set_version(1);
    locationData.set_description(item.Subject.substring(0, 100).c_str());
    locationData.set_address(address.substring(0, 100).c_str());
    locationData.set_starttimestamp(TimeUtils::ToUtcTime(item.LocalStartTime));
    locationData.set_endtimestamp(TimeUtils::ToUtcTime(item.LocalEndTime));

    CrowdNotifierData &crowdNotifierData = *payload.mutable_crowdnotifierdata();
    crowdNotifierData.set_version(1);
    crowdNotifierData.set_publickey(cwa::publicKey, sizeof(cwa::publicKey));

    const auto hash = Sha256(address + item.Subject);
    std::vector<uint8_t> seed(16);
    for (int i = 0; i < std::min(seed.size(), hash.length()); ++i)
    {
        seed[i] = hash[i];
    }
    crowdNotifierData.set_cryptographicseed(seed.data(), seed.size());

    CWALocationData countryData;
    countryData.set_version(1);
    countryData.set_type(LOCATION_TYPE_TEMPORARY_OTHER);
    const uint32_t minutes = (item.LocalEndTime - item.LocalStartTime) / 60;
    countryData.set_defaultcheckinlengthinminutes(minutes);
    countryData.SerializeToString(payload.mutable_countrydata());

    std::vector<uint8_t> buffer(payload.ByteSizeLong());
    if (payload.SerializeToArray(buffer.data(), buffer.size()))
    {
        result = String(cwa::urlPrefix) + base64url::encode((char *)buffer.data(), buffer.size()).c_str();
    }

    return result;
}