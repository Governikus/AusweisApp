/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Contains the definition of the CertificateChecker class.
 */

#pragma once

#include "GlobalStatus.h"
#include "context/AuthContext.h"

#include <QSet>
#include <QSslCertificate>
#include <QString>

#include <functional>

namespace governikus
{

class CertificateChecker
{
	Q_GADGET

	private:
		static bool abortOnError();

	public:
		enum class CertificateStatus
		{
			Good,
			Unsupported_Algorithm_Or_Length,
			Hash_Not_In_Description
		};
		Q_ENUM(CertificateStatus)

		static GlobalStatus::Code getGlobalStatus(CertificateStatus pStatus, bool pPaos);

		static CertificateStatus checkAndSaveCertificate(const QSslCertificate& pCertificate,
				const QUrl& pUrl,
				const QSharedPointer<AuthContext> pContext);
};

} // namespace governikus
