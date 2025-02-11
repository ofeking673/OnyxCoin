#pragma once

#include "MessageP2P.h"
#include "Block.h"

class PhaseState
{
public:
	PhaseState(const Block& block);

	void addPrepareMessage(const MessageP2P& prepareMessage);
	void addCommitMessage(const MessageP2P& commitMessage);
	
	bool isRecievedPrepareMessageFromAuthorAlready(const std::string& author);
	bool isRecievedCommitMessageFromAuthorAlready(const std::string& author);

	int getPrepareAmount() const;
	int getCommitAmount() const;

	void setPrepared();
	void setCommitted();

	bool isPrepared();
	bool isCommitted();
private:
	bool _prePrepared;
	bool _prepared;
	bool _committed;
	
	std::vector<MessageP2P> _prepareMessages;
	std::vector<MessageP2P> _commitMessages;

	Block _block;
};

