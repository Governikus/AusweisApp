/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "PaosMessage.h"

using namespace governikus;

PaosMessage::PaosMessage(PaosType pType)
	: mMessageID()
	, mRelatesTo()
	, mType(pType)
{
}


PaosMessage::~PaosMessage()
{
}


bool PaosMessage::handleWSAddressingHeaders(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes&)
{
	if (pElementName == QLatin1String("MessageID"))
	{
		mMessageID = pValue;
		return true;
	}
	else if (pElementName == QLatin1String("RelatesTo"))
	{
		mRelatesTo = pValue;
		return true;
	}
	return false;
}
