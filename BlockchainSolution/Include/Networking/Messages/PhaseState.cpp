#include "pch.h"
#include "PhaseState.h"

PhaseState::PhaseState()
	: _prePrepared(false)
	, _hashReady(false)
	, _prepared(false)
	, _committed(false)
	, _sentPrepare(false)
	, _sentCommit(false)
{
}

PhaseState::PhaseState(const Block& block)
	: _block(block)
	, _prePrepared(true)
	, _hashReady(false)
	, _prepared(false)
	, _committed(false)
	, _sentPrepare(false)
	, _sentCommit(false)
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
			std::cout << "Prepare messages: " << _prepareMessages.size() << std::endl;
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
			_commitMessages.push_back(commitMessage);
			std::cout << "Commit messages: " << _commitMessages.size() << std::endl;

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

void PhaseState::setHashReady(const Block& block)
{
	// Set the block as the proposed hash
	if (!_hashReady)
	{
		// Check if similar blocks
		if (_block.checkIfBlockMined(block))
		{
			_block = block;
			_hashReady = true;
		}
	}
}

void PhaseState::setPrepared()
{
	_prepared = true;
}

void PhaseState::setCommitted()
{
	_committed = true;
}

bool PhaseState::isHashReady() const
{
	return _hashReady;
}

bool PhaseState::isPrepared() const
{
	return _prepared;
}

bool PhaseState::isCommitted() const
{
	return _committed;
}

bool PhaseState::isSentPrepare() const
{
	return _sentPrepare;
}

bool PhaseState::isSentCommit() const
{
	return _sentCommit;
}

void PhaseState::setSentPrepare()
{
	_sentPrepare = true;
}

void PhaseState::setSentCommit()
{
	_sentCommit = true;
}
