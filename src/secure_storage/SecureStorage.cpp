/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */


#include "SecureStorage.h"

#include "BuildHelper.h"
#include "FileDestination.h"
#include "SingletonHelper.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QMetaEnum>
#include <QTextStream>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(securestorage)

namespace
{
#define CONFIG_NAME(_name, _key) /* NOLINT */\
		inline QLatin1String _name(){\
			return QLatin1String(_key);\
		}

CONFIG_NAME(CONFIGURATION_VENDOR, "vendor")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_CV_ROOT_CERTIFICATE, "cvRootCertificates")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_CV_ROOT_CERTIFICATE_TEST, "cvRootCertificatesTest")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_UPDATE_CERTIFICATES, "updateCertificates")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_TLS_SETTINGS, "tlsSettings")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_PSK, "tlsSettingsPsk")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_REMOTE_IFD, "tlsSettingsRemoteIfd")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_REMOTE_IFD_PAIRING, "tlsSettingsRemoteIfdPairing")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_LOCAL_IFD, "tlsSettingsLocalIfd")
CONFIG_NAME(CONFIGURATION_GROUP_NAME_MIN_KEY_SIZES, "minKeySizes")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_SELF_AUTHENTICATION, "selfAuthentication")
CONFIG_NAME(CONFIGURATION_NAME_SELF_AUTHENTICATION_URL, "url")
CONFIG_NAME(CONFIGURATION_NAME_SELF_AUTHENTICATION_TEST_URL, "testUrl")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_UPDATE_SERVER, "updateServer")
CONFIG_NAME(CONFIGURATION_NAME_UPDATE_SERVER_BASE_URL, "baseUrl")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_WHITELIST_SERVER, "whitelistServer")
CONFIG_NAME(CONFIGURATION_NAME_WHITELIST_SERVER_BASE_URL, "baseUrl")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_UPDATES, "updates")
CONFIG_NAME(CONFIGURATION_NAME_APPCAST_UPDATE_URL, "release")
CONFIG_NAME(CONFIGURATION_NAME_APPCAST_BETA_UPDATE_URL, "beta")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_SMART, "smart")
CONFIG_NAME(CONFIGURATION_NAME_SMART_PERSONALIZATION_URL, "personalizationUrl")
CONFIG_NAME(CONFIGURATION_NAME_SMART_PERSONALIZATION_TEST_URL, "personalizationTestUrl")
CONFIG_NAME(CONFIGURATION_NAME_SMART_SERVICEID, "serviceId")
CONFIG_NAME(CONFIGURATION_NAME_SMART_SSDAID, "ssdAid")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_LOCAL_IFD, "localIfd")
CONFIG_NAME(CONFIGURATION_NAME_LOCAL_IFD_PACKAGE_NAME, "packageName")
CONFIG_NAME(CONFIGURATION_NAME_LOCAL_IFD_MIN_VERSION, "minVersion")
CONFIG_NAME(CONFIGURATION_NAME_LOCAL_IFD_ALLOWED_CERTIFICATE_HASHES, "allowedCertificateHashes")
CONFIG_NAME(CONFIGURATION_NAME_LOCAL_IFD_MIN_PSK_SIZE, "minPskSize")

CONFIG_NAME(CONFIGURATION_GROUP_NAME_SIZES_IFD, "sizesIfd")
CONFIG_NAME(CONFIGURATION_NAME_SIZES_IFD_CREATE, "createRsa")
CONFIG_NAME(CONFIGURATION_NAME_SIZES_IFD_MIN, "min")

} // namespace

defineSingleton(SecureStorage)


SecureStorage::SecureStorage()
	: mCvcas()
	, mCvcasTest()
	, mUpdateCertificates()
	, mSelfAuthenticationUrl()
	, mSelfAuthenticationTestUrl()
	, mUpdateServerBaseUrl()
	, mWhitelistServerBaseUrl()
	, mAppcastUpdateUrl()
	, mAppcastBetaUpdateUrl()
	, mSmartPersonalizationUrl()
	, mSmartPersonalizationTestUrl()
	, mSmartServiceId()
	, mSmartSsdAid()
	, mLocalIfdPackageName()
	, mLocalIfdMinVersion()
	, mLocalIfAllowedCertificateHashes()
	, mLocalIfdMinPskSize()
	, mTlsConfig()
	, mTlsConfigPsk()
	, mTlsConfigRemoteIfd()
	, mTlsConfigRemoteIfdPairing()
	, mTlsConfigLocalIfd()
	, mMinKeySizes()
	, mMinKeySizesIfd()
	, mCreateKeySizeIfd()
{
	load();
}


QString SecureStorage::getDeveloperConfig() const
{
	if (BuildHelper::getCertificateType() == CertificateType::DEVELOPER)
	{
		return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QStringLiteral("/config.json");
	}

	return QString();
}


QString SecureStorage::getCustomConfig() const
{
	const auto& path = getCustomConfigPath();
	if (QFile(path).exists())
	{
		return path;
	}

	return QString();
}


QString SecureStorage::getCustomConfigPath() const
{
	return FileDestination::getPath(QStringLiteral("config.json"));
}


QString SecureStorage::getEmbeddedConfig() const
{
	return QStringLiteral(":/config.json");
}


QJsonObject SecureStorage::loadFile(const QStringList& pFiles) const
{
	for (const auto& path : pFiles)
	{
		if (path.isNull())
		{
			continue;
		}

		qCDebug(securestorage) << "Load SecureStorage:" << path;

		QFile configFile(path);
		if (!configFile.exists())
		{
			qCCritical(securestorage) << "SecureStorage not found";
			continue;
		}

		if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCCritical(securestorage) << "Wasn't able to open SecureStorage";
			continue;
		}

		QJsonParseError parseError {};
		const auto& document = QJsonDocument::fromJson(configFile.readAll(), &parseError);
		if (parseError.error != QJsonParseError::NoError)
		{
			qCCritical(securestorage) << "Parse error while reading SecureStorage on position" << parseError.offset << ':' << parseError.errorString();
			continue;
		}

		return document.object();
	}

	return QJsonObject();
}


void SecureStorage::load()
{
	const QStringList files({getDeveloperConfig(), getCustomConfig(), getEmbeddedConfig()});
	qCDebug(securestorage) << "Try locations:" << files;
	const auto& config = loadFile(files);
	if (config.isEmpty())
	{
		return;
	}

	mVendor = config.value(CONFIGURATION_VENDOR()).toString();

	mCvcas.clear();
	mCvcas = readByteArrayList(config, CONFIGURATION_GROUP_NAME_CV_ROOT_CERTIFICATE());

	mCvcasTest.clear();
	mCvcasTest = readByteArrayList(config, CONFIGURATION_GROUP_NAME_CV_ROOT_CERTIFICATE_TEST());
	mCvcasTest = loadTestCvcsFromAppDir() + mCvcasTest;

	QByteArrayList certificates = readByteArrayList(config, CONFIGURATION_GROUP_NAME_UPDATE_CERTIFICATES());
	mUpdateCertificates.clear();
	for (int i = 0; i < certificates.size(); ++i)
	{
		QSslCertificate certificate(certificates[i], QSsl::Der);
		if (certificate.isNull())
		{
			qCCritical(securestorage) << "Failed to read update certificate[" << i << "] from SecureStorage";
			continue;
		}
		mUpdateCertificates += certificate;
	}

	QJsonValue tlsValue = config.value(CONFIGURATION_GROUP_NAME_TLS_SETTINGS());
	if (tlsValue.isObject())
	{
		mTlsConfig.load(tlsValue.toObject());
	}

	QJsonValue tlsPskValue = config.value(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_PSK());
	if (tlsPskValue.isObject())
	{
		mTlsConfigPsk.load(tlsPskValue.toObject());
	}

	QJsonValue tlsRemoteReaderValue = config.value(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_REMOTE_IFD());
	if (tlsRemoteReaderValue.isObject())
	{
		mTlsConfigRemoteIfd.load(tlsRemoteReaderValue.toObject());
	}
	QJsonValue tlsRemoteReaderPairingValue = config.value(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_REMOTE_IFD_PAIRING());
	if (tlsRemoteReaderPairingValue.isObject())
	{
		mTlsConfigRemoteIfdPairing.load(tlsRemoteReaderPairingValue.toObject());
	}
	QJsonValue tlsLocalIfd = config.value(CONFIGURATION_GROUP_NAME_TLS_CONFIGURATION_LOCAL_IFD());
	if (tlsLocalIfd.isObject())
	{
		mTlsConfigLocalIfd.load(tlsLocalIfd.toObject());
	}

	mMinKeySizes = readKeySizes(config, CONFIGURATION_GROUP_NAME_MIN_KEY_SIZES());

	const QJsonObject obj = config.value(CONFIGURATION_GROUP_NAME_SIZES_IFD()).toObject();
	mMinKeySizesIfd = readKeySizes(obj, CONFIGURATION_NAME_SIZES_IFD_MIN());
	mCreateKeySizeIfd = obj.value(CONFIGURATION_NAME_SIZES_IFD_CREATE()).toInt();

	mSelfAuthenticationUrl = readGroup(config, CONFIGURATION_GROUP_NAME_SELF_AUTHENTICATION(), CONFIGURATION_NAME_SELF_AUTHENTICATION_URL());
	mSelfAuthenticationTestUrl = readGroup(config, CONFIGURATION_GROUP_NAME_SELF_AUTHENTICATION(), CONFIGURATION_NAME_SELF_AUTHENTICATION_TEST_URL());

	mUpdateServerBaseUrl = readGroup(config, CONFIGURATION_GROUP_NAME_UPDATE_SERVER(), CONFIGURATION_NAME_UPDATE_SERVER_BASE_URL());
	mWhitelistServerBaseUrl = readGroup(config, CONFIGURATION_GROUP_NAME_WHITELIST_SERVER(), CONFIGURATION_NAME_WHITELIST_SERVER_BASE_URL());

	mAppcastUpdateUrl = readGroup(config, CONFIGURATION_GROUP_NAME_UPDATES(), CONFIGURATION_NAME_APPCAST_UPDATE_URL());
	mAppcastBetaUpdateUrl = readGroup(config, CONFIGURATION_GROUP_NAME_UPDATES(), CONFIGURATION_NAME_APPCAST_BETA_UPDATE_URL());

	mSmartPersonalizationUrl = readGroup(config, CONFIGURATION_GROUP_NAME_SMART(), CONFIGURATION_NAME_SMART_PERSONALIZATION_URL());
	mSmartPersonalizationTestUrl = readGroup(config, CONFIGURATION_GROUP_NAME_SMART(), CONFIGURATION_NAME_SMART_PERSONALIZATION_TEST_URL());
	mSmartServiceId = readGroup(config, CONFIGURATION_GROUP_NAME_SMART(), CONFIGURATION_NAME_SMART_SERVICEID());
	mSmartSsdAid = readGroup(config, CONFIGURATION_GROUP_NAME_SMART(), CONFIGURATION_NAME_SMART_SSDAID());

	QJsonValue localIfdValue = config.value(CONFIGURATION_GROUP_NAME_LOCAL_IFD());
	if (localIfdValue.isObject())
	{
		mLocalIfdPackageName = readGroup(config, CONFIGURATION_GROUP_NAME_LOCAL_IFD(), CONFIGURATION_NAME_LOCAL_IFD_PACKAGE_NAME());
		mLocalIfdMinVersion = readGroup(config, CONFIGURATION_GROUP_NAME_LOCAL_IFD(), CONFIGURATION_NAME_LOCAL_IFD_MIN_VERSION());
		QJsonObject localIfdValueObject = localIfdValue.toObject();
		mLocalIfAllowedCertificateHashes = readByteArrayList(localIfdValueObject, CONFIGURATION_NAME_LOCAL_IFD_ALLOWED_CERTIFICATE_HASHES());
		mLocalIfdMinPskSize = localIfdValueObject.value(CONFIGURATION_NAME_LOCAL_IFD_MIN_PSK_SIZE()).toInt();
	}

	qCDebug(securestorage) << "SecureStorage loaded:" << isValid();
	qCInfo(securestorage) << "Vendor" << mVendor;
}


bool SecureStorage::isValid() const
{
	// check some eID required member
	return !mCvcas.isEmpty()
		   && !mCvcasTest.isEmpty()
		   && !mSelfAuthenticationUrl.isEmpty()
		   && !mSelfAuthenticationTestUrl.isEmpty()
		   && mTlsConfig.isValid()
		   && mTlsConfigPsk.isValid()
		   && mTlsConfigRemoteIfd.isValid()
		   && mTlsConfigRemoteIfdPairing.isValid()
		   && mTlsConfigLocalIfd.isValid()
		   && !mMinKeySizes.isEmpty()
		   && mLocalIfdMinPskSize > 0;
}


QByteArrayList SecureStorage::loadTestCvcsFromAppDir() const
{
	QByteArrayList testCvcs;
	const QDir appDir(QCoreApplication::applicationDirPath());
	const QStringList& dirEntries = appDir.entryList({QStringLiteral("*.cvcert.hex")}, QDir::Files);
	for (QString cvcFilePath : dirEntries)
	{
		cvcFilePath = appDir.absolutePath() + QDir::separator() + cvcFilePath;
		const QByteArray& hex = loadTestCvc(cvcFilePath);
		if (hex.isEmpty())
		{
			qWarning() << "Can not load CVC from" << cvcFilePath;
			continue;
		}

		qDebug() << "Adding CVC from" << cvcFilePath << ':' << hex;
		testCvcs += hex;
	}
	return testCvcs;
}


QByteArray SecureStorage::loadTestCvc(const QString& pPath) const
{
	QFile cvcFile(pPath);
	const int TEN_MEGA_BYTE = 10 * 1024 * 1024;
	if (cvcFile.size() > TEN_MEGA_BYTE)
	{
		return QByteArray();
	}
	if (!cvcFile.open(QFile::ReadOnly | QFile::Text))
	{
		return QByteArray();
	}
	return cvcFile.readAll();
}


[[nodiscard]] const QString& SecureStorage::getVendor() const
{
	return mVendor;
}


const QByteArrayList& SecureStorage::getCVRootCertificates(bool pProductive) const
{
	return pProductive ? mCvcas : mCvcasTest;
}


const QList<QSslCertificate>& SecureStorage::getUpdateCertificates() const
{
	return mUpdateCertificates;
}


const QUrl& SecureStorage::getSelfAuthenticationUrl(bool pTest) const
{
	return pTest ? mSelfAuthenticationTestUrl : mSelfAuthenticationUrl;
}


const QUrl& SecureStorage::getUpdateServerBaseUrl() const
{
	return mUpdateServerBaseUrl;
}


const QUrl& SecureStorage::getWhitelistServerBaseUrl() const
{
	return mWhitelistServerBaseUrl;
}


const QUrl& SecureStorage::getAppcastUpdateUrl() const
{
	return mAppcastUpdateUrl;
}


const QUrl& SecureStorage::getAppcastBetaUpdateUrl() const
{
	return mAppcastBetaUpdateUrl;
}


const QString& SecureStorage::getSmartPersonalizationUrl(bool pTest) const
{
	return pTest ? mSmartPersonalizationTestUrl : mSmartPersonalizationUrl;
}


const QString& SecureStorage::getSmartServiceId() const
{
	return mSmartServiceId;
}


const QString& SecureStorage::getSmartSsdAid() const
{
	return mSmartSsdAid;
}


const TlsConfiguration& SecureStorage::getTlsConfig(TlsSuite pTlsSuite) const
{
	return pTlsSuite == TlsSuite::PSK ? mTlsConfigPsk : mTlsConfig;
}


const TlsConfiguration& SecureStorage::getTlsConfigRemoteIfd(TlsSuite pTlsSuite) const
{
	return pTlsSuite == TlsSuite::PSK ? mTlsConfigRemoteIfdPairing : mTlsConfigRemoteIfd;
}


const TlsConfiguration& SecureStorage::getTlsConfigLocalIfd() const
{
	return mTlsConfigLocalIfd;
}


const QString& SecureStorage::getLocalIfdPackageName() const
{
	return mLocalIfdPackageName;
}


const QString& SecureStorage::getLocalIfdMinVersion() const
{
	return mLocalIfdMinVersion;
}


const QByteArrayList& SecureStorage::getLocalIfdAllowedCertificateHashes() const
{
	return mLocalIfAllowedCertificateHashes;
}


int SecureStorage::getLocalIfdMinPskSize() const
{
	return mLocalIfdMinPskSize;
}


int SecureStorage::getMinimumKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const
{
	if (!mMinKeySizes.contains(pKeyAlgorithm))
	{
		qCWarning(securestorage) << "No minimum key size specified, returning default";
	}
	return mMinKeySizes.value(pKeyAlgorithm, 0);
}


int SecureStorage::getMinimumIfdKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const
{
	if (!mMinKeySizesIfd.contains(pKeyAlgorithm))
	{
		qCWarning(securestorage) << "No minimum key size specified, returning default";
	}
	return mMinKeySizesIfd.value(pKeyAlgorithm, 0);
}


int SecureStorage::getIfdCreateSize() const
{
	return mCreateKeySizeIfd;
}


QJsonArray SecureStorage::readJsonArray(const QJsonObject& pConfig, const QLatin1String pName) const
{
	QJsonValue value = pConfig.value(pName);
	if (!value.isArray())
	{
		qCCritical(securestorage) << "Expecting array for" << pName << "in SecureStorage";
		return QJsonArray();
	}
	return value.toArray();
}


QString SecureStorage::readGroup(const QJsonObject& pConfig, const QLatin1String pGroup, const QLatin1String pName) const
{
	QJsonValue value = pConfig.value(pGroup);
	if (!value.isObject())
	{
		qCCritical(securestorage) << "Expecting group for" << pGroup << "in SecureStorage";
		return QString();
	}

	QJsonObject groupObject = value.toObject();
	value = groupObject.value(pName);
	if (!value.isString())
	{
		qCCritical(securestorage) << "Expecting string for" << pGroup << "/" << pName << "in SecureStorage";
		return QString();
	}

	return value.toString();
}


QMap<QSsl::KeyAlgorithm, int> SecureStorage::readKeySizes(const QJsonObject& pConfig, const QLatin1String pKey) const
{
	QMap<QSsl::KeyAlgorithm, int> keySizes;
	const auto& object = pConfig.value(pKey).toObject();
	if (!object.isEmpty())
	{
		const auto& keys = object.keys();
		for (const QString& key : keys)
		{
			const auto value = object.value(key).toInt(0);
			if (key == QLatin1String("Rsa"))
			{
				keySizes.insert(QSsl::KeyAlgorithm::Rsa, value);
			}
			else if (key == QLatin1String("Dsa"))
			{
				keySizes.insert(QSsl::KeyAlgorithm::Dsa, value);
			}
			else if (key == QLatin1String("Dh"))
			{
				keySizes.insert(QSsl::KeyAlgorithm::Dh, value);
			}
			else if (key == QLatin1String("Ec"))
			{
				keySizes.insert(QSsl::KeyAlgorithm::Ec, value);
			}
			else
			{
				qCCritical(securestorage) << "Ignore unknown key type" << key;
			}
		}
	}
	return keySizes;
}


QByteArrayList SecureStorage::readByteArrayList(const QJsonObject& pConfig, const QLatin1String pName) const
{
	QJsonArray jsonArray = readJsonArray(pConfig, pName);
	QByteArrayList byteArrayList;

	for (int i = 0; i < jsonArray.size(); ++i)
	{
		QJsonValue entry = jsonArray[i];
		if (!entry.isString())
		{
			qCCritical(securestorage) << "Expected hexstring in array[" << i << "] " << pName << " in SecureStorage";
			continue;
		}
		byteArrayList << QByteArray::fromHex(entry.toString().toLatin1());
	}

	return byteArrayList;
}
