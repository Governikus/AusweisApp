/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetSessionId.h"

#include "LogHandler.h"
#include "context/PersonalizationContext.h"

#include <QJsonDocument>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


StateGetSessionId::StateGetSessionId(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericProviderCommunication(pContext)
{
}


QUrl StateGetSessionId::getRequestUrl() const
{
	return getContext()->getRefreshUrl();
}


void StateGetSessionId::setProgress() const
{
	const auto& context = getContext();
	Q_ASSERT(context);
	//: LABEL ANDROID IOS
	context->setProgress(100, tr("Getting results from server"));
}


bool StateGetSessionId::parseSessionId(const QByteArray& pData)
{
	QJsonParseError jsonError {};
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qDebug() << "JSON parsing failed:" << jsonError.errorString();
		return false;
	}

	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	const auto obj = json.object();
	const auto sessionId = QUuid(obj.value(QLatin1String("sessionID")).toString());
	if (sessionId.isNull())
	{
		return false;
	}

	context->setSessionIdentifier(sessionId);
	return true;
}


void StateGetSessionId::handleNetworkReply(const QByteArray& pContent)
{
	if (parseSessionId(pContent))
	{
		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "No valid sessionID to prepare personalization";
	updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
	Q_EMIT fireAbort();
}
