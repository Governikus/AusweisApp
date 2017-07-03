/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "TlsConfiguration.h"

#include <QDate>
#include <QLoggingCategory>
#include <QStringList>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)

QStringList TlsConfiguration::getFatalErrors(const QList<QSslError>& pErrors)
{
	static const QSet<QSslError::SslError> fatalErrors(
		{QSslError::CertificateSignatureFailed, QSslError::CertificateNotYetValid, QSslError::CertificateExpired,
		 QSslError::InvalidNotBeforeField, QSslError::InvalidNotAfterField, QSslError::CertificateRevoked, QSslError::InvalidCaCertificate,
		 QSslError::CertificateRejected, QSslError::SubjectIssuerMismatch, QSslError::HostNameMismatch,
		 QSslError::UnspecifiedError, QSslError::NoSslSupport, QSslError::CertificateBlacklisted}
		);

	QStringList fatalErrorStrings;
	for (const auto& error : pErrors)
	{
		QString msg = QStringLiteral("%1: %2").arg(static_cast<int>(error.error())).arg(error.errorString());
		if (fatalErrors.contains(error.error()))
		{
			if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
			{
				qCWarning(developermode) << msg;
			}
			else
			{
				qCWarning(network) << msg;
				fatalErrorStrings += msg;
			}
		}
		else
		{
			qCWarning(network) << "(ignored) " << msg;
		}
	}
	return fatalErrorStrings;
}


bool TlsConfiguration::containsFatalError(QNetworkReply* pReply, const QList<QSslError>& pErrors)
{
	Q_ASSERT(pReply);

	if (getFatalErrors(pErrors).isEmpty())
	{
		qCDebug(network) << "Ignore SSL errors";
		pReply->ignoreSslErrors();
		return false;
	}

	return true;
}
