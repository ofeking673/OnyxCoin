# Network Message Documentation

This document describes all the P2P messages used in the blockchain network.  
Each network message follows the same top-level structure, but the payload and the meaning of the fields differ depending on the message type.

---

## 1. Message Structure

A P2P message is represented by the **`MessageP2P`** class and contains:

| Field             | Type                          | Description                                                                                                     |
|-------------------|-------------------------------|-----------------------------------------------------------------------------------------------------------------|
| **Signature**     | `std::string`                 | A signature of the whole message using ECDSA with the peer’s private key. In order to authenticate the peer.    |
| **Author**        | `std::string`                 | The public key of the author of this message usually used for replying with information to this user.|
| **Type**          | `MessageType` (enum class)    | The message type, indicating the purpose or command (e.g., `PING`, `PONG`, `GET_BLOCK`, etc.).                  |
| **PayloadLength** | `uint32_t`                    | The length of the **Payload**.                                                                                 |
| **Payload**       | `std::string`                 | The message-specific data. Once deserialized, it may be interpreted as transaction data, block data, inventory data, etc., depending on the message type. |
    


```cpp
// Example structure in code
MessageP2P
{
    // Header
    std::string           _signature;
    std::string           _author;
    MessageType           _type;
    uint32_t              _payloadLength;

    // Payload (actual data)
    std::string  _payload;
};
```

## 2. Message Types Overview

Below is a table summarizing the `MessageType` enum:

| Enum Value          | Integer | Description                                                        |
|-------------------  |---------|--------------------------------------------------------------------|
| **PING**            | 1       | Used to check if the peer is alive/connected.                      |
| **PONG**            | 2       | Response to a PING.                                                |
| **GET_PEERS**       | 3       | Request a list of known peers from another node.                   |
| **PEER_LIST**       | 4       | Provide a list of known peers.                                     |
| **GET_BLOCK**       | 5       | Request a specific block by hash.                                  |
| **BLOCK**           | 6       | Provide a block.                                                   |
| **NEW_TRANSACTION** | 7       | Broadcast a new transaction to the network.                        |
| **GET_TRANSACTION** | 8       | Request a specific transaction by ID.                              |
| **INVENTORY**       | 9       | Broadcast inventory data (lists of known blocks/transactions).     |
| **GET_HEADERS**     | 10      | Request block headers from a certain point in the chain.           |
| **HEADERS**         | 11      | Provide a list of block headers in response to GET_HEADERS.        |

The following sections describe each message in more detail.

## 3. Message Details

### 3.1 PING
**Type Value:** `MessageType::PING`  
**Purpose:**  
- To check if the remote peer is alive and responsive.  
- Can also be used to keep connections alive.

**Payload:**  
- Timestamp, for when the PING has been sent.

**Typical Usage:**  
- A peer sends `PING` to another peer after a period of inactivity or on connection establishment to check availability.  
- The receiver should promptly respond with `PONG`.

---

### 3.2 PONG
**Type Value:** `MessageType::PONG`  
**Purpose:**  
- To respond to a `PING` message.

**Payload:**  
- Echoes the Timestamp, `PING` provided.

**Typical Usage:**  
- Whenever a `PING` is received, respond with `PONG`.

---

### 3.3 GET_PEERS
**Type Value:** `MessageType::GET_PEERS`  
**Purpose:**  
- To request a list of known peers from a node.
- Useful when a node wants to discover new peers to connect to.

**Payload:**  
- Empty (the node just wants the peer list).

**Typical Usage:**  
- A new node or a node that wants to expand its peer connections sends `GET_PEERS`.  
- The receiving node responds with `PEER_LIST`.

---

### 3.4 PEER_LIST
**Type Value:** `MessageType::PEER_LIST`  
**Purpose:**  
- To provide a list of known peers.

**Payload:**  
- Serialized list of peers (A map of [public key and a bool of isConnectionAlive] _to_ [socket]).  

**Typical Usage:**  
- Sent as a response to `GET_PEERS`.  
- Could also be broadcast periodically to propagate peer knowledge.

---

### 3.5 GET_BLOCK
**Type Value:** `MessageType::GET_BLOCK`  
**Purpose:**  
- To request a specific block from the peer by its hash and the previous block hash.

**Payload:**  
- **Block Hash:** The identifier of the block being requested.  
- **Previous Block Hash:** Used to ensure continuity or to help peers locate the block.

**Typical Usage:**  
- A node detects it is missing a particular block in the chain.  
- It sends `GET_BLOCK` along with the block’s hash.  
- The peer responds with a `BLOCK` message containing the block data.

---

### 3.6 BLOCK
**Type Value:** `MessageType::BLOCK`  
**Purpose:**  
- To send a full block (header + transactions) to the peer.
- Typically a response to `GET_BLOCK`.

**Payload:**  
- The serialized block data, which includes the block header (index, timestamp, previousHash, hash) and the list of transactions.

**Typical Usage:**  
- Sent in response to `GET_BLOCK`.  
- Can also be broadcast when a new block is discovered, using
 `INVENTORY`–then–`GET_BLOCK`–then–`BLOCK` pattern.

---

### 3.7 NEW_TRANSACTION
**Type Value:** `MessageType::NEW_TRANSACTION`  
**Purpose:**  
- To broadcast a newly created or received transaction to the rest of the network.

**Payload:**  
- The serialized transaction data.

**Typical Usage:**  
- A node mines or receives a new transaction.  
- It sends `NEW_TRANSACTION` to its peers.  
- Peers can validate the transaction and possibly relay it further.

---

### 3.8 GET_TRANSACTION
**Type Value:** `MessageType::GET_TRANSACTION`  
**Purpose:**  
- To request a specific transaction by its ID.

**Payload:**  
- **Transaction ID:** The unique identifier of the transaction.

**Typical Usage:**  
- A node sees an `INVENTORY` message containing a transaction it does not have.  
- It sends `GET_TRANSACTION` with the transaction’s ID.  
- The peer responds with `NEW_TRANSACTION` message containing the transaction data.

---

### 3.9 INVENTORY
**Type Value:** `MessageType::INVENTORY`  
**Purpose:**  
- To inform peers about blocks and transactions a node has.
- Helps peers decide if they need to request new blocks or transactions.

**Payload:**  
- **InventoryData** structure, which contains:  
  - A list of transaction IDs (`_txIDs`).  
  - A list of block (hash, prevHash) pairs (`_blocksHash`).

**Typical Usage:**  
- A node discovers new blocks or transactions.  
- It broadcasts `INVENTORY` so that peers know these items are available.  
- Interested peers may respond with `GET_BLOCK` or `GET_TRANSACTION`.

---

### 3.10 GET_HEADERS
**Type Value:** `MessageType::GET_HEADERS`  
**Purpose:**  
- To request a list of block headers, typically from some point in the chain.
- Helps a node synchronize headers (and later decide which blocks to fetch).

**Payload**  
- **Block Locator Hashes:** An ordered list of block hashes from the node’s locally known best chain, typically starting from the newest block back to older blocks.  
  - The receiving peer finds a point where it shares history and then starts sending headers from that point.
- **Stop Hash:** A block hash indicating where the peer should stop sending headers. If empty or zero, it means “send as many headers as you have, up to the limit.”

**Typical Usage:**  
- A node wants to synchronize with the best chain.  
- It sends `GET_HEADERS` with a set of block locator hashes and a stop hash.  
- The peer responds with `HEADERS` containing the next block headers in the chain.

---

### 3.11 HEADERS
**Type Value:** `MessageType::HEADERS`  
**Purpose:**  
- To provide block headers (usually in response to `GET_HEADERS`).

**Payload**  
- A serialized list of block headers. Each header includes:  
  - Index
  - Previous block hash    
  - Timestamp  
  - Hash

**Typical Usage:**  
- Sent in response to `GET_HEADERS`.  
- A node receiving `HEADERS` can decide which blocks it needs to fully download (via `GET_BLOCK`) or continue requesting more headers until fully synced.
