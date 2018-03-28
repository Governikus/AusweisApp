/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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


QUrl UrlUtil::addMajorMinor(const QUrl& pOriginUrl, const GlobalStatus& pStatus)
{
	const Result result(pStatus);
	const QString& major = removePrefix(result.getMajorString());

	QUrlQuery q;
	q.setQuery(pOriginUrl.query());
	q.addQueryItem(QStringLiteral("ResultMajor"), major);

	if (result.getMinor() != GlobalStatus::Code::No_Error)
	{
		QString minor;

		if (result.isOriginServer())
		{
			minor = QStringLiteral("serverError");
		}
		else if (result.getMinor() == GlobalStatus::Code::Paos_Error_AL_Communication_Error ||
				result.getMinor() == GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed ||
				result.getMinor() == GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User)
		{
			minor = removePrefix(result.getMinorString());
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
