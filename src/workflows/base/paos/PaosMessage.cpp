/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "PaosMessage.h"


using namespace governikus;


PaosMessage::PaosMessage(PaosType pType)
	: mMessageID()
	, mRelatesTo()
	, mType(pType)
{
}


PaosMessage::~PaosMessage() = default;


const QString& PaosMessage::getMessageId() const
{
	return mMessageID;
}


void PaosMessage::setMessageId(const QString& messageId)
{
	mMessageID = messageId;
}


const QString& PaosMessage::getRelatesTo() const
{
	return mRelatesTo;
}


void PaosMessage::setRelatesTo(const QString& relatesTo)
{
	mRelatesTo = relatesTo;
}
