/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
	body[QLatin1String("osType")] = getOsType();

	return QJsonDocument(body).toJson(QJsonDocument::Compact);
}


void StateGetChallenge::setProgress() const
{
	const auto& context = getContext();
	Q_ASSERT(context);
	//: LABEL ANDROID IOS
	context->setProgress(10, tr("Getting challenge from server"));
}


QString StateGetChallenge::getOsType() const
{
#if defined(Q_OS_IOS)
	return QStringLiteral("iOS");

#elif defined(Q_OS_ANDROID)
	return QStringLiteral("Android");

#else
	static const char* SMART_EID_MOCK_OS_TYPE = "AUSWEISAPP2_SMART_EID_MOCK_OS_TYPE";
	const auto os = qEnvironmentVariable(SMART_EID_MOCK_OS_TYPE, QStringLiteral("Unknown"));
	qDebug() << "Using" << SMART_EID_MOCK_OS_TYPE << ":" << os;
	return os;

#endif
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
	Q_EMIT fireAbort();
}
