/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TlsChecker.h"

#include "AppSettings.h"
#include "SecureStorage.h"

#include <QCryptographicHash>
#include <QLoggingCategory>
#include <algorithm>

Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


bool TlsChecker::checkCertificate(const QSslCertificate& pCertificate,
		QCryptographicHash::Algorithm pAlgorithm,
		const QSet<QString>& pAcceptedCertificateHashes)
{
	qDebug() << "Check certificate CN=" << pCertificate.subjectInfo(QSslCertificate::CommonName) << "SN=" << pCertificate.serialNumber();
	QString hash = QString::fromLatin1(pCertificate.digest(pAlgorithm).toHex());
	qDebug() << "Certificate hash(" << pAlgorithm << ")" << hash;
	qDebug() << "Accepted certificate hashes" << pAcceptedCertificateHashes;

	return std::any_of(pAcceptedCertificateHashes.constBegin(), pAcceptedCertificateHashes.constEnd(), [&hash](const auto& pAcceptedHash)
		{
			return hash.compare(pAcceptedHash, Qt::CaseInsensitive) == 0;
		});
}


bool TlsChecker::hasValidCertificateKeyLength(const QSslCertificate& pCertificate)
{
	if (pCertificate.isNull())
	{
		qDebug() << "Certificate is null";
		return false;
	}

	auto keyLength = pCertificate.publicKey().length();
	auto keyAlgorithm = pCertificate.publicKey().algorithm();
	qDebug() << "Check certificate key of type" << TlsChecker::toString(keyAlgorithm) << "and key size" << keyLength;
	return isValidKeyLength(keyLength, keyAlgorithm, false);
}


bool TlsChecker::hasValidEphemeralKeyLength(const QSslKey& pEphemeralServerKey)
{
	int keyLength = pEphemeralServerKey.length();
	QSsl::KeyAlgorithm keyAlgorithm = pEphemeralServerKey.algorithm();

	if (keyAlgorithm == QSsl::Opaque)
	{
		qWarning() << "Qt failed to determine algorithm";
	}

	qDebug() << "Check ephemeral key of type" << TlsChecker::toString(keyAlgorithm) << "and key size" << keyLength;
	return isValidKeyLength(keyLength, keyAlgorithm, true);
}


QString TlsChecker::getCertificateIssuerName(const QSslCertificate& pCertificate)
{
	const auto& issuerNameList = pCertificate.issuerInfo(QSslCertificate::CommonName);
	if (!issuerNameList.isEmpty())
	{
		return issuerNameList.first();
	}
	return QString();
}


bool TlsChecker::isValidKeyLength(int pKeyLength, QSsl::KeyAlgorithm pKeyAlgorithm, bool pIsEphemeral)
{
	const auto* secureStorage = Env::getSingleton<SecureStorage>();
	const int minKeySize = pIsEphemeral ? secureStorage->getMinimumEphemeralKeySize(pKeyAlgorithm) : secureStorage->getMinimumStaticKeySize(pKeyAlgorithm);

	qDebug() << "Minimum requested key size" << minKeySize;

	bool sufficient = pKeyLength >= minKeySize;
	if (!sufficient)
	{
		auto keySizeError = QStringLiteral("%1 key with insufficient key size found %2").arg(toString(pKeyAlgorithm)).arg(pKeyLength);
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCWarning(developermode).noquote() << keySizeError;
			sufficient = true;
		}
		else
		{
			qWarning().noquote() << keySizeError;
		}
	}
	return sufficient;
}


QString TlsChecker::toString(QSsl::SslProtocol pProtocol)
{
	switch (pProtocol)
	{
		case QSsl::SslProtocol::AnyProtocol:
			return QStringLiteral("AnyProtocol");

		case QSsl::SslProtocol::SecureProtocols:
			return QStringLiteral("SecureProtocols");

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0)) || QT_DEPRECATED_SINCE(5, 15) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		case QSsl::SslProtocol::SslV2:
			return QStringLiteral("SslV2");

		case QSsl::SslProtocol::SslV3:
			return QStringLiteral("SslV3");

		case QSsl::SslProtocol::TlsV1SslV3:
			return QStringLiteral("TlsV1SslV3");

#endif

			QT_WARNING_PUSH
					QT_WARNING_DISABLE_DEPRECATED

		case QSsl::SslProtocol::TlsV1_0:
			return QStringLiteral("TlsV1_0");

		case QSsl::SslProtocol::TlsV1_0OrLater:
			return QStringLiteral("TlsV1_0OrLater");

		case QSsl::SslProtocol::TlsV1_1:
			return QStringLiteral("TlsV1_1");

		case QSsl::SslProtocol::TlsV1_1OrLater:
			return QStringLiteral("TlsV1_1OrLater");

		case QSsl::SslProtocol::DtlsV1_0:
			return QStringLiteral("DtlsV1_0");

		case QSsl::SslProtocol::DtlsV1_0OrLater:
			return QStringLiteral("DtlsV1_0OrLater");

		case QSsl::SslProtocol::DtlsV1_2:
			return QStringLiteral("DtlsV1_2");

		case QSsl::SslProtocol::DtlsV1_2OrLater:
			return QStringLiteral("DtlsV1_2OrLater");

			QT_WARNING_POP

		case QSsl::SslProtocol::TlsV1_2:
			return QStringLiteral("TlsV1_2");

		case QSsl::SslProtocol::TlsV1_2OrLater:
			return QStringLiteral("TlsV1_2OrLater");

		case QSsl::SslProtocol::TlsV1_3:
			return QStringLiteral("TlsV1_3");

		case QSsl::SslProtocol::TlsV1_3OrLater:
			return QStringLiteral("TlsV1_3OrLater");

		case QSsl::SslProtocol::UnknownProtocol:
			return QStringLiteral("UnknownProtocol");
	}

	Q_UNREACHABLE();
}


QString TlsChecker::toString(QSsl::KeyAlgorithm pKeyAlgorithm)
{
	switch (pKeyAlgorithm)
	{
		case QSsl::KeyAlgorithm::Opaque:
			return QStringLiteral("Opaque");

		case QSsl::KeyAlgorithm::Dsa:
			return QStringLiteral("Dsa");

		case QSsl::KeyAlgorithm::Rsa:
			return QStringLiteral("Rsa");

		case QSsl::KeyAlgorithm::Ec:
			return QStringLiteral("Ec");

		case QSsl::KeyAlgorithm::Dh:
			return QStringLiteral("Dh");
	}

	return QStringLiteral("Unknown (%1)").arg(pKeyAlgorithm);
}


QStringList TlsChecker::getFatalErrors(const QList<QSslError>& pErrors)
{
	static const QSet<QSslError::SslError> fatalErrors(
		{
			QSslError::CertificateSignatureFailed, QSslError::CertificateNotYetValid, QSslError::CertificateExpired,
			QSslError::InvalidNotBeforeField, QSslError::InvalidNotAfterField, QSslError::CertificateRevoked, QSslError::InvalidCaCertificate,
			QSslError::CertificateRejected, QSslError::SubjectIssuerMismatch, QSslError::HostNameMismatch,
			QSslError::UnspecifiedError, QSslError::NoSslSupport, QSslError::CertificateBlacklisted
		});

	QStringList fatalErrorStrings;
	for (const auto& error : pErrors)
	{
		const auto& msg = error.errorString();
		if (!fatalErrors.contains(error.error()))
		{
			qCDebug(network) << "(ignored)" << msg;
			continue;
		}

		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCWarning(developermode) << msg;
			if (!error.certificate().isNull())
			{
				qCWarning(developermode) << error.certificate();
			}
		}
		else
		{
			qCWarning(network) << msg;
			if (!error.certificate().isNull())
			{
				qCWarning(network) << error.certificate();
			}
			fatalErrorStrings += msg;
		}
	}

	return fatalErrorStrings;
}


bool TlsChecker::containsFatalError(const QSharedPointer<QNetworkReply>& pReply, const QList<QSslError>& pErrors)
{
	if (pReply.isNull())
	{
		qCCritical(network) << "Reply should not be nullptr";
		Q_ASSERT(pReply);
		return false;
	}

	if (getFatalErrors(pErrors).isEmpty())
	{
		qCDebug(network) << "Ignore SSL errors";
		pReply->ignoreSslErrors(pErrors);
		return false;
	}

	return true;
}


QString TlsChecker::sslErrorsToString(const QList<QSslError>& pErrors)
{
	QStringList errorString;
	for (const auto& error : pErrors)
	{
		errorString += error.errorString();
	}
	return errorString.join(QLatin1Char(','));
}


void TlsChecker::logSslConfig(const QSslConfiguration& pCfg, const MessageLogger& pLogger)
{
	pLogger.info() << "Used session cipher" << pCfg.sessionCipher();
	pLogger.info() << "Used session protocol:" << toString(pCfg.sessionProtocol());

	{
		auto stream = pLogger.info();
		stream << "Used ephemeral server key:";
		if (!pCfg.ephemeralServerKey().isNull())
		{
			stream << pCfg.ephemeralServerKey();
		}
	}

	pLogger.info() << "Used peer certificate:" << pCfg.peerCertificate();

	auto session = pCfg.sessionTicket();
	if (!session.isEmpty())
	{
		// do not print session ticket as plain text
		session = QCryptographicHash::hash(session, QCryptographicHash::Sha256).toHex();
	}
	pLogger.info() << "Used ssl session:" << session;
	pLogger.info() << "Handshake of tls connection done!";
}
