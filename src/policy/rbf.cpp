// Copyright (c) 2016 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "policy/rbf.h"

bool SignalsOptInRBF(const CTransaction &tx)
{
    BOOST_FOREACH(const CTxIn &txin, tx.vin) {
        if (txin.nSequence < std::numeric_limits<unsigned int>::max()-1) {
            return true;
        }
    }
    return false;
}

<<<<<<< HEAD
bool IsRBFOptIn(const CTxMemPoolEntry &entry, CTxMemPool &pool)
=======
RBFTransactionState IsRBFOptIn(const CTransaction &tx, CTxMemPool &pool)
>>>>>>> official/0.13
{
    AssertLockHeld(pool.cs);

    CTxMemPool::setEntries setAncestors;

    // First check the transaction itself.
<<<<<<< HEAD
    if (SignalsOptInRBF(entry.GetTx())) {
        return true;
=======
    if (SignalsOptInRBF(tx)) {
        return RBF_TRANSACTIONSTATE_REPLACEABLE_BIP125;
>>>>>>> official/0.13
    }

    // If this transaction is not in our mempool, then we can't be sure
    // we will know about all its inputs.
<<<<<<< HEAD
    if (!pool.exists(entry.GetTx().GetHash())) {
        throw std::runtime_error("Cannot determine RBF opt-in signal for non-mempool transaction\n");
=======
    if (!pool.exists(tx.GetHash())) {
        return RBF_TRANSACTIONSTATE_UNKNOWN;
>>>>>>> official/0.13
    }

    // If all the inputs have nSequence >= maxint-1, it still might be
    // signaled for RBF if any unconfirmed parents have signaled.
    uint64_t noLimit = std::numeric_limits<uint64_t>::max();
    std::string dummy;
<<<<<<< HEAD
=======
    CTxMemPoolEntry entry = *pool.mapTx.find(tx.GetHash());
>>>>>>> official/0.13
    pool.CalculateMemPoolAncestors(entry, setAncestors, noLimit, noLimit, noLimit, noLimit, dummy, false);

    BOOST_FOREACH(CTxMemPool::txiter it, setAncestors) {
        if (SignalsOptInRBF(it->GetTx())) {
<<<<<<< HEAD
            return true;
        }
    }
    return false;
=======
            return RBF_TRANSACTIONSTATE_REPLACEABLE_BIP125;
        }
    }
    return RBF_TRANSACTIONSTATE_FINAL;
>>>>>>> official/0.13
}
