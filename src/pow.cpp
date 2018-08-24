// Copyright (c) 2017-2018 The Popchain Core Developers

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

#include <math.h>
#include <iostream>
#include <algorithm>

// ghost new difficulty algorithm
// pindex chainactivate tip, pblock ready to assembly
uint64_t calculateDifficulty(const CBlockIndex* pindex, const CBlockHeader *pblock, const Consensus::Params& params)
{
    // Genesis block get minimum difficulty
    if (pindex == NULL)
        return params.minimumDifficulty;

//    const CBlockIndex* pindexParent = pindex->pprev;
//    if (pindexParent == NULL)
//        return params.minimumDifficulty;
  
    if (UintToArith256(pindex->GetBlockHash()) == UintToArith256(params.hashGenesisBlock))
        return params.minimumDifficulty;

    uint64_t difficulty;
    int32_t const timestampDiff = pblock->nTime - pindex->nTime;

    if (pindex->nHeight < params.nYolandaTime){
        if (timestampDiff < 30) difficulty = pindex->nDifficulty + pindex->nDifficulty / params.difficultyRapidFitDivisor;
        else difficulty = pindex->nDifficulty - pindex->nDifficulty / params.difficultyRapidFitDivisor;
        //std::cout<<"AStep"<<" height "<<pindex->nHeight<<" nTime: "<<pindex->nTime<<" timestampDiff: "<<timestampDiff<<" difficulty: "<<difficulty<<std::endl;
    } else {
        int64_t const adjFactor = std::max((pindex->hasUncles() ? 2 : 1) - timestampDiff / 20, -99);
        difficulty = pindex->nDifficulty + pindex->nDifficulty / params.difficultyBoundDivisor * adjFactor;
        //std::cout<<"BStep"<<" height "<<pindex->nHeight<<" nTime: "<<pindex->nTime<<" timestampDiff: "<<timestampDiff<<" adjFactor: "<<adjFactor<<" difficulty: "<<difficulty<<std::endl;
    }

    assert(difficulty > 0);
    if (params.minimumDifficulty > difficulty)
        difficulty = params.minimumDifficulty;
    assert (difficulty < std::numeric_limits<uint64_t>::max());
    return difficulty;
}

arith_uint256 getHashTraget (uint64_t difficulty)
{
   arith_uint256 hashTarget = UintToArith256(maxUint256)/arith_uint256(difficulty);
   return hashTarget;
}

uint32_t getNBits(arith_uint256 hashTarget)
{
    return hashTarget.GetCompact();
}

// GetNextWorkRequired called getNBits
unsigned int GetNextWorkRequired(const CBlockIndex* pindex, const CBlockHeader *pblock, const Consensus::Params& params)
{
    uint32_t nBits = getNBits(getHashTraget(calculateDifficulty(pindex, pblock, params)));
    return nBits;
}


/*
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Find the first block in the averaging interval
    const CBlockIndex* pindexFirst = pindexLast;
    arith_uint256 bnTot {0};
    for (int i = 0; pindexFirst && i < params.nPowAveragingWindow; i++) {   // 17
        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindexFirst->nBits);
        bnTot += bnTmp;
        pindexFirst = pindexFirst->pprev;
    }

    // Check we have enough blocks
    if (pindexFirst == NULL)
        return nProofOfWorkLimit;

    arith_uint256 bnAvg {bnTot / params.nPowAveragingWindow};

    return CalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast(), params);
}

unsigned int CalculateNextWorkRequired(arith_uint256 bnAvg,
                                       int64_t nLastBlockTime, int64_t nFirstBlockTime,
                                       const Consensus::Params& params)
{
    // Limit adjustment step
    // Use medians to prevent time-warp attacks
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    LogPrint("pow", "  nActualTimespan = %d  before dampening\n", nActualTimespan);
    nActualTimespan = params.AveragingWindowTimespan() + (nActualTimespan - params.AveragingWindowTimespan())/4;
    LogPrint("pow", "  nActualTimespan = %d  before bounds\n", nActualTimespan);

    if (nActualTimespan < params.MinActualTimespan())
        nActualTimespan = params.MinActualTimespan();
    if (nActualTimespan > params.MaxActualTimespan())
        nActualTimespan = params.MaxActualTimespan();

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew {bnAvg};
    bnNew /= params.AveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrint("pow", "GetNextWorkRequired RETARGET\n");
    LogPrint("pow", "params.AveragingWindowTimespan() = %d    nActualTimespan = %d\n", params.AveragingWindowTimespan(), nActualTimespan);
    LogPrint("pow", "Current average: %08x  %s\n", bnAvg.GetCompact(), bnAvg.ToString());
    LogPrint("pow", "After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}
*/


/*popchain ghost*/
//bool CheckProofOfWork(uint256 hash, uint256 nDifficulty, const Consensus::Params& params)
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative = false;
    bool fOverflow = false;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);
    //bnTarget = maxUint256Div(nDifficulty);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return error("CheckProofOfWork(): nBits below minimum work");

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
    {
        return error("CheckProofOfWork(): hash doesn't match nBits");
    }
    return true;
}

/*popchain ghost*/

/*popchain ghost*/
arith_uint256 GetBlockDifficulty(const CBlockIndex& block)
{
//    arith_uint256 bnTarget;
//    bool fNegative;
//    bool fOverflow;
	
//    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
//    if (fNegative || fOverflow || bnTarget == 0)
//        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    //return (~bnTarget / (bnTarget + 1)) + 1;

    if (block.nDifficulty > 0)
        return block.nDifficulty;
    return 0;

    /*popchain ghost*/
    //bnTarget = maxUint256Div(block.nDifficulty);
	//return bnTarget;
	/*popchain ghost*/
}

/*popchain ghost*/

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockDifficulty(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * r.GetLow64();
}
