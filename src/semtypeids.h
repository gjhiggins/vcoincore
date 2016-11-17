// semantic type identifiers and key

#ifndef SEM_TYPE_IDS_H
#define SEM_TYPE_IDS_H

enum SEM_TYPE {
        // VCN-0: NONE; non-value
        VCN_NONE,
        // VCN-1: ASSERT; assertion of resource
        VCN_ASSERT,
        // VCN-2: BANS:BCT; decentralised blacklist
        VCN_BANS_BCT,
        // VCN-3: VOTE; voting
        VCN_VOTE
};

#endif
