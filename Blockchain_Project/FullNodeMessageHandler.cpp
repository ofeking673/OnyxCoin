#include "FullNodeMessageHandler.h"
#include "iostream"
#include "MessageParser.h"


FullNodeMessageHandler::FullNodeMessageHandler(std::string keyPath, int port) :
    IMessageHandler(keyPath, port)
{
	_blockchain = Blockchain::getInstance();
	_utxoSet = UTXOSet::getInstance();
}

FullNodeMessageHandler::~FullNodeMessageHandler()
{
    // Cleanup if necessary
}

std::vector<MessageP2P> FullNodeMessageHandler::onPing(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::PING)
    {
        return {};
    }

    // Log the event
    std::cout << "[FullNodeMessageHandler] Received PING from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());
    
    // Typical behavior might include sending a PONG message back.
    // That would require access to a network interface or peer manager
    // (not shown here). For example:
    //
    //MessageP2P pongMsg(
    //     msg.getSignature(),          // Keep the same signature or your own
    //     peerManager.getPubKey(),     // Author signature
    //     MessageType::PONG,           // Message type
    //     0,                           // No payload
    //     {}                           // Empty payload vector
    // );
    // Source address is stored in msg._author
    // network->send(pongMsg);
    std::vector<MessageP2P> messages;
    MessageP2P pongMsg = MessageManager::createPongMessage(_node.getMyPublicKey(), msg.getPayload()["time"]);
    //_peerManager.sendMessage(msg.getAuthor(), pongMsg.toJson());
    messages.push_back(pongMsg);
    return messages;

    /*
    Typical Logic
    Log that you received a PING.
    Respond with a PONG message (often containing minimal or no payload).
    Update your peer manager to indicate that the peer is active.
    Possible Steps

    Print/Log the event.
    Construct a MessageP2P object of type PONG.
    Send the PONG back to the origin peer (using your network layer).
    Update a “lastPingTime” or “lastSeenTime” in your peer manager.
    */
}

std::vector<MessageP2P> FullNodeMessageHandler::onPong(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::PONG)
    {
        return {};
    }

    // Log the event
    std::cout << "[FullNodeMessageHandler] Received PONG from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // No handling needed. Only updating the last contact time.
    return {};
}

std::vector<MessageP2P> FullNodeMessageHandler::onGetPeers(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::GET_PEERS)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetPeers from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic to handle a request for peer information

    /*
    Typical Logic
    Log that the peer requested peer information.
    Gather your known peers (often a list of IP/port pairs).
    Send a PeerList message back with some or all known peers.
    Possible Steps

    Print/Log the event.
    Query your peer manager for a list of known peers.
    Construct a MessageP2P of type PEER_LIST (could contain serialized IP addresses/ports).
    Send the PEER_LIST to the requester.

    */

    json serializedPeerList = _node.peersToJson();

    std::vector<MessageP2P> messages;
    // Send the peer list back to the peer requested it
    MessageP2P peerListMsg = MessageManager::createPeerListMessage(_node.getMyPublicKey(), serializedPeerList);
    messages.push_back(peerListMsg);
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onPeerList(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::PEER_LIST)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Peer List from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic to handle a list of peers

    /*
    Typical Logic
    Log that you received a list of peers.
    Parse the payload to extract peer addresses.
    Add new peers to your peer manager’s address list (deduplicate if needed).
    Possibly connect to a few new peers to expand your network.
    Possible Steps

    Print/Log the event.
    Parse the list of peers from msg.getPayload().
    For each peer, check if it’s already known, otherwise add it to your peer manager.
    Optionally initiate new outbound connections to some of these peers.
    */

    auto newPeersList = _node.fromJsonToPeers(msg.getPayload());

    auto newPeers = _node.getNewPeers(newPeersList);

    
    for (auto peer : newPeers)
    {
        _node.connectToNode(peer.second.ip, peer.second.port, peer.second.publicKey, peer.second.nodeId);
    }

    // No need to return messages to the sender
    return {};
}

std::vector<MessageP2P> FullNodeMessageHandler::onGetBlock(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::GET_BLOCK)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetBlock from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic to handle a request for a block

    /*
    Typical Logic
    Log that you received a request for a block.
    Determine which block is requested (by hash, height, etc.).
    Retrieve that block from your local Blockchain storage.
    Send back a BLOCK message containing the serialized block data.
    Possible Steps

    Print/Log the event.
    Extract the block identifier (hash or height) from the message payload.
    Look up the block in your local blockchain database.
    If found, create a MessageP2P of type BLOCK with the serialized block data as payload.
    Send the BLOCK message back.
    */

    auto blkHash = MessageParser::parseGetBlockMessage(msg);

    // Find the requested block
    Block block = _blockchain->findBlock(blkHash.first, blkHash.second);

    std::vector<MessageP2P> messages;
    // Send the Block back to the peer requested it
    MessageP2P blockMsg = MessageManager::createBlockMessage(_node.getMyPublicKey(), block);
    //_peerManager.sendMessage(msg.getAuthor(), blockMsg.toJson());
    messages.push_back(blockMsg);
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onBlock(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::BLOCK)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Block from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic to handle incoming block data

    /*
    Typical Logic
    Log that you received a block.
    Deserialize and validate the block (syntax check, proof-of-work, signatures, etc.).
    Add the block to your local blockchain if valid.
    Update the chain’s tip if this block extends the best chain.
    Relay the block to other peers (optional, depending on your node’s relay policy).
    Possible Steps

    Print/Log the event.
    Deserialize the block data from msg.getPayload().
    Validate the block (e.g., check proof-of-work, transactions).
    If valid, add it to your blockchain (handle chain reorganization if necessary).
    Broadcast or announce this new block to other peers via an inventory or block message.
    */

    Block block = Block::fromJson(msg.getPayload());
    if (block.isErrorBlock())
    {
        // Recieved error block
        // Might be because asked for worng block hash and previous hash,
        // or the node doesn't have the block searched for
        // TO-DO: Send something about it to the network.
    }

    // TO-DO: Verify the recieved block
    // When impplementing consensus mechanism
    // if(!block.verifyBlock())...


    // TO-DO: Add the block to the chain, if not already in it
    // When implementing consensus mechanism
    

    // TO-DO: Broadcast block or inventory.
    // Need to think about the implementation




    return {}; // REMOVE THIS
}

std::vector<MessageP2P> FullNodeMessageHandler::onNewTransaction(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::NEW_TRANSACTION)
    {
        return {};
    }

    // Log the event
    std::cout << "[FullNodeMessageHandler] Received New Transaction from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic for receiving broadcast of a new transaction

    /*
    Typical Logic
    Log that a new transaction was announced.
    Deserialize and validate the transaction.
    Store the transaction in your mempool if it’s valid.
    Optionally broadcast it to other peers so they are also aware.
    Possible Steps

    Print/Log the event.
    Deserialize the transaction from the payload.
    Validate the transaction (signatures, inputs, outputs, fees, etc.).
    If valid, store it in your mempool.
    Broadcast a “NewTransaction” or “Inventory” message to alert other peers.
    */

    Transaction tx = Transaction::fromJson(msg.getPayload());
    if (tx.isErrorTransaction())
    {
        // Recieved error transaction
        // Might be because asked for worng trnasaction ID,
        // or the node doesn't have the transaction searched for
        // TO-DO: Send something about it to the network.
    }
    if (!tx.verifyTransactionSignature())
    {
        // Send something that says that the transaction is invalid
    }

    // Add to pending transaction, if not already in it
    _blockchain->addTransaction(tx);


    // TO-DO: Broadcast new transaction or inventory.
    // Need to think about the implementation


    return {}; // REMOVE THIS
}

std::vector<MessageP2P> FullNodeMessageHandler::onGetTransaction(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::GET_TRANSACTION)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetTransaction from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic for handling transaction data

    /*
    Typical Logic
    Log that the peer wants a specific transaction.
    Parse the transaction hash or ID from the payload.
    Look for the transaction in your mempool or blockchain (if confirmed).
    Send the transaction data back as a TRANSACTION message if found.
    Possible Steps

    Print/Log the event.
    Extract transaction ID from msg.getPayload().
    Check if it’s in your mempool or if it’s already confirmed in the blockchain.
    If found, construct a MessageP2P with TRANSACTION type and the serialized transaction.
    Send the message back.
    */
    std::string txID = MessageParser::parseGetTransactionMessage(msg);
    Transaction tx = _blockchain->findTransactionInPending(txID);
    if (tx.isErrorTransaction())
    {
        // Transaction NOT found in pending transactions
        // Search trnasaction in whole chain
        tx = _blockchain->findTransactionInChain(txID);
    }

    // Send the transaction back to the peer requested it
    std::vector<MessageP2P> messages;
    MessageP2P newTxMsg = MessageManager::createNewTransactionMessage(_node.getMyPublicKey(), tx);
    //_peerManager.sendMessage(msg.getAuthor(), newTxMsg.toJson());
    messages.push_back(newTxMsg);
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onInventory(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::INVENTORY)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received INVENTORY ANNOUUNCEMENT from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic for receiving inventory announcements (blocks/tx)

    /*
    Typical Logic
    Log that you received an inventory announcement.
    Parse the inventory (list of block hashes, transaction IDs, etc.) in the payload.
    Determine which items you do not have locally.
    Send requests for those items back to the peer using GetBlock or GetTransaction messages.
    Possible Steps

    Print/Log the event.
    Extract the list of inventory items from msg.getPayload().
    For each item, check if you already have it (e.g., check blockchain or mempool).
    If missing, request the item from the peer (using onGetBlock, onGetTransaction, or a direct “GetData”-type message).
    */
    std::vector<MessageP2P> messages;

    InventoryData inventory = InventoryData::fromJson(msg.getPayload());
    std::vector<std::string> txIDs = inventory.getTxIDs();
    for (auto& txID : txIDs)
    {
        if (!_blockchain->hasTransaction(txID))
        {
            // We don't have the transaction. add it to the chain.
            // Create getTransaction message and send it to the peer that sent the inventory message
            MessageP2P getTxMsg = MessageManager::createGetTransactionMessage(_node.getMyPublicKey(), txID);
            //_peerManager.sendMessage(msg.getAuthor(), getTxMsg.toJson());
            messages.push_back(getTxMsg);
        }
    }

    // blockHash | prevBlockHash
    auto blocksHashes = inventory.getBlocksHash();

    for (auto& blockHashes : blocksHashes)
    {
        if (!_blockchain->hasBlock(blockHashes.first, blockHashes.second))
        {
            // We don't have the Block. add it to the chain.
            // Create getBlock message and send it to the peer that sent the inventory message
            MessageP2P getBlockMsg = MessageManager::createGetBlockMessage(_node.getMyPublicKey(), blockHashes.first, blockHashes.second);
            //_peerManager.sendMessage(msg.getAuthor(), getBlockMsg.toJson());
            messages.push_back(getBlockMsg);
        }
    }
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onGetHeaders(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::GET_HEADERS)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetHeaders from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic for a request for block headers

    /*
    Typical Logic
    Log that the peer is requesting block headers.
    Parse the request to find the block locator or certain heights.
    Collect the applicable headers from your blockchain.
    Send back a HEADERS message containing the requested headers.
    Possible Steps

    Print/Log the event.
    Extract the starting point or block locator from msg.getPayload().
    Gather headers from your best chain up to a certain limit (e.g., 2,000 headers at a time).
    Serialize those headers into a MessageP2P of type HEADERS.
    Send the HEADERS message back to the requesting peer.
    */

    std::vector<std::pair<std::string, std::string>> blockHashes;
    std::string stopHash;
    MessageParser::parseGetHeadersMessage(msg, blockHashes, stopHash);

    // Find fork point
    //    The locator is in order from newest to oldest. We find the first hash we know.
    int forkIndex = -1;
    for (const auto& hash : blockHashes)
    {
        int localIndex = _blockchain->getHeightByHash(hash.first);
        if (localIndex >= 0) 
        {
            forkIndex = localIndex;
            break;
        }
    }

    // If none of the locator hashes are known, start from genesis (index 0).
    if (forkIndex == -1) 
    {
        forkIndex = 0;
    }
    else 
    {
        // Start from the block after the fork point to get next headers
        forkIndex += 1;
    }

    // Gather up to MAX_HEADERS from forkIndex
    auto headersToSend = _blockchain->getHeadersFrom(forkIndex, MAX_HEADERS, stopHash);

    std::vector<MessageP2P> messages;
    // Send the transaction back to the peer requested it
    MessageP2P headersMsg = MessageManager::createHeadersMessage(_node.getMyPublicKey(), headersToSend);
    //_peerManager.sendMessage(msg.getAuthor(), headersMsg.toJson());
    messages.push_back(headersMsg);
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onHeaders(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::HEADERS)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Block Headers from peer." << std::endl;

    // Update last contact with peer
    _node.updatePeersLastContact(msg.getAuthor());

    // Stub: implement logic to handle incoming block headers

    /*
    Typical Logic
    Log that you received block headers.
    Deserialize the headers list.
    Validate them (proof-of-work, chain linkage, etc.).
    Update your local chain’s header index.
    Decide whether to request the corresponding blocks (to catch up on the chain).
    Possible Steps

    Print/Log the event.
    Deserialize header data from msg.getPayload().
    Validate each header in the chain of headers (check previous hash, difficulty).
    Store or update them in your local chain’s header index.
    If they extend your chain, request those blocks with GetBlock or another mechanism for full sync.
    */

    std::vector<BlockHeader> receivedHeaders = BlockHeader::jsonToVector(msg.getPayload());

    // Validate / append
    _blockchain->appendHeaders(receivedHeaders);

    std::vector<MessageP2P> messages;

    // Decide whether to request more headers or start requesting blocks
    //    If we got MAX_HEADERS in the response, there's likely more to fetch.
    if (receivedHeaders.size() == MAX_HEADERS) 
    {
        // Request more headers from the last known header
        // Build a new locator starting from the last header we got.
        std::string lastHash = receivedHeaders.back().getHash();
        std::string lastPrevHash = receivedHeaders.back().getPreviousHash();
        std::vector<std::pair<std::string, std::string>> blockLocatorHashes;
        std::pair<std::string, std::string> locatorHashes(lastHash, lastPrevHash);
        blockLocatorHashes.push_back(locatorHashes);


        MessageP2P getMoreHeaders = MessageManager::createGetHeadersMessage(_node.getMyPublicKey(), blockLocatorHashes, "");

        // Requesting more headers from last hash recieved
        //_peerManager.sendMessage(msg.getAuthor(), getMoreHeaders.toJson());
        messages.push_back(getMoreHeaders);

    }
    else 
    {
        // We either have all headers or reached the tip from that peer's perspective.
        // Start requesting blocks for the newly discovered headers.
        auto queuedHeaders = _blockchain->getAppendedHeaders();

        for (auto qHeader : queuedHeaders)
        {
            MessageP2P getBlockMsg = MessageManager::createGetBlockMessage(_node.getMyPublicKey(), qHeader.getHash(), qHeader.getPreviousHash());

            // Maybe broadcast instead?
            //_peerManager.sendMessage(msg.getAuthor(), getBlockMsg.toJson());
            messages.push_back(getBlockMsg);
        }
    }
    return messages;
}

std::vector<MessageP2P> FullNodeMessageHandler::onHandshake(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::HEADERS)
    {
        return {};
    }
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Handshake from peer." << std::endl;
    

    PeerInfo newPeer = PeerInfo::fromJson(msg.getPayload());

    // Get my peer list (before adding the new peer)
    json serializedPeerList = _node.peersToJson();

    // Add the new peer to the node's peer list
    _node.addPeer(newPeer);

    std::vector<MessageP2P> messages;
    // Send my peer list back to the new peer started communication
    MessageP2P peerListMsg = MessageManager::createPeerListMessage(_node.getMyPublicKey(), serializedPeerList);
    messages.push_back(peerListMsg);

    return messages;
}
