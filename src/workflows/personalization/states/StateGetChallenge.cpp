/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetChallenge.h"

#include "context/PersonalizationContext.h"

#include <QJsonDocument>
#include <QUrlQuery>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


StateGetChallenge::StateGetChallenge(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericProviderCommunication(pContext)
{
}


QUrl StateGetChallenge::getRequestUrl() const
{
	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	return context->getAppletServiceUrl(QStringLiteral("challenge"));
}


QByteArray StateGetChallenge::getPayload() const
{
	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	QJsonObject body;
	body[QLatin1String("sessionID")] = context->getSessionIdentifier().toString(QUuid::WithoutBraces);
	body[QLatin1String("challengeType")] = context->getChallengeType();

	return QJsonDocument(body).toJson(QJsonDocument::Compact);
}


void StateGetChallenge::setProgress() const
{
	const auto& context = getContext();
	Q_ASSERT(context);
	//: LABEL ANDROID IOS
	context->setProgress(10, tr("Getting challenge from server"));
}


bool StateGetChallenge::parseChallenge(const QByteArray& pData)
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
	const auto challenge = obj.value(QLatin1String("challenge")).toString();
	context->setChallenge(challenge);
	return !challenge.isEmpty();
}


void StateGetChallenge::handleNetworkReply(const QByteArray& pContent)
{
	if (parseChallenge(pContent))
	{
		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "No valid challenge to prepare personalization";
	updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
	Q_EMIT fireAbort(FailureCode::Reason::Get_Challenge_Invalid);
}
