// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <arith_uint256.h>
#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <pow.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#define FROMGENESIS 1

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

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 100000;
        consensus.BIP16Exception = uint256S("00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633");
        consensus.BIP34Height = FROMGENESIS;
        consensus.BIP34Hash = uint256S("00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633");
        consensus.BIP65Height = 4000000; // Implements OP_CHECKLOCKTIMEVERIFY opcode.
        consensus.BIP66Height = 4000000; // Enforces DER encoding from given block hight onwards.
        consensus.CSVHeight = FROMGENESIS; // Block height at which CheckSequenceVerify (BIP68, BIP112 and BIP113) becomes active.
        consensus.SegwitHeight = 4000000 /*FROMGENESIS*/;
        // consensus.MinBIP9WarningHeight = 483840; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20 or 1 / 2^12 or 0.000244140625 or “20 zeroes followed by (256-20) ones”
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016 (95% consensus is required to accept protocol rule changes)
        consensus.nMinerConfirmationWindow = 2016; // The 95% consensus must be obtained in 2016 (or nPowTargetTimespan / nPowTargetSpacing) blocks time (window = one retargeting period).
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work and newly mined blocks must have a hash smaller than this
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
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 0.5;

        genesis = CreateGenesisBlock(1431517588, 1486592, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("mainnet: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("mainnet: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("mainnet: %s\n", consensus.powLimit.ToString().c_str());

        /*
        // Reset consensus to zero
        consensus.hashGenesisBlock = uint256S("0x");
        // calculate Genesis Block
        if (true && (genesis.GetHash() != consensus.hashGenesisBlock)) {
            printf("Calculating Genesis Block:\n");
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
                    printf("NONCE WRAPPED, incrementing time");
                    ++genesis.nTime;
                }
                // if (genesis.nNonce % 10000 == 0)
                // {
                //     printf("nonce %08u: hash = %s \n", genesis.nNonce, genesis.GetHash().ToString().c_str());
                // }
            }
            std::cout << "Mainnet ---\n";
            std::cout << "  nonce: " << genesis.nNonce <<  "\n";
            std::cout << "   time: " << genesis.nTime << "\n";
            std::cout << "   hash: " << genesis.GetHash().ToString().c_str() << "\n";
            std::cout << "   merklehash: "  << genesis.hashMerkleRoot.ToString().c_str() << "\n";
            // Mainnet --- nonce: {{nnonce}} time: {{ntime}} hash: {{genesisblockhash}}
        }
        */
        std::cout << "Mainnet ---\n";
        std::cout << "  nonce: " << genesis.nNonce <<  "\n";
        std::cout << "  nbits: " << genesis.nBits << "\n";
        std::cout << "  time: " << genesis.nTime << "\n";
        std::cout << "  hash: " << genesis.GetHash().ToString().c_str() << "\n";
        std::cout << "  merklehash: "  << genesis.hashMerkleRoot.ToString().c_str() << "\n";

        assert(consensus.hashGenesisBlock == uint256S("00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633"));
        assert(genesis.hashMerkleRoot == uint256S("1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vSeeds.emplace_back("minkiz.co", false);

        // base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);   // 0x46
        // base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);   // 0x1e
        // base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,224);  // 0xc6
        // base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5F, 0x1C, 0xF8}; // vpub
        // base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5F, 0x18, 0xC0}; // vprv
        /* base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x80000028); // BIP44 coin type is '28' */
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);                    // VCoin addresses start with 'V'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);                    // VCoin script addresses start with '7'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,96 + 128);              // VCoin private keys start with '7' or 'V'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 pubkeys start with 'drkv'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 prvkeys start with 'drkp'

        bech32_hrp = "vc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false; // Whether to check mempool and block index consistency by default
        fRequireStandard = true; // Filter out transactions that don't comply with the Bitcoin Core standard
        fMineBlocksOnDemand = false;
        m_is_test_chain = false;

        checkpointData = {
            {
                {0, uint256S("00000b7e804f0de87e7752550ff04d7686a4599509897feefd7f03904eb45633")},
                {500000, uint256S("00000000000329d124fce422ab9a061886c9e7d3985939c82c7d7d9a69f886c7")},
                {1000000, uint256S("000000000018081c155a551e258ce12851a4ecfdc330ad6521cd2d1b82c65109")},
                {1500000, uint256S("0000000000327ed56a55dcd7cc1bcf9728474be64a6fd7c385ec3ed2e51d85fc")},
                {2000000, uint256S("000000000032a43666eeda7cff4571dbaeb424438e37c9f6b96c28720fe5a48Z")},
                {2500000, uint256S("00000000042125f842c3a27f2a6b5420bd566c780869e09ef87b3b7b500d2db9")},
                {3000000, uint256S("000000000a9aa04df5ea04919e5e99eb6a17cdbcc92a75b458584c32402ad81a")},
                {3500000, uint256S("000000005c06cdac01ff6014d3e6104ab75947b83af5b136e2b1f30b4404a866")},
            }
        };

        chainTxData = ChainTxData{
            1563440362, // * UNIX timestamp of last checkpoint block
            3000000,    // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            2880        // * estimated number of transactions per day after checkpoint
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
        consensus.BIP34Height = FROMGENESIS;
        consensus.BIP34Hash = uint256S("000007c2d96e3435b752fc25a219ff70c963540d07c73243419b8e9274456f39");
        consensus.BIP65Height = 4000000; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 4000000; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.CSVHeight = 4000000; // 00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb
        consensus.SegwitHeight = 4000000; // 00000000002b980fcd729daaa248fd9316a5200e9b367f4ff2c42453e84201ca
        // consensus.MinBIP9WarningHeight = 836640; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 20
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

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
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 0.5;

        genesis = CreateGenesisBlock(1581969864, 713385, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("testnet: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("testnet: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("testnet: %s\n", consensus.powLimit.ToString().c_str());

        /*
        // Reset consensus to zero
        consensus.hashGenesisBlock = uint256S("");
        // calculate Genesis Block
        if (true && (genesis.GetHash() != consensus.hashGenesisBlock)) {
            printf("Calculating Genesis Block:\n");
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
                    printf("NONCE WRAPPED, incrementing time");
                    ++genesis.nTime;
                }
                // if (genesis.nNonce % 10000 == 0)
                // {
                //     printf("nonce %08u: hash = %s \n", genesis.nNonce, genesis.GetHash().ToString().c_str());
                // }
            }
            std::cout << "Testnet ---\n";
            std::cout << "  nonce: " << genesis.nNonce <<  "\n";
            std::cout << "   time: " << genesis.nTime << "\n";
            std::cout << "   hash: " << genesis.GetHash().ToString().c_str() << "\n";
            std::cout << "   merklehash: "  << genesis.hashMerkleRoot.ToString().c_str() << "\n";
            // Testnet --- nonce: {{nnonce}} time: {{ntime}} hash: {{genesisblockhash}}
        }
        */
        assert(consensus.hashGenesisBlock == uint256S("000007c2d96e3435b752fc25a219ff70c963540d07c73243419b8e9274456f39"));
        assert(genesis.hashMerkleRoot == uint256S("1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top

        vSeeds.emplace_back("minkiz.co", false);

        // base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127); // 0x7f
        // base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);  // 0x1e
        // base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,255); // 0xff
        // base58Prefixes[EXT_PUBLIC_KEY] = {0x87, 0xc9, 0x27, 0x00}; // 'Vpub'
        // base58Prefixes[EXT_SECRET_KEY] = {0x87, 0xc8, 0x30, 0x80}; // 'Vpriv'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,130);                    // VCoin addresses start with 'V'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);                    // VCoin script addresses start with '7'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,111 + 128);              // VCoin private keys start with '7' or 'V'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 pubkeys start with 'drkv'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 prvkeys start with 'drkp'

        bech32_hrp = "tv";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        m_is_test_chain = true;


        checkpointData = {
            {
                {0, uint256S("000007e14c52364cee2d4d9483541d473e3e73c896df75882273b91313b44816")},
            }
        };

        chainTxData = ChainTxData{
            1374901773,
            0,
            2880
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = FROMGENESIS; // BIP34 activated on regtest (Used in functional tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = FROMGENESIS; // BIP65 activated on regtest (Used in functional tests)
        consensus.BIP66Height = FROMGENESIS; // BIP66 activated on regtest (Used in functional tests)
        consensus.CSVHeight = FROMGENESIS; // CSV activated on regtest (Used in rpc activation tests)
        consensus.SegwitHeight = FROMGENESIS; // SEGWIT is always activated on regtest unless overridden
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1200; // 20 minutes
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

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
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1296688602, 3, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        LogPrintf("regtest: %s\n", consensus.hashGenesisBlock.ToString().c_str());
        LogPrintf("regtest: %s\n", genesis.hashMerkleRoot.ToString().c_str());
        LogPrintf("regtest: %x\n", consensus.powLimit.ToString().c_str());

        /*
        // Reset consensus to zero
        consensus.hashGenesisBlock = uint256S("0x");
        // calculate Genesis Block
        if (true && (genesis.GetHash() != consensus.hashGenesisBlock)) {
            printf("Calculating Genesis Block:\n");
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
                    printf("NONCE WRAPPED, incrementing time");
                    ++genesis.nTime;
                }
                // if (genesis.nNonce % 10000 == 0)
                // {
                //     printf("nonce %08u: hash = %s \n", genesis.nNonce, genesis.GetHash().ToString().c_str());
                // }
            }
            std::cout << "Regtestnet ---\n";
            std::cout << "  nonce: " << genesis.nNonce <<  "\n";
            std::cout << "   time: " << genesis.nTime << "\n";
            std::cout << "   hash: " << genesis.GetHash().ToString().c_str() << "\n";
            std::cout << "   merklehash: "  << genesis.hashMerkleRoot.ToString().c_str() << "\n";
            // Regtestnet --- nonce: {{regtestnnonce}} time: {{regtestntime}} hash: {{regtestgenesisblockhash}}
        }
        */

        assert(consensus.hashGenesisBlock == uint256S("ffc694d084bd98d8b0708c8a5fba877f498476439c7ab31f0cf3f5c38c026a64"));
        assert(genesis.hashMerkleRoot == uint256S("1576ef41775095b26a8f8f2bb65b693ec12230608a425aa84ee462381cae00e6"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.
        vSeeds.emplace_back("minkiz.co", false);

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        m_is_test_chain = true;

        // base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        // base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // base58Prefixes[EXT_PUBLIC_KEY] = {0x87, 0xc9, 0x27, 0x00}; // 'tpub'
        // base58Prefixes[EXT_SECRET_KEY] = {0x87, 0xc8, 0x30, 0x80}; // 'tpriv'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);                    // VCoin addresses start with 'V'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);                    // VCoin script addresses start with '7'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);              // VCoin private keys start with '7' or 'V'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 pubkeys start with 'drkv'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >(); // Chaincoin BIP32 prvkeys start with 'drkp'

        bech32_hrp = "vcrt";

        checkpointData = {
            {
                {0, uint256S("ffc694d084bd98d8b0708c8a5fba877f498476439c7ab31f0cf3f5c38c026a64")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (gArgs.IsArgSet("-segwitheight")) {
        int64_t height = gArgs.GetArg("-segwitheight", consensus.SegwitHeight);
        if (height < -1 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use -1 to disable segwit.", height));
        } else if (height == -1) {
            LogPrintf("Segwit disabled for testing\n");
            height = std::numeric_limits<int>::max();
        }
        consensus.SegwitHeight = static_cast<int>(height);
    }

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() != 3) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
        }
        int64_t nStartTime, nTimeout;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}
