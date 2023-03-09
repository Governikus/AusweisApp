/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */


#include "UrlUtil.h"

#include "AppSettings.h"
#include "Env.h"
#include "Initializer.h"

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


QString UrlUtil::removePrefix(QString pStr)
{
	return pStr.replace(QRegularExpression(QStringLiteral("(.*)#")), QLatin1String(""));
}


QString UrlUtil::getSuffix(ECardApiResult::Minor pMinor)
{
	return removePrefix(ECardApiResult::getMinorString(pMinor));
}


QUrl UrlUtil::addMajorMinor(const QUrl& pOriginUrl, const GlobalStatus& pStatus)
{
	QUrlQuery q;
	q.setQuery(pOriginUrl.query());

	const ECardApiResult::Major majorEnumVal = pStatus.isError() ? ECardApiResult::Major::Error : ECardApiResult::Major::Ok;
	QString major = removePrefix(ECardApiResult::getMajorString(majorEnumVal));
	q.addQueryItem(QStringLiteral("ResultMajor"), major);

	if (pStatus.isError())
	{
		QString minor;

		switch (pStatus.getStatusCode())
		{
			case GlobalStatus::Code::Paos_Error_AL_Communication_Error:
				minor = getSuffix(ECardApiResult::Minor::AL_Communication_Error);
				break;

			case GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
				minor = getSuffix(ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed);
				break;

			case GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User:
				minor = getSuffix(ECardApiResult::Minor::SAL_Cancellation_by_User);
				break;

			default:
				if (pStatus.isOriginServer())
				{
					minor = QStringLiteral("serverError");
				}
				else
				{
					minor = QStringLiteral("clientError");
				}
				break;
		}
		q.addQueryItem(QStringLiteral("ResultMinor"), minor);
	}

	QUrl adaptedUrl(pOriginUrl);
	adaptedUrl.setQuery(q);
	return adaptedUrl;
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
		Env::getSingleton<AppSettings>()->getGeneralSettings().setSimulatorEnabled(enableSimulator);
	}
}


QPair<UrlQueryRequest, QString> UrlUtil::getRequest(const QUrlQuery& pUrl)
{
	const auto queryItems = pUrl.queryItems();
	for (const auto& item : queryItems)
	{
		const auto type = Enum<UrlQueryRequest>::fromString(item.first.toUpper(), UrlQueryRequest::UNKNOWN);
		if (type != UrlQueryRequest::UNKNOWN)
		{
			return {type, item.second};
		}
	}

	return {UrlQueryRequest::UNKNOWN, QString()};
}


#include "moc_UrlUtil.cpp"
