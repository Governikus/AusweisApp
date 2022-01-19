/*!
 * \brief Helper to convert \ref Result to Redirect-Result-String and some other URL stuff.
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ECardApiResult.h"

#include <QSslCertificate>
#include <QString>
#include <QUrl>

namespace governikus
{

/*!
 * \brief Utility class for checking various constraints on URLs.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
class UrlUtil
{
	UrlUtil() = delete;
	~UrlUtil() = delete;

	static inline QString removePrefix(QString pStr);
	static inline QString getSuffix(ECardApiResult::Minor pMinor);

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

};

} // namespace governikus
