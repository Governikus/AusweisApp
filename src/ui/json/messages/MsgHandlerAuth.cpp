/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerAuth.h"

#include "UILoader.h"
#include "UIPlugInJson.h"
#include "VolatileSettings.h"

#include <QSharedPointer>
#include <QUrlQuery>

using namespace governikus;


MsgHandlerAuth::MsgHandlerAuth()
	: MsgHandler(MsgType::AUTH)
{
}


MsgHandlerAuth::MsgHandlerAuth(const QJsonObject& pObj)
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
		const auto& url = createUrl(jsonTcTokenUrl.toString());
		if (url.isValid())
		{
			initMessages(pObj[QLatin1String("messages")].toObject());
			initDeveloperMode(pObj[QLatin1String("developerMode")]);
			initHandleInterrupt(pObj[QLatin1String("handleInterrupt")]);
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

	mJsonObject[QLatin1String("result")] = ECardApiResult(pContext->getStatus()).toJson();

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
	if (parsedUrl.isValid() && !parsedUrl.host().isEmpty())
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


void MsgHandlerAuth::initMessages(const QJsonObject& pUi)
{
	if (!pUi.isEmpty())
	{
		const VolatileSettings::Messages messages(pUi[QLatin1String("sessionStarted")].toString(),
				pUi[QLatin1String("sessionFailed")].toString(),
				pUi[QLatin1String("sessionSucceeded")].toString(),
				pUi[QLatin1String("sessionInProgress")].toString());

		Env::getSingleton<VolatileSettings>()->setMessages(messages);
	}
}


void MsgHandlerAuth::initHandleInterrupt(const QJsonValue& pValue)
{
	if (pValue.isBool())
	{
		Env::getSingleton<VolatileSettings>()->setHandleInterrupt(pValue.toBool());
	}
}


void MsgHandlerAuth::initDeveloperMode(const QJsonValue& pValue)
{
	if (pValue.isBool())
	{
		Env::getSingleton<VolatileSettings>()->setDeveloperMode(pValue.toBool());
		qDebug() << "Using Developer Mode on SDK:" << Env::getSingleton<VolatileSettings>()->isDeveloperMode();
	}
}


void MsgHandlerAuth::initAuth(const QUrl& pTcTokenUrl)
{
	auto mJson = Env::getSingleton<UILoader>()->getLoaded(UIPlugInName::UIPlugInJson);
	Q_EMIT mJson->fireAuthenticationRequest(pTcTokenUrl);
}


void MsgHandlerAuth::setError(const QLatin1String pError)
{
	setValue("error", pError);
}
