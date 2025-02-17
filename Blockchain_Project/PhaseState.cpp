#include "PhaseState.h"

PhaseState::PhaseState()
	:_prePrepared(false)
	, _prepared(false)
	, _committed(false)
{
}

PhaseState::PhaseState(const Block& block)
	: _block(block)
	, _prePrepared(true)
	, _prepared(false)
	, _committed(false)
{
}

void PhaseState::addPrepareMessage(const MessageP2P& prepareMessage)
{
	if (prepareMessage.getType() == MessageType::PREPARE)
	{
		// Check if already pushed a message from that author.
		if (!isRecievedPrepareMessageFromAuthorAlready(prepareMessage.getAuthor()))
		{
			_prepareMessages.push_back(prepareMessage);
		}
	}
}

void PhaseState::addCommitMessage(const MessageP2P& commitMessage)
{
	if (commitMessage.getType() == MessageType::COMMIT)
	{
		// Check if already pushed a message from that author.
		if (!isRecievedCommitMessageFromAuthorAlready(commitMessage.getAuthor()))
		{
			_prepareMessages.push_back(commitMessage);
		}
	}
}

bool PhaseState::isRecievedPrepareMessageFromAuthorAlready(const std::string& author)
{
	for (auto msg : _prepareMessages)
	{
		if (msg.getAuthor() == author)
		{
			return true;
		}
	}
	return false;
}

bool PhaseState::isRecievedCommitMessageFromAuthorAlready(const std::string& author)
{
	for (auto msg : _commitMessages)
	{
		if (msg.getAuthor() == author)
		{
			return true;
		}
	}
	return false;
}

int PhaseState::getPrepareAmount() const
{
	return _prepareMessages.size();
}

int PhaseState::getCommitAmount() const
{
	return _commitMessages.size();
}

void PhaseState::setPrepared()
{
	_prepared = true;
}

void PhaseState::setCommitted()
{
	_committed = true;
}

bool PhaseState::isPrepared()
{
	return _prepared;
}

bool PhaseState::isCommitted()
{
	return _committed;
}
