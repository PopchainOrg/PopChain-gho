// Copyright (c) 2017-2018 The Popchain Core Developers

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include "uint256.h"
#include <map>
#include <string>
#include "amount.h"

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_CSV, // Deployment of BIP68, BIP112, and BIP113.
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;
};

/**
 * Popchain DevTeam
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;

    // miner reward
    typedef int64_t i64;
    i64 premine;		// premine
    i64 genesisReward;          // genesis reward
    i64 minerReward4;		// block reward to miners per block in the 1st 4 years
    i64 minerReward5;		// block reward to miners per block in the 2nd 4 years
	
    // founders reward
    i64 foundersReward;         // super block reward to founders in first 4 years


    // popnode colleteral
    i64 colleteral;
 
    int nSubsidyHalvingInterval;
    int endOfFoundersReward() const {
        return 4 * 12 * nSuperblockCycle + 1;
    }

    int nInstantSendKeepLock; // in blocks

    int nSuperblockStartBlock;
    int nSuperblockCycle; // in blocks
    int nPopnodeMinimumConfirmations;
    /** Used to check majorities for block version upgrade */
    int nMajorityEnforceBlockUpgrade;
    int nMajorityRejectBlockOutdated;
    int nMajorityWindow;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargetting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    /* popchain ghost */
    uint256 powLimit;
    uint32_t difficultyBoundDivisor;
    uint32_t difficultyRapidFitDivisor;
    //uint32_t durationLimit;
    uint64_t minimumDifficulty;
    /* popchain ghost */
    bool fPowAllowMinDifficultyBlocks;
    int64_t nPowAveragingWindow;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    uint32_t nYolandaTime;
    //int64_t nPowTargetTimespan;
    //int64_t nPowMaxAdjustDown;
    //int64_t nPowMaxAdjustUp;
    //int64_t AveragingWindowTimespan() const { return nPowAveragingWindow * nPowTargetSpacing; } // 17*2.5*60
    //int64_t MinActualTimespan() const { return (AveragingWindowTimespan() * (100 - nPowMaxAdjustUp  )) / 100; }
    //int64_t MaxActualTimespan() const { return (AveragingWindowTimespan() * (100 + nPowMaxAdjustDown)) / 100; }
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
