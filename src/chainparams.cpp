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

/*
void  CChainParams::MineNewGenesisBlock()
{
    //This will output (to stdout) the code for a new genesis block when it is found
    genesis.nTime=time(NULL);
    genesis.nNonce=0;
    uint256 thash;
    while(1)
    {
        thash=genesis.GetHash();
        if (this->CheckProofOfWork(thash, genesis.nBits))
            break;
        if ((genesis.nNonce & 0xFF) == 0)
        {
            printf("nonce %08X: hash = %s\n",genesis.nNonce, thash.ToString().c_str());
        }
        ++genesis.nNonce;
        if (genesis.nNonce == 0)
        {
            printf("NONCE WRAPPED, incrementing time\n");
            ++genesis.nTime;
        }
    }
    LogPrintf("genesis.nTime = %u;\n",genesis.nTime);
    LogPrintf("genesis.nNonce = %u;\n",genesis.nNonce);
    LogPrintf("assert(genesis.hashMerkleRoot == uint256(\"0x%s\"));\n",genesis.hashMerkleRoot.ToString().c_str());
    LogPrintf("//genesis hash: 0x%s\n", genesis.GetHash().ToString().c_str());
    LogPrintf("//genesis hash: 0x%s\n", genesis.GetHash().ToString().c_str());
    exit(1);
}

//need a different implementation here that doesn't use error() and that doesn't use Params() since it isn't yet usable
bool CChainParams::CheckProofOfWork(uint256 hash, unsigned int nBits)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow).getuint256();

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return false;

    return true;
}
*/

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
        vAlertPubKey = ParseHex("0x04ddf6acee2cdba487e3bb2ec67ba96abd5bcc06ab62e79d00f745d9021d38b3739b2001bfc13055150047bd6b6a958e797be065fe05c9a8c1486605c169c5a12e");
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

        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(70);
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(30);
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(224);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        /*
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70); // ‘V’  (P2PKH address)
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30); // ‘D’ (P2SH address) multisig begins with D ??? (Maybe 133, ‘v’?)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,224); // [PUBKEY_ADDRESS] (70) + 128 = 198
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB0)(0x1E); // 'xpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xA0)(0xE4); // 'xpriv'
        // base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000025); // BIP44 coin type is '25'
        */

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
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
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
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

        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(127);
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(30);
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(255);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        /*
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,130); // ‘u’ (P2PKH address)
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30); // ‘D’ (P2SH address)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,258); // [PUBKEY_ADDRESS] (130) + 128 = 258 Private key (WIF, compressed pubkey)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x81)(0xCF); // 'tpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x80)(0x94); // 'tpriv'
        // base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000025); // BIP44 coin type is '25'
        */

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = false;
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
class CRegTestParams : public CTestNetParams {
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
        }
        tinyformat::printf("%s\n", hashGenesisBlock.ToString().c_str());
        tinyformat::printf("%d\n", genesis.nNonce);
        */
        // assert(consensus.hashGenesisBlock == uint256S(""));

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
