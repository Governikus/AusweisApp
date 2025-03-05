/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerReader.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerReader::MsgHandlerReader(const QJsonObject& pObj, const MsgContext& pContext)
	: MsgHandler(MsgType::READER)
{
	const auto& jsonName = pObj[QLatin1String("name")];
	if (jsonName.isUndefined())
	{
		setError(QLatin1String("Name cannot be undefined"));
	}
	else if (!jsonName.isString())
	{
		setError(QLatin1String("Invalid name"));
	}
	else
	{
		setReaderInfo(Env::getSingleton<ReaderManager>()->getReaderInfo(jsonName.toString()), pContext);
	}
}


MsgHandlerReader::MsgHandlerReader(const ReaderInfo& pInfo, const MsgContext& pContext)
	: MsgHandler(MsgType::READER)
{
	Q_ASSERT(!pInfo.getName().isEmpty());
	setReaderInfo(pInfo, pContext);
}


void MsgHandlerReader::setError(const QLatin1String pError)
{
	setValue(QLatin1String("error"), pError);
}


void MsgHandlerReader::setReaderInfo(const ReaderInfo& pInfo, const MsgContext& pContext)
{
	insertJsonObject(createReaderInfo(pInfo, pContext));
}


QJsonObject MsgHandlerReader::createReaderInfo(const ReaderInfo& pInfo, const MsgContext& pContext)
{
	Q_ASSERT(!pInfo.getName().isEmpty());
	QJsonObject obj;

	const bool known = pInfo.isValid();
	obj[QLatin1String("name")] = pInfo.getName();
	obj[QLatin1String("attached")] = known;
	if (known)
	{
		obj[QLatin1String("insertable")] = pInfo.isInsertable();
		obj[QLatin1String("keypad")] = !pInfo.isBasicReader();

		if (pInfo.hasEid() || (pInfo.hasCard() && pContext.getApiLevel() > MsgLevel::v2))
		{
			QJsonObject card;
			if (pInfo.hasEid())
			{
#if __has_include("SmartManager.h")
				card[QLatin1String("eidType")] = Enum<AcceptedEidType>::getName(static_cast<AcceptedEidType>(pInfo.getCardInfo().getMobileEidType()));
#endif
				card[QLatin1String("deactivated")] = pInfo.isPinDeactivated();
				card[QLatin1String("inoperative")] = pInfo.isPukInoperative();
				card[QLatin1String("retryCounter")] = pInfo.getRetryCounter();
			}
			obj[QLatin1String("card")] = card;
		}
		else
		{
			obj[QLatin1String("card")] = QJsonValue::Null;
		}
	}

	return obj;
}
