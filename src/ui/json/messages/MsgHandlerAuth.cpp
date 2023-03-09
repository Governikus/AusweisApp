/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerAuth.h"

#include "UILoader.h"
#include "UIPlugInJson.h"
#include "controller/AuthController.h"

#include <QSharedPointer>
#include <QUrlQuery>

using namespace governikus;


MsgHandlerAuth::MsgHandlerAuth()
	: MsgHandlerWorkflows(MsgType::AUTH)
{
}


MsgHandlerAuth::MsgHandlerAuth(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerAuth()
{
	const auto& jsonTcTokenUrl = pObj[QLatin1String("tcTokenURL")];
	if (jsonTcTokenUrl.isUndefined())
	{
		setError(QLatin1String("tcTokenURL cannot be undefined"));
	}
	else if (!jsonTcTokenUrl.isString())
	{
		setError(QLatin1String("Invalid tcTokenURL"));
	}
	else
	{
		if (const auto& url = createUrl(jsonTcTokenUrl.toString()); url.isValid())
		{
			handleWorkflowProperties(pObj, pContext);
			initAuth(url);
			setVoid();
			return;
		}
		Q_ASSERT(mJsonObject[QLatin1String("error")].isString());
	}
}


MsgHandlerAuth::MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext)
	: MsgHandlerAuth()
{
	Q_ASSERT(pContext);

	mJsonObject[QLatin1String("result")] = ECardApiResult(pContext->getStatus(), pContext->getFailureCode()).toJson();

	QString url;
	if (pContext->getRefreshUrl().isEmpty())
	{
		const auto& token = pContext->getTcToken();
		if (!token.isNull() && pContext->getTcToken()->getCommunicationErrorAddress().isValid())
		{
			url = pContext->getTcToken()->getCommunicationErrorAddress().toString();
		}
	}
	else
	{
		url = pContext->getRefreshUrl().toString();
	}

	setValue("url", url);
}


QUrl MsgHandlerAuth::createUrl(const QString& pUrl)
{
	if (const QUrl parsedUrl(pUrl); parsedUrl.isValid() && !parsedUrl.host().isEmpty())
	{
		QUrlQuery query;
		query.addQueryItem(QStringLiteral("tcTokenURL"), QString::fromLatin1(QUrl::toPercentEncoding(pUrl)));

		QUrl url(QStringLiteral("http://localhost/")); // just a dummy for StateParseTcTokenUrl
		url.setQuery(query);

		return url;
	}

	setError(QLatin1String("Validation of tcTokenURL failed"));
	return QUrl();
}


void MsgHandlerAuth::initAuth(const QUrl& pTcTokenUrl)
{
	auto* ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
	Q_ASSERT(ui);
	Q_EMIT ui->fireWorkflowRequested(AuthController::createWorkflowRequest(pTcTokenUrl));
}
