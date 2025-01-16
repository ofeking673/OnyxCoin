#include "FullNodeMessageHandler.h"
#include "iostream"

FullNodeMessageHandler::FullNodeMessageHandler()
{
	_blockchain = Blockchain::getInstance();
	_utxoSet = UTXOSet::getInstance();
}

FullNodeMessageHandler::~FullNodeMessageHandler()
{
    // Cleanup if necessary
}

void FullNodeMessageHandler::onPing(const MessageP2P& msg)
{
    if (msg.getType() != MessageType::PING)
    {
        return;
    }

    // Log the event
    std::cout << "[FullNodeMessageHandler] Received PING from peer." << std::endl;


    // Typical behavior might include sending a PONG message back.
    // That would require access to a network interface or peer manager
    // (not shown here). For example:
    //
    // MessageP2P pongMsg(
    //     msg.getSignature(),          // Keep the same signature or your own
    //     MessageType::PONG,           // Message type
    //     0,                           // No payload
    //     {}                           // Empty payload vector
    // );
    // network->send(pongMsg);




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

void FullNodeMessageHandler::onPong(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received PONG from peer." << std::endl;

    // Typical behavior might include updating a "last seen" timestamp
    // for the peer that sent the PONG, or other connection management tasks.

    /*
    Typical Logic
    Log that you received a PONG.
    Update the peer’s “last seen” timestamp or note that the peer responded promptly.
    Possible Steps

    Print/Log the event.
    Update “lastSeenTime” or “lastPongTime” for the peer.
    If you were doing a round-trip measurement, store the RTT (round-trip time).    
    */
}

void FullNodeMessageHandler::onGetPeers(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetPeers from peer." << std::endl;

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
}

void FullNodeMessageHandler::onPeerList(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Peer List from peer." << std::endl;

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
}

void FullNodeMessageHandler::onGetBlock(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetBlock from peer." << std::endl;

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
}

void FullNodeMessageHandler::onBlock(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Block from peer." << std::endl;

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
}

void FullNodeMessageHandler::onNewTransaction(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received New Transaction from peer." << std::endl;

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
}

void FullNodeMessageHandler::onGetTransaction(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetTransaction from peer." << std::endl;

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
}

void FullNodeMessageHandler::onInventory(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received INVENTORY ANNOUUNCEMENT from peer." << std::endl;

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
}

void FullNodeMessageHandler::onGetHeaders(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received GetHeaders from peer." << std::endl;

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
}

void FullNodeMessageHandler::onHeaders(const MessageP2P& msg)
{
    // Log the event
    std::cout << "[FullNodeMessageHandler] Received Block Headers from peer." << std::endl;

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
}

