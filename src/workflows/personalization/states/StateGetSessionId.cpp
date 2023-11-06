/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetSessionId.h"

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


bool StateGetSessionId::parseResponse(const QByteArray& pData) const
{
	QJsonParseError jsonError {};
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qDebug() << "JSON parsing failed:" << jsonError.errorString();
		return false;
	}

	const auto sessionId = QUuid(json.object().value(QLatin1String("sessionID")).toString());
	if (sessionId.isNull())
	{
		qDebug() << "No valid sessionID to prepare personalization";
		return false;
	}

	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);
	context->setSessionIdentifier(sessionId);
	return true;
}


void StateGetSessionId::handleNetworkReply(const QByteArray& pContent)
{
	if (parseResponse(pContent))
	{
		Q_EMIT fireContinue();
		return;
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Authentication_Failed);
	Q_EMIT fireAbort(FailureCode::Reason::Get_Session_Id_Invalid);
}
