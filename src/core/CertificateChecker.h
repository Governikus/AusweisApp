/*!
 * \brief Contains the definition of the CertificateChecker class.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"
#include "paos/retrieve/DidAuthenticateEac1.h"

#include <QSet>
#include <QSslCertificate>
#include <QString>

#include <functional>

namespace governikus
{

/*!
 * \brief Utility class for checking various constraints on certificates
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */
class CertificateChecker
{
	Q_GADGET

	public:
		enum class CertificateStatus
		{
			Good,
			Unsupported_Algorithm_Or_Length,
			Hash_Not_In_Description
		};
		Q_ENUM(CertificateStatus)


		/*!
		 * Checks certificate and, if OK, save it using a callback function.
		 *
		 * \return Returns a translated error string if an error happened, otherwise QString()
		 */
		static CertificateStatus checkAndSaveCertificate(const QSslCertificate& pCertificate,
				const QUrl& pUrl,
				const QSharedPointer<DIDAuthenticateEAC1>& pEAC1,
				const QSharedPointer<const CVCertificate>& pDvCvc,
				const std::function<void(const QUrl&, const QSslCertificate&)>& pSaveCertificateFunc);
};

} // namespace governikus
