/*!
 * SecureStorage.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "FileDestination.h"
#include "SecureStorage.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMetaEnum>
#include <QTextStream>


using namespace governikus;

const QLatin1String SETTINGS_GROUP_NAME_CV_ROOT_CERTIFICATE("cvRootCertificates");
const QLatin1String SETTINGS_GROUP_NAME_CV_ROOT_CERTIFICATE_TEST("cvRootCertificatesTest");

const QLatin1String SETTINGS_GROUP_NAME_UPDATE_CERTIFICATES("updateCertificates");

const QLatin1String SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS("signatureAlgorithms");
const QLatin1String SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS_PSK("signatureAlgorithmsPsk");

const QLatin1String SETTINGS_GROUP_NAME_ALLOWED_ECS("ecCiphers");
const QLatin1String SETTINGS_GROUP_NAME_PSK_CIPHERS("pskCiphers");
const QLatin1String SETTINGS_GROUP_NAME_FS_CIPHERS("fsCiphers");
const QLatin1String SETTINGS_GROUP_NAME_FS_CIPHERS_WITH_BACKWARD_COMPATIBILITY("fsCiphersWithBc");
const QLatin1String SETTINGS_NAME_SSL_PROTOCOL_VERSION("sslProtocolVersion");
const QLatin1String SETTINGS_NAME_SSL_PROTOCOL_VERSION_PSK("sslProtocolVersionPsk");

const QLatin1String SETTINGS_GROUP_NAME_SELF_AUTHENTICATION("selfAuthentication");
const QLatin1String SETTINGS_NAME_SELF_AUTHENTICATION_URL("url");
const QLatin1String SETTINGS_NAME_SELF_AUTHENTICATION_TEST_URL("testUrl");
const QLatin1String SETTINGS_NAME_SELF_AUTHENTICATION_CERTDESCR("certDescr");
const QLatin1String SETTINGS_NAME_SELF_AUTHENTICATION_TEST_CERTDESCR("testCertDescr");

const QLatin1String SETTINGS_GROUP_NAME_DRIVERS("drivers");
const QLatin1String SETTINGS_NAME_DRIVERS_UPDATE_URL("updateUrl");

const QLatin1String SETTINGS_GROUP_NAME_PROVIDERS("providers");
const QLatin1String SETTINGS_NAME_PROVIDERS_UPDATE_URL("updateUrl");
const QLatin1String SETTINGS_NAME_PROVIDERS_ICONS_UPDATE_URL_BASE("iconsUpdateUrlBase");

const QLatin1String SETTINGS_GROUP_NAME_UPDATES("updates");
const QLatin1String SETTINGS_NAME_APPCAST_UPDATE_URL("release");
const QLatin1String SETTINGS_NAME_APPCAST_BETA_UPDATE_URL("beta");

SecureStorage::SecureStorage()
	: mLoadedTime()
	, mCvcas()
	, mCvcasTest()
	, mUpdateCertificates()
	, mSelfAuthenticationUrl()
	, mSelfAuthenticationTestUrl()
	, mDriverUpdateUrl()
	, mProviderUpdateUrl()
	, mProviderIconUpdateUrlBase()
	, mSelfAuthenticationCertDescr()
	, mSelfAuthenticationTestCertDescr()
	, mAppcastUpdateUrl()
	, mAppcastBetaUpdateUrl()
	, mAllowedSslEllipticCurves()
	, mPskCiphers()
	, mCiphersWithForwardSecrecy()
	, mCiphersWithSha1ForBackwardCompatibility()
	, mProtocolVersion(QSsl::SslProtocol::SecureProtocols)
	, mProtocolVersionPsk(QSsl::SslProtocol::SecureProtocols)
	, mSignatureAlgorithms()
	, mSignatureAlgorithmsPsk()
{
}


SecureStorage::~SecureStorage()
{
}


void SecureStorage::load()
{
	const auto& path = FileDestination::getPath("config.json");
	if (!QFile::exists(path))
	{
		qCritical() << "SecureStorage not found";
		return;
	}

	const auto& lastModified = QFileInfo(path).lastModified();
	if (lastModified.isValid() && lastModified <= mLoadedTime)
	{
		return;
	}

	QFile configFile(path);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical() << "Wasn't able to open SecureStorage";
		return;
	}

	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(configFile.readAll(), &parseError);
	configFile.close();
	if (parseError.error != QJsonParseError::NoError)
	{
		qCritical() << "Parse error while reading SecureStorage on position " << parseError.offset << ": " << parseError.errorString();
		return;
	}
	QJsonObject config = document.object();

	mCvcas.clear();
	readByteArrayList(mCvcas, config, SETTINGS_GROUP_NAME_CV_ROOT_CERTIFICATE);

	mCvcasTest.clear();
	readByteArrayList(mCvcasTest, config, SETTINGS_GROUP_NAME_CV_ROOT_CERTIFICATE_TEST);

	QByteArrayList certificates;
	readByteArrayList(certificates, config, SETTINGS_GROUP_NAME_UPDATE_CERTIFICATES);
	mUpdateCertificates.clear();
	for (int i = 0; i < certificates.size(); ++i)
	{
		QSslCertificate certificate(QByteArray::fromHex(certificates[i]), QSsl::Der);
		if (certificate.isNull())
		{
			qCritical() << "Failed to read update certificate[" << i << "] from SecureStorage";
			continue;
		}
		mUpdateCertificates += certificate;
	}

	mAllowedSslEllipticCurves.clear();
	QJsonArray allowedEcs;
	if (readJsonArray(allowedEcs, config, SETTINGS_GROUP_NAME_ALLOWED_ECS))
	{
		for (const QJsonValue& line : qAsConst(allowedEcs))
		{
			mAllowedSslEllipticCurves += line.toString();
		}
	}

	mPskCiphers.clear();
	QJsonArray pskCiphers;
	if (readJsonArray(pskCiphers, config, SETTINGS_GROUP_NAME_PSK_CIPHERS))
	{
		for (const QJsonValue& line : qAsConst(pskCiphers))
		{
			mPskCiphers += line.toString();
		}
	}

	mCiphersWithForwardSecrecy.clear();
	QJsonArray fsCiphers;
	if (readJsonArray(fsCiphers, config, SETTINGS_GROUP_NAME_FS_CIPHERS))
	{
		for (const QJsonValue& line : qAsConst(fsCiphers))
		{
			mCiphersWithForwardSecrecy += line.toString();
		}
	}

	mCiphersWithSha1ForBackwardCompatibility.clear();
	QJsonArray fsCiphersWithBc;
	if (readJsonArray(fsCiphersWithBc, config, SETTINGS_GROUP_NAME_FS_CIPHERS_WITH_BACKWARD_COMPATIBILITY))
	{
		for (const QJsonValue& line : qAsConst(fsCiphersWithBc))
		{
			mCiphersWithSha1ForBackwardCompatibility += line.toString();
		}
	}
	mProtocolVersion = readSslProtocol(config, SETTINGS_NAME_SSL_PROTOCOL_VERSION);
	mProtocolVersionPsk = readSslProtocol(config, SETTINGS_NAME_SSL_PROTOCOL_VERSION_PSK);

	mSignatureAlgorithms = readSignatureAlgorithms(config, SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS);
	if (mSignatureAlgorithms.isEmpty())
	{
		qWarning() << "Using default for" << SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS;
	}
	mSignatureAlgorithmsPsk = readSignatureAlgorithms(config, SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS_PSK);
	if (mSignatureAlgorithmsPsk.isEmpty())
	{
		qWarning() << "Using default for" << SETTINGS_GROUP_NAME_SIGNATURE_ALGORITHMS_PSK;
	}

	mSelfAuthenticationUrl = readGroup(config, SETTINGS_GROUP_NAME_SELF_AUTHENTICATION, SETTINGS_NAME_SELF_AUTHENTICATION_URL);
	mSelfAuthenticationTestUrl = readGroup(config, SETTINGS_GROUP_NAME_SELF_AUTHENTICATION, SETTINGS_NAME_SELF_AUTHENTICATION_TEST_URL);
	mSelfAuthenticationCertDescr = readGroup(config, SETTINGS_GROUP_NAME_SELF_AUTHENTICATION, SETTINGS_NAME_SELF_AUTHENTICATION_CERTDESCR).toLatin1();
	mSelfAuthenticationTestCertDescr = readGroup(config, SETTINGS_GROUP_NAME_SELF_AUTHENTICATION, SETTINGS_NAME_SELF_AUTHENTICATION_TEST_CERTDESCR).toLatin1();

	mDriverUpdateUrl = readGroup(config, SETTINGS_GROUP_NAME_DRIVERS, SETTINGS_NAME_DRIVERS_UPDATE_URL);

	mProviderUpdateUrl = readGroup(config, SETTINGS_GROUP_NAME_PROVIDERS, SETTINGS_NAME_PROVIDERS_UPDATE_URL);
	mProviderIconUpdateUrlBase = readGroup(config, SETTINGS_GROUP_NAME_PROVIDERS, SETTINGS_NAME_PROVIDERS_ICONS_UPDATE_URL_BASE);

	mAppcastUpdateUrl = readGroup(config, SETTINGS_GROUP_NAME_UPDATES, SETTINGS_NAME_APPCAST_UPDATE_URL);
	mAppcastBetaUpdateUrl = readGroup(config, SETTINGS_GROUP_NAME_UPDATES, SETTINGS_NAME_APPCAST_BETA_UPDATE_URL);

	mLoadedTime = lastModified;
	qInfo() << "SecureStorage successfully loaded";
}


const QByteArrayList& SecureStorage::getCVRootCertificates(bool pProductive) const
{
	return pProductive ? mCvcas : mCvcasTest;
}


const QVector<QSslCertificate>& SecureStorage::getUpdateCertificates() const
{
	return mUpdateCertificates;
}


const QUrl& SecureStorage::getSelfAuthenticationUrl(bool pTest) const
{
	return pTest ? mSelfAuthenticationTestUrl : mSelfAuthenticationUrl;
}


const QByteArray& SecureStorage::getSelfAuthenticationCertDescr(bool pTest) const
{
	return pTest ? mSelfAuthenticationTestCertDescr : mSelfAuthenticationCertDescr;
}


const QUrl& SecureStorage::getDriverUpdateUrl() const
{
	return mDriverUpdateUrl;
}


const QUrl& SecureStorage::getProviderUpdateUrl() const
{
	return mProviderUpdateUrl;
}


const QString& SecureStorage::getProviderIconUpdateUrlBase() const
{
	return mProviderIconUpdateUrlBase;
}


const QUrl& SecureStorage::getAppcastUpdateUrl() const
{
	return mAppcastUpdateUrl;
}


const QUrl& SecureStorage::getAppcastBetaUpdateUrl() const
{
	return mAppcastBetaUpdateUrl;
}


const QVector<QString>& SecureStorage::getAllowedSslEllipticCurves() const
{
	return mAllowedSslEllipticCurves;
}


const QVector<QString>& SecureStorage::getCiphersWithPsk() const
{
	return mPskCiphers;
}


const QVector<QString>& SecureStorage::getCiphersWithForwardSecrecy() const
{
	return mCiphersWithForwardSecrecy;
}


const QVector<QString>& SecureStorage::getCiphersWithSha1ForBackwardCompatibility() const
{
	return mCiphersWithSha1ForBackwardCompatibility;
}


QSsl::SslProtocol SecureStorage::getSslProtocolVersion() const
{
	return mProtocolVersion;
}


QSsl::SslProtocol SecureStorage::getSslProtocolVersionPsk() const
{
	return mProtocolVersionPsk;
}


const QVector<SignatureAlgorithmPair>& SecureStorage::getSignatureAlgorithms() const
{
	return mSignatureAlgorithms;
}


const QVector<SignatureAlgorithmPair>& SecureStorage::getSignatureAlgorithmsPsk() const
{
	return mSignatureAlgorithmsPsk;
}


bool SecureStorage::readJsonArray(QJsonArray& pArray, const QJsonObject& pConfig, const QLatin1String& pName)
{
	QJsonValue value = pConfig.value(pName);
	if (!value.isArray())
	{
		qCritical() << "Expecting array for" << pName << "in SecureStorage";
		return false;
	}
	pArray = value.toArray();
	return true;
}


QString SecureStorage::readGroup(const QJsonObject& pConfig, const QLatin1String& pGroup, const QLatin1String& pName)
{
	QJsonValue value = pConfig.value(pGroup);
	if (!value.isObject())
	{
		qCritical() << "Expecting group for" << pGroup << "in SecureStorage";
		return QString();
	}

	QJsonObject groupObject = value.toObject();
	value = groupObject.value(pName);
	if (!value.isString())
	{
		qCritical() << "Expecting string for" << pGroup << "/" << pName << "in SecureStorage";
		return QString();
	}

	return value.toString();
}


QSsl::SslProtocol SecureStorage::readSslProtocol(const QJsonObject& pConfig, const QLatin1String& pName)
{
	const auto& value = pConfig.value(pName).toString();
	if (value == QLatin1String("TlsV1_0OrLater"))
	{
		return QSsl::SslProtocol::TlsV1_0OrLater;
	}
	if (value == QLatin1String("TlsV1_1OrLater"))
	{
		return QSsl::SslProtocol::TlsV1_1OrLater;
	}
	if (value == QLatin1String("TlsV1_2OrLater"))
	{
		return QSsl::SslProtocol::TlsV1_2OrLater;
	}
	qCritical() << pName << ": Unsupported TLS protocol version detected" << value;
	qCritical() << "Returning default \"SecureProtocols\"";
	return QSsl::SslProtocol::SecureProtocols;
}


QVector<SignatureAlgorithmPair> SecureStorage::readSignatureAlgorithms(const QJsonObject& pConfig, const QLatin1String& pKey)
{
	const QJsonValue& tmp = pConfig[pKey];
	if (tmp.isUndefined() || !tmp.isArray())
	{
		qCritical() << pKey << "is malformed";
		return QVector<SignatureAlgorithmPair>();
	}
	const QJsonArray& array = tmp.toArray();

	QVector<SignatureAlgorithmPair> algorithms;
	for (const QJsonValue& line : array)
	{
		const auto& parts = line.toString().split(QLatin1Char('+'));
		if (parts.size() != 2)
		{
			qCritical() << pKey << "has malformed item" << line;
			return QVector<SignatureAlgorithmPair>();
		}

		static const auto& hashMetaEnum = QMetaEnum::fromType<QCryptographicHash::Algorithm>();
		bool hashConversionSuccessfull;
		const int hashInt = hashMetaEnum.keyToValue(parts[1].toLatin1().constData(), &hashConversionSuccessfull);
		if (!hashConversionSuccessfull)
		{
			qCritical() << "Not a hash algorithm" << parts[1];
			return QVector<SignatureAlgorithmPair>();
		}
		auto hash = static_cast<QCryptographicHash::Algorithm>(hashInt);

		if (parts[0] == QLatin1String("Rsa"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Rsa, hash);
		}
		else if (parts[0] == QLatin1String("Dsa"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Dsa, hash);
		}
		else if (parts[0] == QLatin1String("Ec"))
		{
			algorithms += SignatureAlgorithmPair(QSsl::KeyAlgorithm::Ec, hash);
		}
		else
		{
			qCritical() << "Not a signature algorithm" << parts[0];
			return QVector<SignatureAlgorithmPair>();
		}
	}
	return algorithms;
}


void SecureStorage::readByteArrayList(QByteArrayList& pArray, const QJsonObject& pConfig, const QLatin1String& pName)
{
	QJsonArray jsonArray;
	if (readJsonArray(jsonArray, pConfig, pName))
	{
		for (int i = 0; i < jsonArray.size(); ++i)
		{
			QJsonValue certificate = jsonArray[i];
			if (!certificate.isString())
			{
				qCritical() << "Expected hexstring in array[" << i << "] " << pName << " in SecureStorage";
				continue;
			}
			pArray += certificate.toString().toLatin1();
		}
	}
}
