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

const QLatin1String SETTINGS_GROUP_NAME_TLS_SETTINGS("tlsSettings");
const QLatin1String SETTINGS_GROUP_NAME_TLS_SETTINGS_PSK("tlsSettingsPsk");
const QLatin1String SETTINGS_GROUP_NAME_MIN_STATIC_KEY_SIZES("minStaticKeySizes");
const QLatin1String SETTINGS_GROUP_NAME_MIN_EPHEMERAL_KEY_SIZES("minEphemeralKeySizes");

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
	, mTlsSettings()
	, mTlsSettingsPsk()
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

	QJsonValue tlsValue = config.value(SETTINGS_GROUP_NAME_TLS_SETTINGS);
	if (!tlsValue.isUndefined())
	{
		mTlsSettings.load(tlsValue.toObject());
	}

	QJsonValue tlsPskValue = config.value(SETTINGS_GROUP_NAME_TLS_SETTINGS_PSK);
	if (!tlsPskValue.isUndefined())
	{
		mTlsSettingsPsk.load(tlsPskValue.toObject());
	}
	mMinStaticKeySizes = readKeySizes(config, SETTINGS_GROUP_NAME_MIN_STATIC_KEY_SIZES);
	mMinEphemeralKeySizes = readKeySizes(config, SETTINGS_GROUP_NAME_MIN_EPHEMERAL_KEY_SIZES);


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


const TlsSettings& SecureStorage::getTlsSettings() const
{
	return mTlsSettings;
}


const TlsSettings& SecureStorage::getTlsSettingsPsk() const
{
	return mTlsSettingsPsk;
}


int SecureStorage::getMinimumStaticKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const
{
	if (!mMinStaticKeySizes.contains(pKeyAlgorithm))
	{
		qWarning() << "No minimum ephemeral key size specified, returning default";
	}
	return mMinStaticKeySizes.value(pKeyAlgorithm, 0);
}


int SecureStorage::getMinimumEphemeralKeySize(QSsl::KeyAlgorithm pKeyAlgorithm) const
{
	if (!mMinEphemeralKeySizes.contains(pKeyAlgorithm))
	{
		qWarning() << "No minimum ephemeral key size specified, returning default";
	}
	return mMinEphemeralKeySizes.value(pKeyAlgorithm, 0);
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


QMap<QSsl::KeyAlgorithm, int> SecureStorage::readKeySizes(const QJsonObject& pConfig, const QLatin1String& pKey)
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
			else if (key == QLatin1String("Ec"))
			{
				keySizes.insert(QSsl::KeyAlgorithm::Ec, value);
			}
			else
			{
				qCritical() << "Ignore unknown key type" << key;
			}
		}
	}
	return keySizes;
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
