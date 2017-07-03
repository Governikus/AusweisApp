/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerReader.h"

#include "ReaderManager.h"

using namespace governikus;

MsgHandlerReader::MsgHandlerReader(const QJsonObject& pObj)
	: MsgHandler(MsgType::READER)
{
	const auto& jsonName = pObj["name"];
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
		setReaderInfo(jsonName.toString());
	}
}


MsgHandlerReader::MsgHandlerReader(const QString& pName)
	: MsgHandler(MsgType::READER)
{
	Q_ASSERT(!pName.isEmpty());
	setReaderInfo(pName);
}


void MsgHandlerReader::setError(const QLatin1String& pError)
{
	mJsonObject["error"] = pError;
}


void MsgHandlerReader::setReaderInfo(const QString& pName)
{
	setReaderInfo(mJsonObject, ReaderManager::getInstance().getReaderInfo(pName), pName);
}


QJsonObject MsgHandlerReader::createReaderInfo(const ReaderInfo& pInfo)
{
	Q_ASSERT(!pInfo.getName().isEmpty());
	QJsonObject obj;
	setReaderInfo(obj, pInfo, pInfo.getName());
	return obj;
}


void MsgHandlerReader::setReaderInfo(QJsonObject& pObj, const ReaderInfo& pInfo, const QString& pName)
{
	pObj["name"] = pName; // do NOT use pInfo here as it can be empty!
	pObj["attached"] = pInfo.isValid();
	if (pInfo.isValid())
	{
		QJsonObject card;
		const bool npa = pInfo.getCardType() == CardType::EID_CARD;
		card["inserted"] = npa;
		if (npa)
		{
			card["deactivated"] = pInfo.isPinDeactivated();
			card["retryCounter"] = pInfo.getRetryCounter();
		}
		pObj["card"] = card;
	}
}
