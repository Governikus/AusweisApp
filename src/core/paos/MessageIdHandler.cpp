/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "paos/MessageIdHandler.h"

#include <QUuid>

using namespace governikus;

QString MessageIdHandler::createNewMsgId()
{
	return QUuid::createUuid().toString().mid(1, 36).prepend(QLatin1String("urn:uuid:"));
}


const QString& MessageIdHandler::getRemoteMsgId() const
{
	return mRemoteMsgId;
}


void MessageIdHandler::setRemoteMsgId(const QString& pNewRemoteMsgId)
{
	mRemoteMsgId = pNewRemoteMsgId;
}
