/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceSettings.h"

#include "DeviceInfo.h"
#include "KeyPair.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMutableVectorIterator>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
SETTINGS_NAME(SETTINGS_GROUP_NAME_REMOTEREADER, "remotereader")
SETTINGS_NAME(SETTINGS_NAME_DEVICE_NAME, "serverName")
SETTINGS_NAME(SETTINGS_NAME_PIN_PAD_MODE, "pinPadMode")
SETTINGS_NAME(SETTINGS_ARRAY_NAME_TRUSTED_CERTIFICATES, "trustedCertificates")
SETTINGS_NAME(SETTINGS_NAME_TRUSTED_CERTIFICATE_ITEM, "certificate")
SETTINGS_NAME(SETTINGS_NAME_TRUSTED_REMOTE_INFO, "trustedRemoteInfo")
SETTINGS_NAME(SETTINGS_NAME_KEY, "key")
SETTINGS_NAME(SETTINGS_NAME_CERTIFICATE, "certificate")
} // namespace


QString RemoteServiceSettings::generateFingerprint(const QSslCertificate& pCert)
{
	return pCert.isNull() ? QString() : QString::fromLatin1(pCert.digest(QCryptographicHash::Sha256).toHex());
}


RemoteServiceSettings::RemoteServiceSettings()
	: AbstractSettings()
	, mStore(getStore())
{
	mStore->beginGroup(SETTINGS_GROUP_NAME_REMOTEREADER());

	if (!mStore->contains(SETTINGS_NAME_DEVICE_NAME()))
	{
		setServerName(QString());
	}
}


RemoteServiceSettings::~RemoteServiceSettings()
{
}


void RemoteServiceSettings::save()
{
	mStore->sync();
}


QString RemoteServiceSettings::getDefaultServerName()
{
	QString name = DeviceInfo::getName();
	if (name.isEmpty())
	{
		return tr("Remote Reader");
	}

	return name;
}


QString RemoteServiceSettings::getServerName() const
{
	return mStore->value(SETTINGS_NAME_DEVICE_NAME(), QString()).toString();
}


void RemoteServiceSettings::setServerName(const QString& pName)
{
	if (pName.isEmpty())
	{
		mStore->setValue(SETTINGS_NAME_DEVICE_NAME(), getDefaultServerName());
		return;
	}

	mStore->setValue(SETTINGS_NAME_DEVICE_NAME(), pName);
}


bool RemoteServiceSettings::getPinPadMode() const
{
	return mStore->value(SETTINGS_NAME_PIN_PAD_MODE(), false).toBool();
}


void RemoteServiceSettings::setPinPadMode(bool pPinPadMode)
{
	mStore->setValue(SETTINGS_NAME_PIN_PAD_MODE(), pPinPadMode);
}


QList<QSslCertificate> RemoteServiceSettings::getTrustedCertificates() const
{
	const int itemCount = mStore->beginReadArray(SETTINGS_ARRAY_NAME_TRUSTED_CERTIFICATES());

	QList<QSslCertificate> certificates;
	certificates.reserve(itemCount);
	for (int i = 0; i < itemCount; ++i)
	{
		mStore->setArrayIndex(i);
		const auto& cert = mStore->value(SETTINGS_NAME_TRUSTED_CERTIFICATE_ITEM(), QByteArray()).toByteArray();
		certificates << QSslCertificate(cert);
	}

	mStore->endArray();
	return certificates;
}


void RemoteServiceSettings::setUniqueTrustedCertificates(const QSet<QSslCertificate>& pCertificates)
{
	mStore->beginGroup(SETTINGS_ARRAY_NAME_TRUSTED_CERTIFICATES());
	mStore->remove(QString());
	mStore->endGroup();

	mStore->beginWriteArray(SETTINGS_ARRAY_NAME_TRUSTED_CERTIFICATES());
	int i = 0;
	for (const auto& cert : pCertificates)
	{
		mStore->setArrayIndex(i++);
		mStore->setValue(SETTINGS_NAME_TRUSTED_CERTIFICATE_ITEM(), cert.toPem());
	}
	mStore->endArray();

	syncRemoteInfos(pCertificates);
	Q_EMIT fireTrustedCertificatesChanged();
}


void RemoteServiceSettings::setTrustedCertificates(const QList<QSslCertificate>& pCertificates)
{
	setUniqueTrustedCertificates(pCertificates.toSet()); // remove duplicates
}


void RemoteServiceSettings::addTrustedCertificate(const QSslCertificate& pCertificate)
{
	auto certs = getTrustedCertificates();
	certs << pCertificate;
	setTrustedCertificates(certs);
}


void RemoteServiceSettings::removeTrustedCertificate(const QSslCertificate& pCertificate)
{
	auto certs = getTrustedCertificates();
	certs.removeAll(pCertificate);
	setTrustedCertificates(certs);
}


void RemoteServiceSettings::removeTrustedCertificate(const QString& pFingerprint)
{
	const auto& certs = getTrustedCertificates();
	for (const auto& cert : certs)
	{
		if (generateFingerprint(cert) == pFingerprint)
		{
			removeTrustedCertificate(cert);
			return;
		}
	}
}


bool RemoteServiceSettings::checkAndGenerateKey(bool pForceGeneration)
{
	if (getKey().isNull()
			|| getCertificate().isNull()
			|| getCertificate().expiryDate() < QDateTime::currentDateTime()
			|| pForceGeneration)
	{
		qCDebug(settings) << "Generate local keypair...";
		const auto& pair = KeyPair::generate();
		if (pair.isValid())
		{
			setKey(pair.getKey());
			setCertificate(pair.getCertificate());
			return true;
		}

		return false;
	}

	return true;
}


QSslCertificate RemoteServiceSettings::getCertificate() const
{
	return QSslCertificate(mStore->value(SETTINGS_NAME_CERTIFICATE(), QByteArray()).toByteArray());
}


void RemoteServiceSettings::setCertificate(const QSslCertificate& pCert) const
{
	mStore->setValue(SETTINGS_NAME_CERTIFICATE(), pCert.toPem());
}


QSslKey RemoteServiceSettings::getKey() const
{
	const auto& data = mStore->value(SETTINGS_NAME_KEY(), QByteArray()).toByteArray();
	if (data.contains("BEGIN RSA PRIVATE KEY"))
	{
		return QSslKey(data, QSsl::Rsa);
	}

	return QSslKey();
}


void RemoteServiceSettings::setKey(const QSslKey& pKey) const
{
	mStore->setValue(SETTINGS_NAME_KEY(), pKey.toPem());
}


RemoteServiceSettings::RemoteInfo RemoteServiceSettings::getRemoteInfo(const QSslCertificate& pCertificate) const
{
	return getRemoteInfo(generateFingerprint(pCertificate));
}


RemoteServiceSettings::RemoteInfo RemoteServiceSettings::getRemoteInfo(const QString& pFingerprint) const
{
	const auto& infos = getRemoteInfos();
	for (const auto& item : infos)
	{
		if (item.getFingerprint() == pFingerprint)
		{
			return item;
		}
	}

	return RemoteInfo();
}


QVector<RemoteServiceSettings::RemoteInfo> RemoteServiceSettings::getRemoteInfos() const
{
	QVector<RemoteInfo> infos;

	const auto& data = mStore->value(SETTINGS_NAME_TRUSTED_REMOTE_INFO(), QByteArray()).toByteArray();
	const auto& array = QJsonDocument::fromJson(data).array();
	for (const auto& item : array)
	{
		const auto& obj = item.toObject();
		auto fingerprint = obj[QLatin1String("fingerprint")].toString();
		auto name = obj[QLatin1String("name")].toString();
		auto lastConnected = QDateTime::fromString(obj[QLatin1String("lastConnected")].toString(), Qt::ISODateWithMs);
		infos << RemoteInfo(fingerprint, lastConnected, name);
	}

	return infos;
}


void RemoteServiceSettings::setRemoteInfos(const QVector<RemoteInfo>& pInfos)
{
	QJsonArray array;
	for (const auto& item : pInfos)
	{
		QJsonObject obj;
		obj[QLatin1String("fingerprint")] = item.getFingerprint();
		obj[QLatin1String("name")] = item.getName();
		obj[QLatin1String("lastConnected")] = item.getLastConnected().toString(Qt::ISODateWithMs);
		array << obj;
	}

	mStore->setValue(SETTINGS_NAME_TRUSTED_REMOTE_INFO(), QJsonDocument(array).toJson(QJsonDocument::Compact));
	Q_EMIT fireTrustedRemoteInfosChanged();
}


void RemoteServiceSettings::syncRemoteInfos(const QSet<QSslCertificate>& pCertificates)
{
	QStringList trustedFingerprints;
	for (const auto& cert : pCertificates)
	{
		trustedFingerprints << generateFingerprint(cert);
	}

	QVector<RemoteInfo> syncedInfo;

	// remove outdated entries
	const auto& infos = getRemoteInfos();
	for (const auto& info : infos)
	{
		if (trustedFingerprints.contains(info.getFingerprint()))
		{
			trustedFingerprints.removeOne(info.getFingerprint());
			syncedInfo << info;
		}
	}

	// add new entries
	for (const auto& fingerprint : qAsConst(trustedFingerprints))
	{
		syncedInfo << RemoteInfo(fingerprint, QDateTime::currentDateTime());
	}

	setRemoteInfos(syncedInfo);
}


bool RemoteServiceSettings::updateRemoteInfo(const RemoteInfo& pInfo)
{
	if (pInfo.getFingerprint().isEmpty())
	{
		return false;
	}

	auto infos = getRemoteInfos();
	QMutableVectorIterator<RemoteInfo> iter(infos);
	while (iter.hasNext())
	{
		iter.next();
		if (iter.value().getFingerprint() == pInfo.getFingerprint())
		{
			iter.setValue(pInfo);
			setRemoteInfos(infos);
			return true;
		}
	}

	return false;
}


RemoteServiceSettings::RemoteInfo::RemoteInfo(const QString& pFingerprint,
		const QDateTime& pLastConnected,
		const QString& pName)
	: mFingerprint(pFingerprint)
	, mName(pName)
	, mLastConnected(pLastConnected)
{
}


const QString& RemoteServiceSettings::RemoteInfo::getFingerprint() const
{
	return mFingerprint;
}


const QString& RemoteServiceSettings::RemoteInfo::getName() const
{
	return mName;
}


void RemoteServiceSettings::RemoteInfo::setName(const QString& pName)
{
	mName = pName;
}


const QDateTime& RemoteServiceSettings::RemoteInfo::getLastConnected() const
{
	return mLastConnected;
}


void RemoteServiceSettings::RemoteInfo::setLastConnected(const QDateTime& pLastConnected)
{
	mLastConnected = pLastConnected;
}


bool RemoteServiceSettings::RemoteInfo::operator==(const RemoteInfo& pOther) const
{
	return mFingerprint == pOther.mFingerprint
		   && mName == pOther.mName
		   && mLastConnected == pOther.mLastConnected;
}


bool RemoteServiceSettings::RemoteInfo::operator!=(const RemoteInfo& pOther) const
{
	return !(*this == pOther);
}
