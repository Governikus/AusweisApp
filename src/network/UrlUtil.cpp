/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "UrlUtil.h"

#include <QHostAddress>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QUrlQuery>


Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


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


QString UrlUtil::removePrefix(QString pStr)
{
	return pStr.replace(QRegularExpression(QStringLiteral("(.*)#")), QLatin1String(""));
}


QUrl UrlUtil::addMajorMinor(const QUrl& pOriginUrl, const Result& pResult)
{
	QString major = removePrefix(pResult.getMajorString());

	QUrlQuery q;
	q.setQuery(pOriginUrl.query());
	q.addQueryItem(QStringLiteral("ResultMajor"), major);

	if (pResult.getMinor() != Result::Minor::null)
	{
		QString minor;

		if (pResult.isOriginServer())
		{
			minor = QStringLiteral("serverError");
		}
		else if (pResult.getMinor() == Result::Minor::AL_Communication_Error ||
				pResult.getMinor() == Result::Minor::DP_Trusted_Channel_Establishment_Failed ||
				pResult.getMinor() == Result::Minor::SAL_Cancellation_by_User)
		{
			minor = removePrefix(pResult.getMinorString());
		}
		else
		{
			minor = QStringLiteral("clientError");
		}

		q.addQueryItem(QStringLiteral("ResultMinor"), minor);
	}

	QUrl adaptedUrl(pOriginUrl);
	adaptedUrl.setQuery(q);
	return adaptedUrl;
}
