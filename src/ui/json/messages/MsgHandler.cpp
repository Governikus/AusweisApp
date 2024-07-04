/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandler.h"

#include "states/StateEditAccessRights.h"
#include "states/StateEnterNewPacePin.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateSelectReader.h"
#include "states/StateUnfortunateCardPosition.h"

#include <QJsonDocument>

using namespace governikus;

const MsgLevel MsgHandler::DEFAULT_MSG_LEVEL = MsgLevel::v3;

const MsgHandler MsgHandler::Void = MsgHandler();


MsgType MsgHandler::getStateMsgType(const QString& pState, PacePasswordId pPasswordId)
{
	if (StateBuilder::isState<StateEnterPacePassword>(pState))
	{
		if (pPasswordId == PacePasswordId::PACE_PIN)
		{
			return MsgType::ENTER_PIN;
		}
		else if (pPasswordId == PacePasswordId::PACE_CAN)
		{
			return MsgType::ENTER_CAN;
		}
		else if (pPasswordId == PacePasswordId::PACE_PUK)
		{
			return MsgType::ENTER_PUK;
		}
	}
	else if (StateBuilder::isState<StateEnterNewPacePin>(pState))
	{
		return MsgType::ENTER_NEW_PIN;
	}
	else if (StateBuilder::isState<StateEditAccessRights>(pState))
	{
		return MsgType::ACCESS_RIGHTS;
	}
	else if (StateBuilder::isState<StateSelectReader>(pState))
	{
		return MsgType::INSERT_CARD;
	}
	else if (StateBuilder::isState<StateUnfortunateCardPosition>(pState))
	{
		return MsgType::PAUSE;
	}

	return MsgType::VOID;
}


MsgHandler::MsgHandler()
	: MsgHandler(MsgType::VOID)
{
	setVoid();
}


MsgHandler::MsgHandler(MsgType pType)
	: mType(pType)
	, mVoid(false)
	, mJsonObject()
{
	setValue(QLatin1String("msg"), getEnumName(mType));
}


MsgHandler::MsgHandler(MsgType pType, const char* const pKey, const QString& pValue)
	: MsgHandler(pType)
{
	setValue(QLatin1String(pKey), pValue);
}


MsgHandler::MsgHandler(MsgType pType, const char* const pKey, const QLatin1String pValue)
	: MsgHandler(pType)
{
	setValue(QLatin1String(pKey), pValue);
}


QByteArray MsgHandler::toJson() const
{
	Q_ASSERT(isString(QLatin1String("msg")));
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
	return mType == MsgType::VOID || mVoid;
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
		setValue(requestName, requestValue);
	}
}


void MsgHandler::setVoid(bool pVoid)
{
	mVoid = pVoid;
}


void MsgHandler::setValue(const QLatin1String pKey, const QString& pValue)
{
	if (!pValue.isEmpty())
	{
		setValue(pKey, QJsonValue(pValue));
	}
}


void MsgHandler::setValue(const QLatin1String pKey, const QLatin1String pValue)
{
	if (pValue.size())
	{
		setValue(pKey, QJsonValue(pValue));
	}
}


void MsgHandler::setValue(const QLatin1String pKey, const QJsonValue& pValue)
{
	mJsonObject[pKey] = pValue;
}


void MsgHandler::insertJsonObject(const QJsonObject& pObject)
{
	for (auto it = pObject.constBegin(); it != pObject.constEnd(); ++it)
	{
		mJsonObject.insert(it.key(), it.value());
	}
}


bool MsgHandler::contains(QLatin1StringView pKey) const
{
	return mJsonObject.contains(pKey);
}


bool MsgHandler::isString(const QLatin1String pKey) const
{
	return mJsonObject[pKey].isString();
}


MsgHandler::operator Msg() const
{
	return Msg(getType(), getOutput());
}
