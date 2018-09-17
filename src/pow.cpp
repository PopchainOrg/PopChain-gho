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

    if (UintToArith256(pindex->GetBlockHash()) == UintToArith256(params.hashGenesisBlock))
        return params.minimumDifficulty;

    uint64_t difficulty;
    int32_t const timestampDiff = pblock->nTime - pindex->nTime;

    if (pindex->nHeight < params.nYolandaTime){
        if (timestampDiff < 45) difficulty = pindex->nDifficulty + pindex->nDifficulty / params.difficultyRapidFitDivisor;
        else difficulty = pindex->nDifficulty - pindex->nDifficulty / params.difficultyRapidFitDivisor;
    } else {
        int64_t const adjFactor = std::max((pindex->hasUncles() ? 2 : 1) - timestampDiff / 29, -99);
        difficulty = pindex->nDifficulty + pindex->nDifficulty / params.difficultyBoundDivisor * adjFactor;
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



/*popchain ghost*/
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative = false;
    bool fOverflow = false;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

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
    if (block.nDifficulty > 0)
        return block.nDifficulty;
    return 0;
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
