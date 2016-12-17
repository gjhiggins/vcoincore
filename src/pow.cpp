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

static const int64_t nTargetTimespan = 1200; // 20 minutes
static const int64_t nTargetSpacing = 30; // 30 seconds
static const int64_t nInterval = nTargetTimespan / nTargetSpacing; // 40 blocks
static const int64_t nAveragingInterval = nInterval * 20; // 40 blocks
static const int64_t nAveragingTargetTimespan = nAveragingInterval * nTargetSpacing; // 40 minutes

static const int64_t nMaxAdjustDown = 28; // 28% adjustment down
static const int64_t nMaxAdjustUp = 18; // 18% adjustment up

static const int64_t nTargetTimespanAdjDown = nTargetTimespan * (100 + nMaxAdjustDown) / 100;

static const int64_t nMinActualTimespan = nAveragingTargetTimespan * (100 - nMaxAdjustUp) / 100;
static const int64_t nMaxActualTimespan = nAveragingTargetTimespan * (100 + nMaxAdjustDown) / 100;

/*
unsigned int A_GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
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
*/

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    // unsigned int nProofOfWorkLimit = params.ProofOfWorkLimit().GetCompact();
    // arith_uint256 nProofOfWorkLimit = UintToArith256(params.powLimit);
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

    // Genesis block
    if (pindexLast == NULL)
        return bnPowLimit.GetCompact();
        
    if (pindexLast->nHeight+1 < nAveragingInterval) 
        return bnPowLimit.GetCompact();

    // Only change once per interval
    if ((pindexLast->nHeight+1) % nInterval != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->nTime > pindexLast->nTime + nTargetSpacing*2)
                return bnPowLimit.GetCompact();
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == bnPowLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be nAveragingInterval blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < nAveragingInterval-1; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    LogPrintf("  nActualTimespan = %u before bounds\n", nActualTimespan);
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
    // Retarget
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("nTargetTimespan = %u nActualTimespan = %u\n", nAveragingTargetTimespan, nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, arith_uint256().SetCompact(pindexLast->nBits).ToString().c_str());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString().c_str());

    return bnNew.GetCompact();
}

/*
unsigned int NewCalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
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
*/

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

/*
// minimum amount of work that could possibly be required nTime after
// minimum work required was nBase
//
unsigned int ComputeMinWork(unsigned int nBase, int64_t nTime)
{
    const CBigNum &bnLimit = Params().ProofOfWorkLimit();
    // Testnet has min-difficulty blocks
    // after nTargetSpacing*2 time between blocks:
    if (TestNet() && nTime > nTargetSpacing*2)
        return bnLimit.GetCompact();

    CBigNum bnResult;
    bnResult.SetCompact(nBase);
    while (nTime > 0 && bnResult < bnLimit)
    {
        // Maximum adjustment...
        bnResult *= (100 + nMaxAdjustDown);
        bnResult /= 100;
        // ... in best-case exactly adjustment times-normal target time
        nTime -= nTargetTimespanAdjDown;
    }
    if (bnResult > bnLimit)
        bnResult = bnLimit;
    return bnResult.GetCompact();
}
*/
