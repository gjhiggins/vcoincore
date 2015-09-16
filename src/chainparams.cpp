// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "util.h"
#include "utilstrencodings.h"
#include "arith_uint256.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    return genesis;
}

/**
    * Build the genesis block. Note that the output of its generation
    * transaction cannot be spent since it did not originally exist in the
    * database.
    *
    * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
    *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
    *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
    *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
    *   vMerkleTree: 4a5e1e
    */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "vcoin sn";
    // const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    const CScript genesisOutputScript = CScript() << ParseHex("") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x05;
        pchMessageStart[1] = 0x05;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0x05;
        vAlertPubKey = ParseHex("0x04ddf6acee2cdba487e3bb2ec67ba96abd5bcc06ab62e79d00f745d9021d38b3739b2001bfc13055150047bd6b6a958e797be065fe05c9a8c1486605c169c5a12e");
        nDefaultPort = 5530;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1431517588, 1486592, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633"));
        assert(genesis.hashMerkleRoot == uint256S("0x1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vSeeds.push_back(CDNSSeedData("Minkiz", "minkiz.co"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,224);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        /*
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB0)(0x1E); // 'xpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xA0)(0xE4); // 'xpriv'
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000025); // BIP44 coin type is '25'
        */

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633")),
            1431517588, // * UNIX timestamp of last checkpoint block
            0,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            2880     // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = true;
        pchMessageStart[0] = 0x01;
        pchMessageStart[1] = 0xfe;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0x05;
        // vAlertPubKey = ParseHex("0x");
        nDefaultPort = 55534;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1441062000, 1173545, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        /*
        // calculate Genesis Block
        if (true && (genesis.GetHash() != consensus.hashGenesisBlock)) {
            LogPrintf("Calculating Genesis Block:\n");

            arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
            uint256 hash;
            genesis.nNonce = 0;

            // This will figure out a valid hash and Nonce if you're
            // creating a different genesis block:
            // uint256 hashTarget = CBigNum().SetCompact(genesis.nBits).getuint256();
            // hashTarget.SetCompact(genesis.nBits, &fNegative, &fOverflow).getuint256();
            // while (genesis.GetHash() > hashTarget)
            while (UintToArith256(genesis.GetHash()) > hashTarget)
            {
                ++genesis.nNonce;
                if (genesis.nNonce == 0)
                {
                    LogPrintf("NONCE WRAPPED, incrementing time");
                    ++genesis.nTime;
                }
                if (genesis.nNonce % 10000 == 0)
                {
                    LogPrintf("nonce %08u: hash = %s \n", genesis.nNonce, genesis.GetHash().ToString().c_str());
                }
            }
        }

        LogPrintf("testnet:nonce %08u\n", genesis.nNonce);
        LogPrintf("testnet:hash %s\n", genesis.GetHash().ToString().c_str());
        LogPrintf("testnet:merkle %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("testnet:nbits %x\n", consensus.powLimit.ToString().c_str());
        */

        assert(consensus.hashGenesisBlock == uint256S("0x000007e14c52364cee2d4d9483541d473e3e73c896df75882273b91313b44816"));
        assert(genesis.hashMerkleRoot == uint256S("0x1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("MinkizT", "minkiz.co"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        /*
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x81)(0xCF); // 'tpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x80)(0x94); // 'tpriv'
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000025); // BIP44 coin type is '25'
        */

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x000007e14c52364cee2d4d9483541d473e3e73c896df75882273b91313b44816")),
            1374901773,
            0,
            2880
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = true;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x0f;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0x5a;
        nDefaultPort = 56534;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1296688602, 3, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        /*
        assert(consensus.hashGenesisBlock == uint256S("0x0"));
        assert(genesis.hashMerkleRoot == uint256S("0x0"));
        LogPrintf("regtest: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("regtest: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("regtest: %x\n", consensus.powLimit.ToString().c_str());
		*/
        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x0")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,238);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        /*
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x81)(0xCF); // 'tpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x80)(0x94); // 'tpriv'
        base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000025); // BIP44 coin type is '25'
        */
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}

/* Current genesis
    genesis.nBits    = UintToArith256(consensus.powLimit).GetCompact();
    genesis.nBits    = 0x1e0fffff;
    genesis.nTime    = 1431517588;
    genesis.nNonce   = 1486592;
    assert(hashGenesisBlock == uint256("0x00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633"));
    assert(genesis.hashMerkleRoot == uint256("0x1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));
*/


/* 0.8 varsnvals

static const int64 MAX_MONEY = 1000000000 * COIN;


static const int64 nTargetTimespan = 1200; // 20 minutes
static const int64 nTargetSpacing = 30; // 30 seconds
static const int64 nInterval = nTargetTimespan / nTargetSpacing; // 40 blocks

static const int64 nAveragingInterval = nInterval * 20; // 40 blocks
static const int64 nAveragingTargetTimespan = nAveragingInterval * nTargetSpacing; // 40 minutes

static const int64 nMaxAdjustDown = 28; // 28% adjustment down
static const int64 nMaxAdjustUp = 18; // 18% adjustment up


static const int64 nTargetTimespanAdjDown = nTargetTimespan * (100 + nMaxAdjustDown) / 100;

    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        pchMessageStart[0] = 0x05;
        pchMessageStart[1] = 0x05;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 5530;
        nRPCPort = 5531;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);
        nSubsidyHalvingInterval = 100000;
        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        const char* pszTimestamp = "vcoin sn";
        CTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1431517588;
        genesis.nBits    = 0x1e0fffff;
        genesis.nNonce   = 1486592;
        assert(hashGenesisBlock == uint256("0x00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633"));
        assert(genesis.hashMerkleRoot == uint256("0x1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));
        vSeeds.push_back(CDNSSeedData("someaddress.com or IP addy", "someaddress.com"));
        base58Prefixes[PUBKEY_ADDRESS] = 70;
        base58Prefixes[SCRIPT_ADDRESS] = 30;
        base58Prefixes[SECRET_KEY] = 224;

    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        pchMessageStart[0] = 0x01;
        pchMessageStart[1] = 0xfe;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 55534;
        nRPCPort = 55535;
        strDataDir = "testnet";
        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1374901773;
        genesis.nNonce = 1211565;
        base58Prefixes[PUBKEY_ADDRESS] = 130;
        base58Prefixes[SCRIPT_ADDRESS] = 30;
        base58Prefixes[SECRET_KEY] = 239;

    CRegTestParams() {
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x0f;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0x5a;
        nSubsidyHalvingInterval = 150;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 3;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        strDataDir = "regtest";
        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
        base58Prefixes[PUBKEY_ADDRESS] = 0;
        base58Prefixes[SCRIPT_ADDRESS] = 5;
        base58Prefixes[SECRET_KEY] = 128;

*/
