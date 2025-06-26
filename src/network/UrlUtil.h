/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QNetworkReply>
#include <QSharedPointer>
#include <QString>
#include <QUrl>
#include <QUrlQuery>


namespace governikus
{

defineEnumType(UrlQueryRequest,
		UNKNOWN,
		SHOWUI,
		STATUS,
		TCTOKENURL
		)

class UrlUtil
{
	private:
		UrlUtil() = delete;
		~UrlUtil() = delete;

	public:
		static QUrl resolveRedirect(const QSharedPointer<QNetworkReply>& pReply);

		/*!
		 * Determines the URL origin, i.e. the protocol, host name and port part of the full URL.
		 */
		static QUrl getUrlOrigin(const QUrl& pUrl);

		/*!
		 * Checks whether the same origin policy is satisfied for the two specified URL.
		 */
		static bool isMatchingSameOriginPolicy(const QUrl& pUrl1, const QUrl& pUrl2);

		static void setHiddenSettings(const QUrlQuery& pUrl);
		static QPair<UrlQueryRequest, QString> getRequest(const QUrlQuery& pUrl);

		template<typename T>
		static T prepareToEnum(const QString& pStr, T pDefault)
		{
			return Enum<T>::fromString(pStr.toUpper().replace(QLatin1Char('-'), QLatin1Char('_')), pDefault);
		}


};

} // namespace governikus
