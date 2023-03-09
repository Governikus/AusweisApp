/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePersonalization.h"

#include "DeviceInfo.h"
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


void StatePreparePersonalization::handleNetworkReply(const QByteArray& pContent)
{
	Q_UNUSED(pContent)
	Q_EMIT fireContinue();
}
