/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper to convert \ref Result to Redirect-Result-String and some other URL stuff.
 */

#pragma once

#include "ECardApiResult.h"

#include "EnumHelper.h"

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


/*!
 * \brief Utility class for checking various constraints on URLs.
 */
class UrlUtil
{
	private:
		static inline QString removePrefix(QString pStr);
		static inline QString getSuffix(ECardApiResult::Minor pMinor);

		UrlUtil() = delete;
		~UrlUtil() = delete;

	public:
		/*!
		 * Determines the URL origin, i.e. the protocol, host name and port part of the full URL.
		 */
		static QUrl getUrlOrigin(const QUrl& pUrl);

		/*!
		 * Checks whether the same origin policy is satisfied for the two specified URL.
		 */
		static bool isMatchingSameOriginPolicy(const QUrl& pUrl1, const QUrl& pUrl2);

		/*!
		 * Append result to URL.
		 */
		static QUrl addMajorMinor(const QUrl& pUrl, const GlobalStatus& pStatus);

		static void setHiddenSettings(const QUrlQuery& pUrl);
		static QPair<UrlQueryRequest, QString> getRequest(const QUrlQuery& pUrl);

		template<typename T>
		static T prepareToEnum(const QString& pStr, T pDefault)
		{
			return Enum<T>::fromString(pStr.toUpper().replace(QLatin1Char('-'), QLatin1Char('_')), pDefault);
		}


};

} // namespace governikus
