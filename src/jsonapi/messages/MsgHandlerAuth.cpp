/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerAuth.h"

#include "InternalActivationHandler.h"

#include <QUrlQuery>

using namespace governikus;


MsgHandlerAuth::MsgHandlerAuth()
	: MsgHandler(MsgType::AUTH)
{
}


MsgHandlerAuth::MsgHandlerAuth(const QJsonObject& pObj)
	: MsgHandlerAuth()
{
	const auto& jsonTcTokenUrl = pObj["tcTokenURL"];
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
		const auto& url = createUrl(jsonTcTokenUrl.toString());
		if (url.isValid())
		{
			initAuth(url);
			setVoid();
			return;
		}
		Q_ASSERT(mJsonObject["error"].isString());
	}
}


MsgHandlerAuth::MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext)
	: MsgHandlerAuth()
{
	Q_ASSERT(pContext);

	mJsonObject["result"] = pContext->getResult().toJson();

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
	const QUrl parsedUrl(pUrl);
	if (parsedUrl.isValid() && parsedUrl.scheme() == QLatin1String("https") && !parsedUrl.host().isEmpty())
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
	auto handler = ActivationHandler::getInstance<InternalActivationHandler>();
	Q_ASSERT(handler);
	handler->runAuthentication(new InternalActivationContext(pTcTokenUrl));
}


void MsgHandlerAuth::setError(const QLatin1String& pError)
{
	setValue("error", pError);
}
