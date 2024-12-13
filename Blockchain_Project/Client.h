#pragma once
#include "IClient.h"
#include "Networking/ClientSocket.h"

class Client : public IClient
{
	virtual void Action() override;
};

