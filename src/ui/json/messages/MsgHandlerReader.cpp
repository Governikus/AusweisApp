/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerReader.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerReader::MsgHandlerReader(const QJsonObject& pObj)
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
		setReaderInfo(Env::getSingleton<ReaderManager>()->getReaderInfo(jsonName.toString()));
	}
}


MsgHandlerReader::MsgHandlerReader(const ReaderInfo& pInfo)
	: MsgHandler(MsgType::READER)
{
	Q_ASSERT(!pInfo.getName().isEmpty());
	setReaderInfo(pInfo);
}


void MsgHandlerReader::setError(const QLatin1String pError)
{
	mJsonObject[QLatin1String("error")] = pError;
}


void MsgHandlerReader::setReaderInfo(const ReaderInfo& pInfo)
{
	setReaderInfo(mJsonObject, pInfo);
}


QJsonObject MsgHandlerReader::createReaderInfo(const ReaderInfo& pInfo)
{
	Q_ASSERT(!pInfo.getName().isEmpty());
	QJsonObject obj;
	setReaderInfo(obj, pInfo);
	return obj;
}


void MsgHandlerReader::setReaderInfo(QJsonObject& pObj, const ReaderInfo& pInfo)
{
	pObj[QLatin1String("name")] = pInfo.getName();
	pObj[QLatin1String("attached")] = pInfo.isConnected();
	if (pInfo.isConnected())
	{
		pObj[QLatin1String("keypad")] = !pInfo.isBasicReader();

		if (pInfo.hasEidCard())
		{
			QJsonObject card;
			card[QLatin1String("deactivated")] = pInfo.isPinDeactivated();
			card[QLatin1String("inoperative")] = pInfo.isPukInoperative();
			card[QLatin1String("retryCounter")] = pInfo.getRetryCounter();
			pObj[QLatin1String("card")] = card;
		}
		else
		{
			pObj[QLatin1String("card")] = QJsonValue::Null;
		}
	}
}
