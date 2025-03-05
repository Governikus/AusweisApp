/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInsertCard.h"

#include "ReaderManager.h"

#include <QJsonArray>

using namespace governikus;

MsgHandlerInsertCard::MsgHandlerInsertCard()
	: MsgHandler(MsgType::INSERT_CARD)
{
	const auto& infos = Env::getSingleton<ReaderManager>()->getReaderInfos();
	for (const auto& entry : infos)
	{
		if (entry.hasEid())
		{
			setVoid();
			break;
		}
	}
}


MsgHandlerInsertCard::MsgHandlerInsertCard(MsgContext& pContext)
	: MsgHandlerInsertCard()
{
	Q_ASSERT(pContext.getContext());
	pContext.getContext()->setStateApproved();
}


MsgHandlerInsertCard::MsgHandlerInsertCard(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandler(MsgType::INSERT_CARD)
{
	Q_ASSERT(pContext.getContext());

	const auto& value = pObj[QLatin1String("name")];

	if (value.isUndefined())
	{
		setError(QLatin1String("Name cannot be undefined"));
		return;
	}

	if (!value.isString())
	{
		setError(QLatin1String("Invalid name"));
		return;
	}


	const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(value.toString());
	if (!readerInfo.isValid())
	{
		setError(QLatin1String("Unknown reader name"));
		return;
	}

	if (!readerInfo.isInsertable())
	{
		setError(QLatin1String("Card is not insertable"));
		return;
	}

	QVariant data;
	if (readerInfo.getPluginType() == ReaderManagerPluginType::SIMULATOR)
	{
		const auto& filesValue = pObj[QLatin1String("simulator")];
		if (!filesValue.isUndefined() && !filesValue.isObject())
		{
			setError(QLatin1String("Parameter 'simulator' is invalid"));
			return;
		}
		data = filesValue.toObject();
	}

	Env::getSingleton<ReaderManager>()->insert(readerInfo, data);
	setVoid();
}


void MsgHandlerInsertCard::setError(const QLatin1String pError)
{
	setValue(QLatin1String("error"), pError);
}
