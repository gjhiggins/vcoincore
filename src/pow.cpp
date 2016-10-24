// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

/* HOW-TO: adjust difficulty
static const int BITCOIN_DIFFICULTY_HISTORY_BLOCKS = 400;
static const int64_t nTotalWeights = 654916;
static const int64_t aWeights[BITCOIN_DIFFICULTY_HISTORY_BLOCKS] = {
    9850, 9702, 9556, 9413, 9272, 9133, 8996, 8861, 8728, 8597,
    8468, 8341, 8216, 8092, 7971, 7851, 7734, 7618, 7503, 7391,
    7280, 7171, 7063, 6957, 6853, 6750, 6649, 6549, 6451, 6354,
    6259, 6165, 6072, 5981, 5892, 5803, 5716, 5630, 5546, 5463,
    5381, 5300, 5221, 5142, 5065, 4989, 4914, 4841, 4768, 4696,
    4626, 4557, 4488, 4421, 4355, 4289, 4225, 4161, 4099, 4038,
    3977, 3917, 3859, 3801, 3744, 3688, 3632, 3578, 3524, 3471,
    3419, 3368, 3317, 3267, 3218, 3170, 3123, 3076, 3030, 2984,
    2939, 2895, 2852, 2809, 2767, 2725, 2685, 2644, 2605, 2566,
    2527, 2489, 2452, 2415, 2379, 2343, 2308, 2273, 2239, 2206,
    2172, 2140, 2108, 2076, 2045, 2014, 1984, 1954, 1925, 1896,
    1868, 1840, 1812, 1785, 1758, 1732, 1706, 1680, 1655, 1630,
    1606, 1582, 1558, 1534, 1511, 1489, 1466, 1444, 1423, 1401,
    1380, 1360, 1339, 1319, 1299, 1280, 1261, 1242, 1223, 1205,
    1187, 1169, 1151, 1134, 1117, 1100, 1084, 1067, 1051, 1036,
    1020, 1005,  990,  975,  960,  946,  932,  918,  904,  890,
     877,  864,  851,  838,  825,  813,  801,  789,  777,  765,
     754,  743,  731,  720,  710,  699,  688,  678,  668,  658,
     648,  638,  629,  619,  610,  601,  592,  583,  574,  566,
     557,  549,  540,  532,  524,  517,  509,  501,  494,  486,
     479,  472,  465,  458,  451,  444,  437,  431,  424,  418,
     412,  405,  399,  393,  387,  382,  376,  370,  365,  359,
     354,  349,  343,  338,  333,  328,  323,  318,  313,  309,
     304,  300,  295,  291,  286,  282,  278,  274,  269,  265,
     261,  257,  254,  250,  246,  242,  239,  235,  232,  228,
     225,  221,  218,  215,  211,  208,  205,  202,  199,  196,
     193,  190,  187,  184,  182,  179,  176,  174,  171,  168,
     166,  163,  161,  159,  156,  154,  151,  149,  147,  145,
     143,  140,  138,  136,  134,  132,  130,  128,  126,  124,
     123,  121,  119,  117,  115,  114,  112,  110,  109,  107,
     105,  104,  102,  101,   99,   98,   96,   95,   93,   92,
      90,   89,   88,   86,   85,   84,   83,   81,   80,   79,
      78,   76,   75,   74,   73,   72,   71,   70,   69,   68,
      67,   66,   65,   64,   63,   62,   61,   60,   59,   58,
      57,   56,   56,   55,   54,   53,   52,   51,   51,   50,
      49,   48,   48,   47,   46,   46,   45,   44,   44,   43,
      42,   42,   41,   40,   40,   39,   39,   38,   37,   37,
      36,   36,   35,   35,   34,   34,   33,   33,   32,   32,
      31,   31,   30,   30,   29,   29,   28,   28,   27,   27,
      27,   26,   26,   25,   25,   25,   24,   24,   24,   23
};

// Bitcoin:: This function is derived from the Python code written for the BasicCoin project.
// See: https://github.com/zack-bitcoin/basiccoin/blob/master/target.py
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params &params)
{
    const int LOOKUP_STOP_HEIGHT = 2; // Completely avoid the genesis block's interval (for testing, and why not).
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnOldTarget;
    if (pindexLast->nHeight < LOOKUP_STOP_HEIGHT)
	bnOldTarget = bnPowLimit; // Ignore the genesis block and etc. (for testing, and why not).
    else
	bnOldTarget.SetCompact(pindexLast->nBits);

    // ----------------------------------------------------------------------------
    // Compute a weighted time average factor and a weighted past difficulty.
    // ----------------------------------------------------------------------------

    int64_t nTimeFactor = 0;
    arith_uint256 bnNewTarget = 0;

    const CBlockIndex *pindexIter = pindexLast;
    for (int i=0; i<BITCOIN_DIFFICULTY_HISTORY_BLOCKS; ++i) {
	
	const CBlockIndex *pindexIterPrev = pindexIter->pprev;
	
	arith_uint256 bnBlockTarget;
	int64_t nBlockTimeInterval;
	
	if (pindexIter->nHeight < LOOKUP_STOP_HEIGHT) {
	    bnBlockTarget = bnPowLimit;
	    nBlockTimeInterval = params.nPowTargetSpacing;
	} else {
	    bnBlockTarget.SetCompact(pindexIter->nBits);
	    nBlockTimeInterval = std::max(pindexIter->GetBlockTime() - pindexIterPrev->GetBlockTime(), 0L);
	    nBlockTimeInterval = std::min(nBlockTimeInterval, params.nPowTargetSpacing * 100);
	}
	
	// Make room at the high bits to avoid overflows due to the deferred integer divisions.
	bnBlockTarget >>= 32;
	bnBlockTarget *= aWeights[i];
	
	//LogPrintf("bnBlockTarget[-%i]:  %s\n", i, bnBlockTarget.ToString());
	
	bnNewTarget += bnBlockTarget;
	
	//LogPrintf("       bnNewTarget:  %s\n", bnNewTarget.ToString());
	
	nBlockTimeInterval *= aWeights[i];
	nTimeFactor += nBlockTimeInterval;
	
	if (pindexIter->nHeight >= LOOKUP_STOP_HEIGHT)
	    pindexIter = pindexIterPrev;
    }

    //LogPrintf("GetNextWorkRequired: last height = %d, last GetBlockTime() = %ld\n", pindexLast->nHeight, pindexLast->GetBlockTime());

    //LogPrintf("  nTimeFactor = %d (* nTotalWeights * nPowTargetSpacing)\n", nTimeFactor);

    nTimeFactor /= params.nPowTargetSpacing;

    //double dTimeFactor = double(nTimeFactor) / double(nTotalWeights);
    //LogPrintf("  nTimeFactor = %f\n", dTimeFactor);

    // ----------------------------------------------------------------------------
    // Compute the new target, which is essentially the historical weighted
    //   target multiplied by the time factor, plus a clipping step so the
    //   difficulty doesn't ever rise or drop too fast from block to block.
    // ----------------------------------------------------------------------------

    bnNewTarget *= nTimeFactor;

    bnNewTarget /= nTotalWeights; // nTimeFactor deferred integer division
    bnNewTarget /= nTotalWeights; // bnNewTarget deferred integer division
    bnNewTarget <<= 32; // compensate for the least significant bits we nuked before
    
    //LogPrintf("GetNextWorkRequired RETARGET\n");
    //LogPrintf("Before:  %08x  %s\n", pindexLast->nBits, bnOldTarget.ToString());
    //LogPrintf("After:   %08x  %s\n", bnNewTarget.GetCompact(), bnNewTarget.ToString());

    // Since Bitcoin is a "1:1 block reward" game, blocks having too low of a
    //   difficulty (because they e.g. didn't rise in difficulty 'fast enough' to
    //   more quickly track the present hash power) will grant zero in-game
    //   advantage. Easy blocks pay little, hard blocks pay a lot. Thus our only
    //   concerns are related to the health of the network:
    //   (a) avoiding DoS/vandalism from the quick spamming of easy blocks filled
    //       with garbage, and
    //   (b) getting the difficulty stuck in a high setting for too long due to
    //       massive drops in hashpower.
    // Our mechanism to mitigate (b) is clipping at nUpperBound (4%) closely and
    //   clipping more loosely at nLowerBound (20%).
    // (a) is not a long problem in the long term, since Bitcoin is intended to
    //   have full nodes running in pruned mode.

    arith_uint256 bnLowerBound = (100 - params.nMaxAdjustUp) * bnOldTarget / 100;
    arith_uint256 bnUpperBound = (100 + params.nMaxAdjustDown) * bnOldTarget / 100;

    if (bnNewTarget > bnUpperBound)
	bnNewTarget = bnUpperBound;
    else if (bnNewTarget < bnLowerBound)
	bnNewTarget = bnLowerBound;

    if (bnNewTarget > bnPowLimit)
	bnNewTarget = bnPowLimit;

    //LogPrintf("Clipped: %08x  %s\n\n", bnNewTarget.GetCompact(), bnNewTarget.ToString());
    
    return bnNewTarget.GetCompact();
}
*/

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    LogPrintf("GNWR: nProofOfWorkLimit: %8u, pindexLast.nBits %8u\n", nProofOfWorkLimit, pindexLast->nBits);

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{

    LogPrintf("CNWR1 block nBits: %d, params.powLimit: %s\n", pindexLast->nBits, params.powLimit.ToString().c_str());

    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;

    LogPrintf("nActualTimespan = %u before bounds\n", nActualTimespan);

    if (params.fPowAllowMinDifficultyBlocks)
    {
        // Testnet version
        if (nActualTimespan < params.nPowTargetTimespan/4)
            nActualTimespan = params.nPowTargetTimespan/4;
        if (nActualTimespan > params.nPowTargetTimespan*4)
            nActualTimespan = params.nPowTargetTimespan*4;
    }else{
        // Mainnet version
    if (nActualTimespan < params.nPowTargetTimespan)
        nActualTimespan = params.nPowTargetTimespan;
    if (nActualTimespan > params.nPowTargetTimespan)
        nActualTimespan = params.nPowTargetTimespan;
    }

    LogPrintf("nActualTimespan = %u after bounds\n", nActualTimespan);

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    // LogPrintf("CNWR2 bnNew: %s, bnPowLimit: %s\n", bnNew.ToString().c_str(), bnPowLimit.ToString().c_str());
    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;
    LogPrintf("CNWR2 bnNew: %s, bnPowLimit: %s\n", bnNew.ToString().c_str(), bnPowLimit.ToString().c_str());

    return bnNew.GetCompact();
}


bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
