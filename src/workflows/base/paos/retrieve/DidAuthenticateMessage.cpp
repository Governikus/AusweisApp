/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "DidAuthenticateMessage.h"


using namespace governikus;


void DidAuthenticateMessage::setConnectionHandle(const ConnectionHandle& pConnectionHandle)
{
	mConnectionHandle = pConnectionHandle;
}


void DidAuthenticateMessage::setDidName(const QString& pDidName)
{
	mDidName = pDidName;
}


DidAuthenticateMessage::DidAuthenticateMessage(PaosType pType):
	PaosMessage(pType)
{
}


DidAuthenticateMessage::~DidAuthenticateMessage() = default;


const ConnectionHandle& DidAuthenticateMessage::getConnectionHandle() const
{
	return mConnectionHandle;
}


const QString& DidAuthenticateMessage::getDidName() const
{
	return mDidName;
}
