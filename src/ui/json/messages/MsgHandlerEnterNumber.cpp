/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterNumber.h"

#include "MsgHandlerReader.h"
#include "ReaderManager.h"

#include <QRegularExpression>

using namespace governikus;

MsgHandlerEnterNumber::MsgHandlerEnterNumber(MsgType pType, const MsgContext& pContext)
	: MsgHandler(pType)
{
	Q_ASSERT(pContext.getContext());
	setReader(pContext.getContext());
}


void MsgHandlerEnterNumber::setError(const QString& pError)
{
	mJsonObject[QLatin1String("error")] = pError;
}


void MsgHandlerEnterNumber::setReader(const QSharedPointer<const WorkflowContext>& pContext)
{
	const auto& info = pContext->getExpectedReader();
	if (!info.getName().isEmpty())
	{
		mJsonObject[QLatin1String("reader")] = MsgHandlerReader::createReaderInfo(info);
	}
}


void MsgHandlerEnterNumber::parseValue(const QJsonObject& pObj,
		const MsgContext& pContext,
		const std::function<void(const QString& pNumber)>& pFunc, std::pair<ushort, ushort> pCount)
{
	const auto& reader = pContext.getContext()->getReaderName();
	if (reader.isEmpty())
	{
		setError(QStringLiteral("No card inserted"));
		return;
	}

	const auto& value = pObj[QLatin1String("value")];

	if (Env::getSingleton<ReaderManager>()->getReaderInfo(reader).isBasicReader())
	{
		if (value.isUndefined())
		{
			setError(QStringLiteral("Value cannot be undefined"));
			return;
		}

		if (!value.isString())
		{
			setError(QStringLiteral("Invalid value"));
			return;
		}

		const auto& regex = QStringLiteral("^[0-9]{%1,%2}$").arg(pCount.first).arg(pCount.second);
		const auto& number = value.toString();
		if (QRegularExpression(regex).match(number).hasMatch())
		{
			pFunc(number);
		}
		else
		{
			if (pCount.first == pCount.second)
			{
				setError(QStringLiteral("You must provide %1 digits").arg(pCount.first));
			}
			else
			{
				setError(QStringLiteral("You must provide %1 - %2 digits").arg(pCount.first).arg(pCount.second));
			}
		}
	}
	else
	{
		if (value.isUndefined())
		{
			pFunc(QString());
		}
		else
		{
			setError(QStringLiteral("Value cannot be defined"));
		}
	}
}
