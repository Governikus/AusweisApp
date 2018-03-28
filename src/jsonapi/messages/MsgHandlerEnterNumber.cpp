/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterNumber.h"

#include "MsgHandlerReader.h"
#include "ReaderManager.h"

#include <QRegularExpression>

using namespace governikus;

MsgHandlerEnterNumber::MsgHandlerEnterNumber(MsgType pType, const MsgContext& pContext)
	: MsgHandler(pType)
{
	Q_ASSERT(pContext.getWorkflowContext());
	setReader(pContext.getWorkflowContext());
}


void MsgHandlerEnterNumber::setError(const QString& pError)
{
	mJsonObject[QLatin1String("error")] = pError;
}


void MsgHandlerEnterNumber::setReader(const QSharedPointer<const WorkflowContext>& pContext)
{
	const auto& reader = pContext->getReaderName();
	if (!reader.isEmpty())
	{
		const auto& info = ReaderManager::getInstance().getReaderInfo(reader);
		if (info.isConnected())
		{
			mJsonObject[QLatin1String("reader")] = MsgHandlerReader::createReaderInfo(info);
		}
	}
}


void MsgHandlerEnterNumber::parseValue(const QJsonObject& pObj, const std::function<void(const QString& pNumber)>& pFunc, ushort pCount)
{
	const auto& value = pObj[QLatin1String("value")];
	if (value.isUndefined())
	{
		setError(QStringLiteral("Value cannot be undefined"));
	}
	else if (!value.isString())
	{
		setError(QStringLiteral("Invalid value"));
	}
	else
	{
		const auto& regex = QStringLiteral("^[0-9]{%1}$").arg(pCount);
		const auto& number = value.toString();
		if (QRegularExpression(regex).match(number).hasMatch())
		{
			pFunc(number);
		}
		else
		{
			setError(QStringLiteral("You must provide %1 digits").arg(pCount));
		}
	}
}
