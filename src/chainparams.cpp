// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "util.h"
#include "pow.h"
#include "utilstrencodings.h"
#include "arith_uint256.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
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
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
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
        // consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 32);
        // consensus.powLimit = uint256S("0x00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); ~arith_uint256(0) >> 32
        // consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 20);
        consensus.powLimit =  uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
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
        vAlertPubKey = ParseHex("0x0");
        nDefaultPort = 5530;
        nMinerThreads = 0;
        nPruneAfterHeight = 100000;

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
        const char* pszTimestamp = "vcoin sn";
        CMutableTransaction txNew;
        txNew.nVersion = 1;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1431517588;
        genesis.nBits = UintToArith256(consensus.powLimit).GetCompact();
        // genesis.nBits = 0x1e0fffff;
        genesis.nNonce = 1486592;

        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("mainnet: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("mainnet: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("mainnet: %x\n", consensus.powLimit.ToString().c_str());
        // genesis.print();

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

        assert(consensus.hashGenesisBlock == uint256S("0x00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633"));
        assert(genesis.hashMerkleRoot == uint256S("0x1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vSeeds.push_back(CDNSSeedData("Minkiz", "minkiz.co"));

        // base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        // base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        // base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(70);
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(30);
        base58Prefixes[SECRET_KEY] =     boost::assign::list_of(224);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData) {
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
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[1] = 0x01;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0xfe;
        pchMessageStart[3] = 0x05;
        vAlertPubKey = ParseHex("0x");
        nDefaultPort = 55534;
        nMinerThreads = 1;
        nPruneAfterHeight = 1000;

        // consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 20);
        // consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
        consensus.powLimit =  uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        // consensus.hashGenesisBlock = uint256S("0x0000031225834a1423f72fc7f8371e46b0ed172da9a9242edb891902abb85759");
        // consensus.hashGenesisBlock = uint256S("0x7d62df99736efdf7349b48c8fe3839c8fd4a725ab594f150037a178a79b9a1ff");
        consensus.hashGenesisBlock = uint256S("0x00000b62077c6e53a2c7ada6e14feb5dfdeecd6001cc83d3a281145572112b17");

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nBits = UintToArith256(consensus.powLimit).GetCompact();
        // genesis.nBits = 0x207fffff;
        // genesis.nBits = 0x1e0fffff;
        genesis.nTime = 1374901773;
        genesis.nNonce = 1211565;

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

        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("testnet: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("testnet: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("testnet: %x\n", consensus.powLimit.ToString().c_str());

        assert(consensus.hashGenesisBlock == uint256S("0x00000b62077c6e53a2c7ada6e14feb5dfdeecd6001cc83d3a281145572112b17"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("MinkizT", "minkiz.co"));

        // base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        // base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(130);
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(30);
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x0")),
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
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x0f;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0x5a;
        consensus.nSubsidyHalvingInterval = 150;
        // consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.nPowTargetTimespan =  0; 
        consensus.nPowTargetSpacing = 0; 
        consensus.powLimit = ArithToUint256(~arith_uint256(0) >> 1);
        
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 3;

        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("regtest: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("regtest: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("regtest: %x\n", consensus.powLimit.ToString().c_str());

        // assert(consensus.hashGenesisBlock == uint256S("0x13d8d31dde96874006da503dd2b63fa68c698dc823334359e417aa3a92f80433"));

        /*
        while (hashGenesisBlock > bnProofOfWorkLimit.getuint256()){
            if (++genesis.nNonce==0) break;
            consensus.hashGenesisBlock = genesis.GetHash();

        genesis = CreateGenesisBlock(1296688602, 2, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
        assert(genesis.hashMerkleRoot == uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        nMinerThreads = 1;
        nDefaultPort = 56534;
        nPruneAfterHeight = 1000;
        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x0")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
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
