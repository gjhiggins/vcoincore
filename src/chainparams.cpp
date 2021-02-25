// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2021 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>
#include <pow.h>
#include <arith_uint256.h>

#include <assert.h>
#include <memory>


#define FROMGENESIS 1
static bool regenerate = false;
static bool voluble = false;

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
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
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
    const CScript genesisOutputScript = CScript() << ParseHex("") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
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
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 9999999;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 9999999; // Implements OP_CHECKLOCKTIMEVERIFY opcode.
        consensus.BIP66Height = 9999999; // Enforces DER encoding from given block hight onwards.
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.nCoinbaseMaturity = 100;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        // rule changes require 96% agreement, measured across 4 retargeting periods.
        // 95% consensus is required to accept protocol rule changes
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016 (95% consensus is required to accept protocol rule changes)
        // The 95% consensus must be obtained in 2016 blocks time (window = one retargeting period).
        consensus.nMinerConfirmationWindow = 2016; // The 95% consensus must be obtained in 2016 (or nPowTargetTimespan / nPowTargetSpacing) blocks time (window = one retargeting period).


        // the minimum and maximum times that three softforks can activate and how they are to be signaled using version bits.

        // Satoshi's playground
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // From the documentation : "Block height at which CSV (BIP68, BIP112 and BIP113) becomes active."
        // These 3 BIP's collectively knowh as CheckSequenceVerify = https://en.bitcoin.it/wiki/CheckSequenceVerify
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // May 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 0; // May 1st, 2017

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 999999999999ULL; // NOTE: DATACOIN segwit disabled (was SEGWIT = 1479168000; // November 15th, 2016.)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL; // NOTE: DATACOIN segwit disabled (was SEGWIT =1510704000; // November 15th, 2017.)

        // The best chain should have at least this much work and newly mined blocks must have a hash smaller than this
        // Loading blocks will not start until the headers reach this level.
        consensus.nMinimumChainWork = uint256S("0000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("00000000000000000013176bf8d7dfeab4e1db31dc93bc311b436e82ab226b90"); //453354

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x05;
        pchMessageStart[1] = 0x05;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 5530;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1431517588, MAINNETNONCE, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("mainnet: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("mainnet: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("mainnet: %s\n", consensus.powLimit.ToString().c_str());

        assert(consensus.hashGenesisBlock == uint256S(MAINNETGENESISHASH));
        assert(genesis.hashMerkleRoot == uint256S(MERKLETREEROOTHASH));

        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("minkiz.co");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);  // addresses start with 'V' - 0x46
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);  // script addresses start with '7' - 0x1e
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,96 + 128); // private keys start with '7' or 'V' - 0xc6
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5A, 0xD4, 0xD7}; // BIP32 pubkeys start with 'vcpb'
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5A, 0xD5, 0x01}; // BIP32 prvkeys start with 'vcpv'
        base58Prefixes[EXT_COIN_TYPE]  = {0x80, 0x00, 0x00, 0x028}; // BIP44 coin type is '28'

        bech32_hrp = "vc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        // Whether to check mempool and block index consistency by default
        fDefaultConsistencyChecks = false;
        // Filter out transactions that don't comply with the Bitcoin Core standard
        fRequireStandard = true;

        fMineBlocksOnDemand = false;
        fMiningRequiresPeers = true;

        // Blockchain is assumed valid before each of these.
        /**
         * What makes a good checkpoint block?
         * + Is surrounded by blocks with reasonable timestamps
         *   (no blocks before with a timestamp after, none after with
         *    timestamp before)
         * + Contains no strange transactions
         */
        checkpointData = {
            {
                {0, uint256S(MAINNETGENESISHASH)},
                {500000, uint256S("00000000000329d124fce422ab9a061886c9e7d3985939c82c7d7d9a69f886c7")},
                {1000000, uint256S("000000000018081c155a551e258ce12851a4ecfdc330ad6521cd2d1b82c65109")},
                {1500000, uint256S("0000000000327ed56a55dcd7cc1bcf9728474be64a6fd7c385ec3ed2e51d85fc")},
                {2000000, uint256S("000000000032a43666eeda7cff4571dbaeb424438e37c9f6b96c28720fe5a48Z")},
                {2500000, uint256S("00000000042125f842c3a27f2a6b5420bd566c780869e09ef87b3b7b500d2db9")},
                {3000000, uint256S("000000000a9aa04df5ea04919e5e99eb6a17cdbcc92a75b458584c32402ad81a")},
                {3500000, uint256S("000000005c06cdac01ff6014d3e6104ab75947b83af5b136e2b1f30b4404a866")},
            }
        };

        // Transaction data from a certain block onwards.
        chainTxData = ChainTxData{
            1563440362, // * UNIX timestamp of last checkpoint block
            3000000,    // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            2880,       // * estimated number of transactions per day after checkpoint
            0,
            0,
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = FROMGENESIS;
        consensus.BIP34Hash = uint256S("000007c2d96e3435b752fc25a219ff70c963540d07c73243419b8e9274456f39");
        consensus.BIP65Height = FROMGENESIS;
        consensus.BIP66Height = FROMGENESIS;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.nCoinbaseMaturity = 10;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        // rule changes require 96% agreement, measured across 4 retargeting periods.
        // 95% consensus is required to accept protocol rule changes
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        // the minimum and maximum times that three softforks can activate and how they are to be signaled using version bits.

        // Satoshi's playground
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        // From the documentation : "Block height at which CSV (BIP68, BIP112 and BIP113) becomes active."
        // These 3 BIP's collectively knowh as CheckSequenceVerify = https://en.bitcoin.it/wiki/CheckSequenceVerify
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1456790400; // March 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1462060800; // May 1st 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1493596800; // May 1st 2017

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("000007e14c52364cee2d4d9483541d473e3e73c896df75882273b91313b44816"); // 0

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x01;
        pchMessageStart[1] = 0xfe;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 55534;
        nPruneAfterHeight = 1000;

        if (voluble) {
            std::cout << "Creating Testnet genesis block with ntime 1581969864, nonce " << TESTNETNONCE << ", nbits 0x1e0fffff, nversion 1, value 1 * COIN" << std::endl;
            std::cout.flush();
        }
        genesis = CreateGenesisBlock(1581969864, TESTNETNONCE, 0x1e0fffff, 1, 1 * COIN);
        if (voluble) {
            std::cout << "Testnet genesis block created\n";
            std::cout.flush();
        }
        consensus.hashGenesisBlock = genesis.GetHash();

        if (regenerate) {
            if (voluble) {
                std::cout << "Regenerating testnet genesis hash\n";
                std::cout << "genesis hash: " << consensus.hashGenesisBlock.ToString().c_str() << std::endl;
                printf("NOTE: Genesis nTime = %u \n", genesis.nTime);
                printf("WARN: Genesis nNonce (BLANK!) = %u \n", genesis.nNonce);
                std::cout.flush();
            }
            genesis.nNonce = 0;
            arith_uint256 hashTarget = UintToArith256(consensus.powLimit);
            while (UintToArith256(genesis.GetHash()) > hashTarget)
            {
                ++genesis.nNonce;
                if (genesis.nNonce == 0) ++genesis.nTime;
            }
            std::cout << "Testnet ---\n";
            std::cout << "  nonce: " << genesis.nNonce <<  "\n";
            std::cout << "   time: " << genesis.nTime << "\n";
            std::cout << "   hash: " << genesis.GetHash().ToString().c_str() << "\n";
            std::cout << "   merklehash: "  << genesis.hashMerkleRoot.ToString().c_str() << "\n";
            exit(0);
        } else {
            LogPrintf("testnet ---\n");
            LogPrintf("nonce: %u\n", genesis.nNonce);
            LogPrintf("time: %u\n", genesis.nTime);
            LogPrintf("hash: 0x%s\n", genesis.GetHash().ToString().c_str());
            LogPrintf("merklehash: 0x%s\n", genesis.hashMerkleRoot.ToString().c_str());
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S(TESTNETGENESISHASH));
            assert(genesis.hashMerkleRoot == uint256S(TESTNETMERKLETREEROOTHASH));
        }

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("minkiz.co");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,130); // testnet addresses start with 'V' - 0x7f
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);  // testnet script addresses start with '7' - 0x1e
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,111 + 128); // testnet private keys start with '7' or 'V' - 0xff
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5A, 0xCE, 0x83}; // BIP32 testnet pubkeys start with 'drkv'
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5A, 0xCE, 0xAD}; // BIP32 testnet prvkeys start with 'drkp'
        base58Prefixes[EXT_COIN_TYPE]  = {0x80, 0x00, 0x00, 0x01}; // BIP44 testnet coin type is '1'

        bech32_hrp = "vct";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;


        checkpointData = {
            {
                {0, uint256S(TESTNETGENESISHASH)},
            }
        };

        chainTxData = ChainTxData{
            1374901773,
            0,
            2880,
            0,
            0,
        };
    }
};

/**
 * Regression test
 */

class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 0; // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = FROMGENESIS; // BIP65 activated on regtest (Used in functional tests)
        // For spending tests, you have to create a long chain. But the bitcoin test is designed for a height of (100)
        // which is less, so increase BIP66Height to be > 3200
        consensus.BIP66Height = FROMGENESIS; // BIP66 activated on regtest (Used in functional tests)
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.nCoinbaseMaturity = 10;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x0f;
        pchMessageStart[2] = 0xa5;
        pchMessageStart[3] = 0x5a;
        nDefaultPort = 56534;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1296688602, REGTESTNONCE, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("regtest: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("regtest: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("regtest: %x\n", consensus.powLimit.ToString().c_str());

        if (regenerate) {
            if (voluble) {
                std::cout << "Regenerating regtest genesis hash\n";
                std::cout << "genesis hash: " << consensus.hashGenesisBlock.ToString().c_str() << std::endl;
                printf("NOTE: Genesis nTime = %u \n", genesis.nTime);
                printf("WARN: Genesis nNonce (BLANK!) = %u \n", genesis.nNonce);
                std::cout.flush();
            }
            genesis.nNonce = 0;
            // arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
            arith_uint256 hashTarget = UintToArith256(consensus.powLimit);
            // while (UintToArith256(genesis.GetHash()) > UintToArith256(consensus.powLimit))
            while (UintToArith256(genesis.GetHash()) > hashTarget)
            {
                ++genesis.nNonce;
                if (genesis.nNonce == 0) ++genesis.nTime;
            }
            std::cout << "dict(nonce=" << genesis.nNonce;
            std::cout << ", time=" << genesis.nTime;
            std::cout << ", hash=\"" << genesis.GetHash().ToString().c_str();
            std::cout << "\", merklehash=\""  << genesis.hashMerkleRoot.ToString().c_str() <<  "\")\n";
            exit(0);
        } else {
            LogPrintf("regtest ---\n");
            LogPrintf(" nonce: %u\n", genesis.nNonce);
            LogPrintf(" time: %u\n", genesis.nTime);
            LogPrintf(" hash: 0x%s\n", genesis.GetHash().ToString().c_str());
            LogPrintf(" merklehash: 0x%s\n", genesis.hashMerkleRoot.ToString().c_str());
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S(REGTESTGENESISHASH));
            assert(genesis.hashMerkleRoot == uint256S(MERKLETREEROOTHASH));

        }

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = true;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0); // regtest addresses start with 'V'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5); // regtest script addresses start with '7'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128); // regtest private keys start with '7' or 'V'
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF}; // BIP32 regtest pubkeys start with 'drkv'
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94}; // BIP32 regtest prvkeys start with 'drkp'
        base58Prefixes[EXT_COIN_TYPE]  = {0x80, 0x00, 0x00, 0x01}; // BIP44 regtest coin type is '1'

        bech32_hrp = "vcrt";

        checkpointData = {
            {
                {0, uint256S(REGTESTGENESISHASH)},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0,
            0,
            0
        };
        if (regenerate)
            exit(0);
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}

/* Extended keys

    Mainnet:
        Legacy:
        x = P2PKH or P2SH

        Segwit:
        y = P2WPKH in P2SH
        Y = Multi-signature P2WSH in P2SH 

        Bech32:
        z = P2WPKH
        Z = Multi-signature P2WSH

    Testnet:
        Legacy:
        t = P2PKH/P2SH

        Segwit:
        u = P2WPKH in P2SH
        U = Multi-signature P2WSH in P2SH

        Bech32:
        v = P2WPKH
        V = Multi-signature P2WSH

{
'public': '045AD4D7',
'private': '045AD501',
'publicbytes': '{0x04, 0x5A, 0xD4, 0xD7}',
'privatebytes': '{0x04, 0x5A, 0xD5, 0x01}'
}
{
'p2pkhpub': '045AD4D7',
'p2pkhprv': '045AD501',
'p2shpub': '045AD4D7',
'p2shprv': '045AD501',

'p2sh_p2wpkhpub': '045AD66C',
'p2sh_p2wpkhprv': '045AD696',
'p2sh_p2wshpub': '045AAEDE',
'p2sh_p2wshprv': '045AAF08',

'p2wpkhpub': '045AD801',
'p2wpkhprv': '045AD82B',
'p2wshpub': '045AB073',
'p2wshprv': '045AB09D'

}

{
'public': '045ACE83',
'private': '045ACEAD',
'publicbytes': '{0x04, 0x5A, 0xCE, 0x83}',
'privatebytes': '{0x04, 0x5A, 0xCE, 0xAD}'
}
{
'p2pkhpub': '045ACE83',
'p2pkhprv': '045ACEAD',
'p2shpub': '045ACE83',
'p2shprv': '045ACEAD',

'p2sh_p2wpkhpub': '045AD018',
'p2sh_p2wpkhprv': '045AD042',
'p2sh_p2wshpub': '045AA889',
'p2sh_p2wshprv': '045AA8B3',

'p2wpkhpub': '045AD1AD',
'p2wpkhprv': '045AD1D7',
'p2wshpub': '045AAA1F',
'p2wshprv': '045AAA48'
}
*/
