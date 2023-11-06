/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePersonalization.h"

#include "context/PersonalizationContext.h"

#include <QJsonDocument>
#include <QUrlQuery>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


StatePreparePersonalization::StatePreparePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericProviderCommunication(pContext)
{
}


QUrl StatePreparePersonalization::getRequestUrl() const
{
	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	return context->getAppletServiceUrl(QStringLiteral("preparePersonalization"));
}


QByteArray StatePreparePersonalization::getPayload() const
{
	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	QJsonObject preparePersonalizationRequest;
	preparePersonalizationRequest[QLatin1String("sessionID")] = context->getSessionIdentifier().toString(QUuid::WithoutBraces);
	preparePersonalizationRequest[QLatin1String("preparePersonalizationData")] = context->getPreparePersonalizationData();

	return QJsonDocument(preparePersonalizationRequest).toJson(QJsonDocument::Compact);
}


bool StatePreparePersonalization::parseStatusCode(const QByteArray& pData) const
{
	QJsonParseError jsonError {};
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qDebug() << "JSON parsing failed:" << jsonError.errorString();
		return false;
	}

	const auto obj = json.object();
	if (!obj.contains(QLatin1String("statusCode")))
	{
		qDebug() << "JSON parsing failed: statusCode is missing";
		return false;
	}

	const auto statusCode = obj.value(QLatin1String("statusCode"));
	if (!statusCode.isDouble())
	{
		qDebug() << "JSON parsing failed: statusCode has wrong format";
		return false;
	}

	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	context->setFinalizeStatus(statusCode.toInt());
	return true;
}


void StatePreparePersonalization::handleNetworkReply(const QByteArray& pContent)
{
	if (parseStatusCode(pContent))
	{
		const auto& statusCode = qobject_cast<PersonalizationContext*>(getContext())->getFinalizeStatus();
		if (statusCode < 0)
		{
			qWarning() << "preparePersonalization failed with statusCode" << statusCode;
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Smart_PrePersonalization_Wrong_Status);
			return;
		}

		qDebug() << "preparePersonalization finished with statusCode" << statusCode;
		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "No valid network response";
	updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
	Q_EMIT fireAbort(FailureCode::Reason::Smart_PrePersonalization_Incomplete_Information);
}
