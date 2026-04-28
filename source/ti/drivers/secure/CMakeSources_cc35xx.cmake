cmake_minimum_required(VERSION 3.21.3)

set(SOURCES_CC35XX
    ../AESCBC.c
    ../AESCCM.c
    ../AESCMAC.c
    ../AESCTR.c
    ../AESECB.c
    ../AESGCM.c
    # ../crypto/CryptoCC32XX.c
    ../cryptoutils/cryptokey/CryptoKey.c
    ../cryptoutils/cryptokey/CryptoKeyPlaintext.c
    ../cryptoutils/hsm/HSMXXF3.c
    ../cryptoutils/hsm/HSMXXF3Utility.c
    ../cryptoutils/utils/CryptoUtils.c
    ../utils/Random.c
    ../ECDH.c
    ../ECDSA.c
    ../EDDSA.c
    ../psa/ti_psa_crypto_helper.c
    ../psa/ti_psa_crypto_wrapper.c
    ../RNG.c
    ../SHA2.c
    ../TRNG.c
)

set(SOURCES_SEC_ONLY_CC35XX
    ${SOURCES_CC35XX}
    ../aescbc/AESCBCXXF3.c
    ../aesccm/AESCCMXXF3.c
    ../aescmac/AESCMACXXF3.c
    ../aesctr/AESCTRXXF3.c
    ../aesecb/AESECBXXF3.c
    ../aesgcm/AESGCMXXF3HSM.c
    ../ecdh/ECDHXXF3HSM.c
    ../cryptoutils/ecc/ECCParamsXXF3HSM.c
    ../cryptoutils/cryptokey/CryptoKeyKeyStore_PSA_helpers.c
    ../cryptoutils/cryptokey/CryptoKeyKeyStore_PSA.c
    ../ecdsa/ECDSAXXF3HSM.c
    # ../ecjpake/ECJPAKECC26X2.c
    ../eddsa/EDDSAXXF3HSM.c
    ../psa/ti_psa_crypto.c
    ../rng/RNGXXF3HSM.c
    ../sha2/SHA2XXF3HSM.c
    ../trng/TRNGXXF3HSM.c
)
