/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "TlsChecker.h"

#include "AppSettings.h"
#include "SecureStorage.h"

#include <QCryptographicHash>
#include <QLoggingCategory>

#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
	#include <openssl/dh.h>
	#include <openssl/dsa.h>
	#include <openssl/ec.h>
	#include <openssl/evp.h>
	#include <openssl/rsa.h>
#endif

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
	for (const auto& acceptedHash : pAcceptedCertificateHashes)
	{
		if (hash.compare(acceptedHash, Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		qWarning() << "Qt failed to determine algorithm";
#else
		qWarning() << "Qt failed to determine algorithm... fallback to internal handling";
		// try do determine key algorithm and length
		if (auto key = static_cast<EVP_PKEY*>(pEphemeralServerKey.handle()))
		{
			keyLength = EVP_PKEY_bits(key);
			switch (EVP_PKEY_base_id(key))
			{
				case EVP_PKEY_RSA:
					keyAlgorithm = QSsl::Rsa;
					break;

				case EVP_PKEY_DSA:
				/* fall through */
				case EVP_PKEY_DH:
					keyAlgorithm = QSsl::Dsa;
					break;

				case EVP_PKEY_EC:
					keyAlgorithm = QSsl::Ec;
					break;
			}
		}
#endif
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

		case QSsl::SslProtocol::SslV2:
			return QStringLiteral("SslV2");

		case QSsl::SslProtocol::SslV3:
			return QStringLiteral("SslV3");

		case QSsl::SslProtocol::TlsV1SslV3:
			return QStringLiteral("TlsV1SslV3");

		case QSsl::SslProtocol::TlsV1_0:
			return QStringLiteral("TlsV1_0");

		case QSsl::SslProtocol::TlsV1_0OrLater:
			return QStringLiteral("TlsV1_0OrLater");

		case QSsl::SslProtocol::TlsV1_1:
			return QStringLiteral("TlsV1_1");

		case QSsl::SslProtocol::TlsV1_1OrLater:
			return QStringLiteral("TlsV1_1OrLater");

		case QSsl::SslProtocol::TlsV1_2:
			return QStringLiteral("TlsV1_2");

		case QSsl::SslProtocol::TlsV1_2OrLater:
			return QStringLiteral("TlsV1_2OrLater");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
		case QSsl::SslProtocol::TlsV1_3:
			return QStringLiteral("TlsV1_3");

		case QSsl::SslProtocol::TlsV1_3OrLater:
			return QStringLiteral("TlsV1_3OrLater");

		case QSsl::SslProtocol::DtlsV1_0:
			return QStringLiteral("DtlsV1_0");

		case QSsl::SslProtocol::DtlsV1_0OrLater:
			return QStringLiteral("DtlsV1_0OrLater");

		case QSsl::SslProtocol::DtlsV1_2:
			return QStringLiteral("DtlsV1_2");

		case QSsl::SslProtocol::DtlsV1_2OrLater:
			return QStringLiteral("DtlsV1_2OrLater");

#endif
		case QSsl::SslProtocol::UnknownProtocol:
			return QStringLiteral("UnknownProtocol");
	}

	Q_UNREACHABLE();
	return QString();
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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		case QSsl::KeyAlgorithm::Dh:
			return QStringLiteral("Dh");

#endif
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
		if (fatalErrors.contains(error.error()))
		{
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
		else
		{
			qCDebug(network) << "(ignored)" << msg;
		}
	}
	return fatalErrorStrings;
}


bool TlsChecker::containsFatalError(QNetworkReply* pReply, const QList<QSslError>& pErrors)
{
	if (pReply == nullptr)
	{
		qCCritical(network) << "Reply should not be nullptr";
		Q_ASSERT(pReply);
		return false;
	}

	if (getFatalErrors(pErrors).isEmpty())
	{
		qCDebug(network) << "Ignore SSL errors";
		pReply->ignoreSslErrors();
		return false;
	}

	return true;
}


void TlsChecker::logSslConfig(const QSslConfiguration& pCfg, const QMessageLogger& pLogger)
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
