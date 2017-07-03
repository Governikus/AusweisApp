/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
	mJsonObject["error"] = pError;
}


void MsgHandlerEnterNumber::setReader(const QSharedPointer<const WorkflowContext>& pContext)
{
	const auto& reader = pContext->getReaderName();
	if (!reader.isEmpty())
	{
		const auto& info = ReaderManager::getInstance().getReaderInfo(reader);
		if (info.isValid())
		{
			mJsonObject["reader"] = MsgHandlerReader::createReaderInfo(info);
		}
	}
}


void MsgHandlerEnterNumber::parseValue(const QJsonValue& pValue, const std::function<void(const QString& pNumber)>& pFunc, ushort pCount)
{
	if (pValue.isUndefined())
	{
		setError(QStringLiteral("Value cannot be undefined"));
	}
	else if (!pValue.isString())
	{
		setError(QStringLiteral("Invalid value"));
	}
	else
	{
		const auto& regex = QStringLiteral("^[0-9]{%1}$").arg(pCount);
		const auto& number = pValue.toString();
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
