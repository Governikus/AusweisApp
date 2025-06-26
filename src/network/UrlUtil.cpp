/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UrlUtil.h"

#include "AppSettings.h"
#include "Env.h"

#include <QHostAddress>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QUrlQuery>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


QUrl UrlUtil::resolveRedirect(const QSharedPointer<QNetworkReply>& pReply)
{
	const QUrl& redirectUrl = pReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (redirectUrl.isEmpty() || !redirectUrl.isValid() || !redirectUrl.isRelative())
	{
		return redirectUrl;
	}

	const auto& requestUrl = pReply->request().url();
	return requestUrl.resolved(redirectUrl);
}


QUrl UrlUtil::getUrlOrigin(const QUrl& pUrl)
{
	// get default port for scheme
	int defaultPort = -1;
	QString scheme = pUrl.scheme();
	if (scheme == QLatin1String("http"))
	{
		defaultPort = 80;
	}
	else if (scheme == QLatin1String("https"))
	{
		defaultPort = 443;
	}

	QUrl origin;
	origin.setScheme(scheme);
	origin.setHost(pUrl.host());
	origin.setPort(pUrl.port(defaultPort));
	qDebug() << "Normalized URL from" << pUrl << "to" << origin;
	return origin;
}


bool UrlUtil::isMatchingSameOriginPolicy(const QUrl& pUrl1, const QUrl& pUrl2)
{
	QUrl urlOrigin1 = UrlUtil::getUrlOrigin(pUrl1);
	QUrl urlOrigin2 = UrlUtil::getUrlOrigin(pUrl2);
	bool sameOriginPolicyCheckResult = (urlOrigin1 == urlOrigin2);
	qCDebug(network) << "SOP-Check(" << urlOrigin1.toString() << "," << urlOrigin2.toString() << ")=" << sameOriginPolicyCheckResult;
	return sameOriginPolicyCheckResult;
}


void UrlUtil::setHiddenSettings(const QUrlQuery& pUrl)
{
	const auto queryUseTestUri = QLatin1String("useTestUri");
	if (pUrl.hasQueryItem(queryUseTestUri))
	{
		const auto value = pUrl.queryItemValue(queryUseTestUri);
		const bool useTestUri = QVariant(value).toBool();
		Env::getSingleton<AppSettings>()->getGeneralSettings().setUseSelfauthenticationTestUri(useTestUri);
	}

	const auto queryEnableSimulator = QLatin1String("enableSimulator");
	if (pUrl.hasQueryItem(queryEnableSimulator))
	{
		const auto value = pUrl.queryItemValue(queryEnableSimulator);
		const bool enableSimulator = QVariant(value).toBool();
		Env::getSingleton<AppSettings>()->getSimulatorSettings().setEnabled(enableSimulator);
	}
}


QPair<UrlQueryRequest, QString> UrlUtil::getRequest(const QUrlQuery& pUrl)
{
	const auto queryItems = pUrl.queryItems();
	for (const auto& [key, value] : queryItems)
	{
		const auto type = Enum<UrlQueryRequest>::fromString(key.toUpper(), UrlQueryRequest::UNKNOWN);
		if (type != UrlQueryRequest::UNKNOWN)
		{
			return {type, value};
		}
	}

	return {UrlQueryRequest::UNKNOWN, QString()};
}


#include "moc_UrlUtil.cpp"
