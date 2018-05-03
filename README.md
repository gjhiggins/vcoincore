V Core integration/staging tree
===============================

[![Build Status](https://travis-ci.org/vcore/vcore.svg?branch=master)](https://travis-ci.org/vcore/vcore)

https://vcore.org

What is V Core?
----------------

V Core is an experimental digital currency that enables instant payments to
anyone, anywhere in the world. V uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. V Core is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the V Core software, see https://vcore.org/en/download, or read the
[original Bitcoin whitepaper](https://bitcoincore.org/bitcoin.pdf).

License
-------

V Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/vcore/vcore/tags) are created
regularly to indicate new official, stable release versions of V Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

The developer [mailing list](https://lists.linuxfoundation.org/mailman/listinfo/vcore-dev)
should be used to discuss complicated or controversial changes before working
on a patch set.

Developer IRC can be found on Freenode at #vcore-dev.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

Changes to translations as well as new translations can be submitted to
[V Core's Transifex page](https://www.transifex.com/projects/p/vcore/).

Translations are periodically pulled from Transifex and merged into the git repository. See the
[translation process](doc/translation_process.md) for details on how this works.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

Translators should also subscribe to the [mailing list](https://groups.google.com/forum/#!forum/bitcoin-translators).

---

The infohash is a SHA1 schecksum used to verify the the file(s) are the correct ones and have not been altered since the hash was created.

Torrents have an infohash that represents the torrent as a whole with all its files (it's actually a hash of the "info" section of the original torrent file) and is calculated using SHA1 encryption. There is also a "hashtable" that has an SHA1 hash for each piece of the file and the pieces are verified as the finish downloading, and are discarded if they fail to match the hash, indicating something wrong was transmitted to you.

You said you already use magnet links -- well - then you already download torrents using their infohash, since that is how Magnet URNs are used with Bittorrent.

A Bittorrent magnet URN, at minimum, looks like this:

    magnet:?xt=urn:btih:[infohash here, no brackets]

Here is a magnet link as advertised for the current release of Arch Linux:

    magnet:?xt=urn:btih:e940a7a57294e4c98f62514b32611e38181b6cae&dn=archlinux-2013.01.04-dual.iso&tr=udp://tracker.archlinux.org:6969&tr=http://tracker.archlinux.org:6969/announce

The above link works with uTorrent, but it is not an example of a trackerless magnet link (one of the main unique features of using Magnet links to begin with).

It can be shortened to:

    magnet:?xt=urn:btih:e940a7a57294e4c98f62514b32611e38181b6cae

in order to "go trackerless". Notice how the only info uTorrent is provided is the infohash.

THIS is how you download using only an infohash. You type a link like this, with the deseired hash, either into a browser window and submit it or in uTorrent you can use File>Add torrent from URL (or the equivalent Ctrl-U) and submit it there.

---

https://bitcoinism.blogspot.co.uk/2013/07/reclaiming-financial-privacy-with-hd.html

There's been a lot of talk recently about the accounting benefits and backup security of BIP32 Hierarchical Deterministic Wallets. One of the points that is not getting enough attention is the potential for extended public keys to greatly improve financial privacy for end users and business users.

In order for Bitcoin to function pseudonymously it's essential for addresses to be used in such a way that they are difficult to correlate based on the transactions in the blockchain. From the very beginning, developers have recommended as a best practice that addresses should be treated as single-use payment codes and never be reused. This solves half the problem, and it's the part that users have direct control over using existing clients. The other condition needed to stop addresses from being correlated is to never include outputs from different addresses as inputs to the same transaction. This is something that clients that care about privacy generally try to accomplish, but it's not always possible to do this, for example the case where no collection of inputs from the same address is large enough for the desired payment.

If my wallet consists of a 2 BTC unspent output at address A and a 3 BTC unspent output at address B and I desire to make a 4 BTC payment to address C, the only way to accomplish this in a single transaction is to use the outputs at A and B as inputs, with a 4 BTC output going to address C and a 1 BTC output going to change address D. Unfortunately this correlates A, B, and D in such a way that someone observing the blockchain can conclude with a high degree of certainty that the same individual controls all three addresses. This represents a permanent loss of financial privacy.

The only way to retain privacy in this situation is to ask the recipient to give me two destination addresses. Then I can make two transactions and broadcast them into the network a few seconds apart. This leaks far less information about which addresses I control into the blockchain and allows me to retain a higher degree of privacy.

The problem with that approach is that it's more difficult from an accounting perspective and an ease of payment perspective. Without HD wallets, the recipient finds it difficult to correlate multiple addresses with a single transaction, and it's more difficult to transmit the payment addresses to the sender. The recipient must either pregenerate multiple payment addresses just in case and send them all, or else they need some kind of negotiation step where the sender tells the recipient how many addresses they need.

HD wallets and extended public keys can entirely solve these problems, if we simply as a best practice that recipients of bitcoin transactions give the sender a unique extended public key for each transaction instead of giving them a (single) unique address.

HD-aware wallet software can use the extended public key as the unique index for associating blockchain transactions with, for example, an web site order in the same way they normally use a single bitcoin address, but by giving the user the capability to generate as many public keys as they want now the user can choose to protect their privacy by sending their payment in multiple transactions without requiring any explicit coordination between the sender and the recipient.

Another advantage of allowing the sender to generate an arbitrary number of public keys is that services which hold bitcoins for their users, such as Mt Gox, could lock an account's withdrawal addresses to the tree created by an extended public key, and still send the bitcoins to a unique address for each withdrawal. There's nothing stopping services like Mt Gox from implementing locked withdrawal addresses now, but doing so without HD wallets means that the users must sacrifice financial privacy in order to gain the security of knowing that a hacker who compromises their account can't withdraw their bitcoins to an arbitrary address. With HD wallets no compromise is necessary.
