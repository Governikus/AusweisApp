/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandler.h"

#include "states/StateEditAccessRights.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateEstablishPacePuk.h"
#include "states/StateSelectReader.h"

#include <QJsonDocument>

using namespace governikus;

const MsgLevel MsgHandler::DEFAULT_MSG_LEVEL = MsgLevel::v1;

const MsgHandler MsgHandler::Void = MsgHandler();


MsgType MsgHandler::getStateMsgType(const QString& pState)
{
	if (AbstractState::isState<StateEstablishPacePin>(pState))
	{
		return MsgType::ENTER_PIN;
	}
	else if (AbstractState::isState<StateEstablishPaceCan>(pState))
	{
		return MsgType::ENTER_CAN;
	}
	else if (AbstractState::isState<StateEstablishPacePuk>(pState))
	{
		return MsgType::ENTER_PUK;
	}
	else if (AbstractState::isState<StateEditAccessRights>(pState))
	{
		return MsgType::ACCESS_RIGHTS;
	}
	else if (AbstractState::isState<StateSelectReader>(pState))
	{
		return MsgType::INSERT_CARD;
	}

	// indicates "do not use this" otherwise it is an internal error!
	return MsgType::INTERNAL_ERROR;
}


MsgHandler::MsgHandler()
	: MsgHandler(MsgType::INTERNAL_ERROR)
{
	setVoid();
}


MsgHandler::MsgHandler(MsgType pType)
	: mType(pType)
	, mVoid(false)
	, mJsonObject()
{
	mJsonObject[QLatin1String("msg")] = getEnumName(mType);
}


MsgHandler::MsgHandler(MsgType pType, const char* pKey, const QString& pValue)
	: MsgHandler(pType)
{
	setValue(pKey, pValue);
}


MsgHandler::MsgHandler(MsgType pType, const char* pKey, const QLatin1String pValue)
	: MsgHandler(pType)
{
	setValue(pKey, pValue);
}


QByteArray MsgHandler::toJson() const
{
	Q_ASSERT(mJsonObject[QLatin1String("msg")].isString());
	return QJsonDocument(mJsonObject).toJson(QJsonDocument::Compact);
}


QByteArray MsgHandler::getOutput() const
{
	if (isVoid())
	{
		return QByteArray();
	}

	return toJson();
}


bool MsgHandler::isVoid() const
{
	return mVoid;
}


MsgType MsgHandler::getType() const
{
	return mType;
}


void MsgHandler::setRequest(const QJsonObject& pRequest)
{
	const QLatin1String requestName("request");

	const auto& requestValue = pRequest[requestName];
	if (!requestValue.isUndefined())
	{
		mJsonObject[requestName] = requestValue;
	}
}


void MsgHandler::setValue(const char* pKey, const QString& pValue)
{
	setValue(QLatin1String(pKey), pValue);
}


void MsgHandler::setValue(const QLatin1String pKey, const QLatin1String pValue)
{
	if (pValue.size())
	{
		mJsonObject[pKey] = pValue;
	}
}


void MsgHandler::setValue(const char* pKey, const QLatin1String pValue)
{
	setValue(QLatin1String(pKey), pValue);
}


void MsgHandler::setVoid(bool pVoid)
{
	mVoid = pVoid;
}


void MsgHandler::setValue(const QLatin1String pKey, const QString& pValue)
{
	if (!pValue.isEmpty())
	{
		mJsonObject[pKey] = pValue;
	}
}
