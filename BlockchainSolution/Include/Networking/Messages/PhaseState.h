#pragma once

#include "MessageP2P.h"
#include "Structure/Block.h"

class PhaseState
{
public:
	PhaseState();
	PhaseState(const Block& block);

	void addPrepareMessage(const MessageP2P& prepareMessage);
	void addCommitMessage(const MessageP2P& commitMessage);
	
	bool isRecievedPrepareMessageFromAuthorAlready(const std::string& author);
	bool isRecievedCommitMessageFromAuthorAlready(const std::string& author);

	int getPrepareAmount() const;
	int getCommitAmount() const;

	void setHashReady(const Block& block);
	void setPrepared();
	void setCommitted();

	bool isHashReady();
	bool isPrepared();
	bool isCommitted();
private:
	bool _prePrepared;
	bool _hashReady;
	bool _prepared;
	bool _committed;
	
	std::vector<MessageP2P> _prepareMessages;
	std::vector<MessageP2P> _commitMessages;

	Block _block;
};

