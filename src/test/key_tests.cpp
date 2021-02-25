// Copyright (c) 2012-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <base58.h>
#include <key.h>
#include <script/script.h>
#include <uint256.h>
#include <util.h>
#include <utilstrencodings.h>
#include <wallet/wallet.h>

#include <test/test_bitcoin.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <boost/test/unit_test.hpp>

static const std::string addr1 = "exampleuncompressedaddress";
static const std::string strSecret1 = "exampleuncompressedaddress1privkey";

static const std::string addr2 = "exampleuncompressedaddress2";
static const std::string strSecret2 = "exampleuncompressedaddress2privkey";

static const std::string addr1C = "VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW";
static const std::string strSecret1C = "aBfGCimMcpr2ACXnq8rGrzxqJpsiziFSSLjEfgUCHaeNcMupzmVw";

static const std::string addr2C = "VYqLj3D5NGKd6SgSAgqitYN9H5tQtUWcdR";
static const std::string strSecret2C = "aEoXQUdGDGbZNB9VMh8h9UbkjUSRDArERGWoHSGD1Yq84bUNd8Ec";

static const std::string strAddressBad = "ZzzzLc3sNHZxwj4Zk6fB38tEmBryq2cBiF";


BOOST_FIXTURE_TEST_SUITE(key_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(key_test1)
{
    CBitcoinSecret bsecret1, bsecret2, bsecret1C, bsecret2C, baddress1;
    BOOST_CHECK( bsecret1.SetString (strSecret1));
    BOOST_CHECK( bsecret2.SetString (strSecret2));
    BOOST_CHECK( bsecret1C.SetString(strSecret1C));
    BOOST_CHECK( bsecret2C.SetString(strSecret2C));
    BOOST_CHECK(!baddress1.SetString(strAddressBad));

    CKey key1  = bsecret1.GetKey();
    BOOST_CHECK(key1.IsCompressed() == false);
    CKey key2  = bsecret2.GetKey();
    BOOST_CHECK(key2.IsCompressed() == false);

    CKey key1C = bsecret1C.GetKey();
    BOOST_CHECK(key1C.IsCompressed() == true);
    CKey key2C = bsecret2C.GetKey();
    BOOST_CHECK(key2C.IsCompressed() == true);

    CPubKey pubkey1  = key1. GetPubKey();
    CPubKey pubkey2  = key2. GetPubKey();

    CPubKey pubkey1C = key1C.GetPubKey();
    CPubKey pubkey2C = key2C.GetPubKey();

    BOOST_CHECK(key1.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key1C.VerifyPubKey(pubkey1));
    BOOST_CHECK(key1C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey1C));
    BOOST_CHECK(key2.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey2));
    BOOST_CHECK(key2C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(DecodeDestination(addr1)  == CTxDestination(pubkey1.GetID()));
    BOOST_CHECK(DecodeDestination(addr2)  == CTxDestination(pubkey2.GetID()));

    // BOOST_CHECK(DecodeDestination(addr1C) == CTxDestination(pubkey1C.GetID()));
    // int i = 0;
    // for (const auto& dest : GetAllDestinationsForKey(pubkey1C)) {
    //     std::cout << "pubkey1C address #" << i << " = " << EncodeDestination(dest) << std::endl;
    //     i++;
    // }
    std::vector<CTxDestination> pubkey1Cdests = GetAllDestinationsForKey(pubkey1C);
    BOOST_CHECK(DecodeDestination(addr1C) == pubkey1Cdests[0]);

    std::vector<CTxDestination> pubkey2Cdests = GetAllDestinationsForKey(pubkey2C);
    BOOST_CHECK(DecodeDestination(addr2C) == pubkey2Cdests[0]);

    for (int n=0; n<16; n++)
    {
        std::string strMsg = strprintf("Very secret message %i: 11", n);
        uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        // normal signatures

        std::vector<unsigned char> sign1, sign2, sign1C, sign2C;

        BOOST_CHECK(key1.Sign (hashMsg, sign1));
        BOOST_CHECK(key2.Sign (hashMsg, sign2));
        BOOST_CHECK(key1C.Sign(hashMsg, sign1C));
        BOOST_CHECK(key2C.Sign(hashMsg, sign2C));

        BOOST_CHECK( pubkey1.Verify(hashMsg, sign1));
        BOOST_CHECK(!pubkey1.Verify(hashMsg, sign2));
        BOOST_CHECK(!pubkey1.Verify(hashMsg, sign2C));

        BOOST_CHECK(!pubkey2.Verify(hashMsg, sign1));
        BOOST_CHECK( pubkey2.Verify(hashMsg, sign2));
        BOOST_CHECK(!pubkey2.Verify(hashMsg, sign1C));

        BOOST_CHECK(!pubkey1C.Verify(hashMsg, sign2));
        BOOST_CHECK( pubkey1C.Verify(hashMsg, sign1C));
        BOOST_CHECK(!pubkey1C.Verify(hashMsg, sign2C));

        BOOST_CHECK(!pubkey2C.Verify(hashMsg, sign1));
        BOOST_CHECK(!pubkey2C.Verify(hashMsg, sign1C));
        BOOST_CHECK( pubkey2C.Verify(hashMsg, sign2C));

        // compact signatures (with key recovery)

        std::vector<unsigned char> csign1, csign2, csign1C, csign2C;

        BOOST_CHECK(key1.SignCompact (hashMsg, csign1));
        BOOST_CHECK(key2.SignCompact (hashMsg, csign2));
        BOOST_CHECK(key1C.SignCompact(hashMsg, csign1C));
        BOOST_CHECK(key2C.SignCompact(hashMsg, csign2C));

        CPubKey rkey1, rkey2, rkey1C, rkey2C;

        BOOST_CHECK(rkey1.RecoverCompact (hashMsg, csign1));
        BOOST_CHECK(rkey2.RecoverCompact (hashMsg, csign2));
        BOOST_CHECK(rkey1C.RecoverCompact(hashMsg, csign1C));
        BOOST_CHECK(rkey2C.RecoverCompact(hashMsg, csign2C));

        BOOST_CHECK(rkey1  == pubkey1);
        BOOST_CHECK(rkey2  == pubkey2);
        BOOST_CHECK(rkey1C == pubkey1C);
        BOOST_CHECK(rkey2C == pubkey2C);
    }

    // test deterministic signing

    std::vector<unsigned char> detsig, detsigc;
    std::string strMsg = "Very deterministic message";
    uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());
    BOOST_CHECK(key1.Sign(hashMsg, detsig));
    BOOST_CHECK(key1C.Sign(hashMsg, detsigc));

    std::cout << "sed -i 's/key1_" << "detsighex/";
    for (auto& c : detsig)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsig == ParseHex("key1_detsighex"));

    BOOST_CHECK(key2.Sign(hashMsg, detsig));
    BOOST_CHECK(key2C.Sign(hashMsg, detsigc));

    std::cout << "sed -i 's/key2_" << "detsighex/";
    for (auto& c : detsig)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsig == ParseHex("key2_detsighex"));

    BOOST_CHECK(key1.SignCompact(hashMsg, detsig));
    BOOST_CHECK(key1C.SignCompact(hashMsg, detsigc));

    std::cout << "sed -i 's/key1_" << "compact/";
    for (auto& c : detsig)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsig == ParseHex("key1_compact"));

    std::cout << "sed -i 's/key1C_" << "compact/";
    for (auto& c : detsigc)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsigc == ParseHex("key1C_compact"));

    BOOST_CHECK(key2.SignCompact(hashMsg, detsig));
    BOOST_CHECK(key2C.SignCompact(hashMsg, detsigc));

    std::cout << "sed -i 's/key2_" << "compact/";
    for (auto& c : detsig)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsig == ParseHex("key2_compact"));

    std::cout << "sed -i 's/key2C_" << "compact/";
    for (auto& c : detsigc)
        printf("%02hhx", c);
    std::cout << "/' src/test/key_tests.cpp" << std::endl;
    BOOST_CHECK(detsigc == ParseHex("key2C_compact"));
}

BOOST_AUTO_TEST_SUITE_END()

