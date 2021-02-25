// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/test/uritests.h>

#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("v-core://VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?req-message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("v-core:VJQPfPNoz93Nhe5pn78kSomW35ewtik6iW?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}
