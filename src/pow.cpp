// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2021 The V Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <chainparams.h>
#include <chainparamsbase.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util.h>

bool TestNet()
{
    if (ChainNameFromCommandLine() == CBaseChainParams::TESTNET)
        return true;
    return false;
}

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
    
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;
        
    if (pindexLast->nHeight+1 < nAveragingInterval) 
        return nProofOfWorkLimit;

    // Only change once per interval
    if ((pindexLast->nHeight+1) % nInterval != 0)
    {
        if (TestNet())
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->nTime > pindexLast->nTime + nTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
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
    if (nActualTimespan < nMinActualTimespan)
        nActualTimespan = nMinActualTimespan;
    if (nActualTimespan > nMaxActualTimespan)
        nActualTimespan = nMaxActualTimespan;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    // intermediate uint256 can overflow by 1 bit
    bool fShift = bnNew.bits() > 235;
    if (fShift)
        bnNew >>= 1;
    bnNew *= nActualTimespan;
    bnNew /= nAveragingTargetTimespan;
    if (fShift)
        bnNew <<= 1;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    // // debug print
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("nTargetTimespan = %u nActualTimespan = %u\n", nAveragingTargetTimespan, nActualTimespan);
    // LogPrintf("Before: %08x  %s\n", pindexLast->nBits, pindexLast->nBits.getdouble().ToString().c_str());
    // LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

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
    {
        // std::cout << "CheckProofOfWork() : nBits below minimum work: " << nBits;
        return false;
    }

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
    {
        // std::cout << "CheckProofOfWork() : hash doesn't match nBits: " << nBits << " UintToArith256(hash): " << UintToArith256(hash).GetCompact() << " > bnTarget: " << bnTarget.GetCompact() << std::endl;
        return false;
    }

    return true;
}
