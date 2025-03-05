/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceSettings.h"

#include "DeviceInfo.h"
#include "KeyPair.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMutableListIterator>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
SETTINGS_NAME(SETTINGS_GROUP_NAME_REMOTEREADER, "remotereader")
SETTINGS_NAME(SETTINGS_NAME_DEVICE_NAME, "deviceName")
SETTINGS_NAME(SETTINGS_NAME_PIN_PAD_MODE, "pinPadMode")
SETTINGS_NAME(SETTINGS_NAME_SHOW_ACCESS_RIGHTS, "showAccessRights")
SETTINGS_NAME(SETTINGS_NAME_TRUSTED_CERTIFICATES, "trustedCAs")
SETTINGS_NAME(SETTINGS_NAME_TRUSTED_REMOTE_INFO, "trustedRemoteInfo")
SETTINGS_NAME(SETTINGS_NAME_KEY, "key")
SETTINGS_NAME(SETTINGS_NAME_CERTIFICATE, "certificate")
} // namespace


QByteArray RemoteServiceSettings::generateFingerprint(const QSslCertificate& pCert)
{
	return pCert.isNull() ? QByteArray() : pCert.digest(QCryptographicHash::Sha256);
}


RemoteServiceSettings::RemoteServiceSettings()
	: AbstractSettings()
	, mStore(getStore())
{
	mStore->beginGroup(SETTINGS_GROUP_NAME_REMOTEREADER());

	// With 2.1.0 serverName was renamed to deviceName
	const QAnyStringView serverName("serverName");
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (mStore->contains(serverName))
	{
		setDeviceName(mStore->value(serverName).toString());
	}
#endif
	mStore->remove(serverName);

	// With 2.1.0 "trustedCertificates" moved from custom array to "trustedCAs" bytearray
	const QAnyStringView trustedCertificates("trustedCertificates");
	if (mStore->childGroups().contains(trustedCertificates))
	{
		const int itemCount = mStore->beginReadArray(trustedCertificates);
		QSet<QSslCertificate> certificates;
		certificates.reserve(itemCount);
		for (int i = 0; i < itemCount; ++i)
		{
			mStore->setArrayIndex(i);
			const auto& cert = mStore->value(QAnyStringView("certificate"), QByteArray()).toByteArray();
			certificates << QSslCertificate(cert);
		}
		mStore->endArray();
		setUniqueTrustedCertificates(certificates);

		mStore->beginGroup(trustedCertificates);
		mStore->remove(QString());
		mStore->endGroup();
	}
}


QString RemoteServiceSettings::getDefaultDeviceName() const
{
	QString name = DeviceInfo::getName();
	if (name.isEmpty())
	{
		//: LABEL ALL_PLATFORMS
		return tr("Smartphone as card reader (SaC)");
	}

	return name;
}


QString RemoteServiceSettings::getDeviceName() const
{

	if (mStore->contains(SETTINGS_NAME_DEVICE_NAME()))
	{
		return mStore->value(SETTINGS_NAME_DEVICE_NAME(), QString()).toString();
	}

	return getDefaultDeviceName();
}


void RemoteServiceSettings::setDeviceName(const QString& pName)
{
	const QString serverName = pName.trimmed();
	if (serverName.isEmpty() || serverName == getDefaultDeviceName())
	{
		mStore->remove(SETTINGS_NAME_DEVICE_NAME());
	}
	else
	{
		mStore->setValue(SETTINGS_NAME_DEVICE_NAME(), serverName);
	}
	save(mStore);
}


bool RemoteServiceSettings::getPinPadMode() const
{
	return mStore->value(SETTINGS_NAME_PIN_PAD_MODE(), true).toBool();
}


void RemoteServiceSettings::setPinPadMode(bool pPinPadMode)
{
	mStore->setValue(SETTINGS_NAME_PIN_PAD_MODE(), pPinPadMode);
	save(mStore);
}


bool RemoteServiceSettings::getShowAccessRights() const
{
	return mStore->value(SETTINGS_NAME_SHOW_ACCESS_RIGHTS(), false).toBool();
}


void RemoteServiceSettings::setShowAccessRights(bool pShowAccessRights)
{
	mStore->setValue(SETTINGS_NAME_SHOW_ACCESS_RIGHTS(), pShowAccessRights);
	save(mStore);
}


QList<QSslCertificate> RemoteServiceSettings::getTrustedCertificates() const
{
	return QSslCertificate::fromData(mStore->value(SETTINGS_NAME_TRUSTED_CERTIFICATES(), QByteArray()).toByteArray());
}


void RemoteServiceSettings::setUniqueTrustedCertificates(const QSet<QSslCertificate>& pCertificates)
{
	QByteArrayList data;
	for (const auto& cert : pCertificates)
	{
		data << cert.toPem();
	}
	mStore->setValue(SETTINGS_NAME_TRUSTED_CERTIFICATES(), data.join());

	syncRemoteInfos(pCertificates);
	Q_EMIT fireTrustedCertificatesChanged();
}


void RemoteServiceSettings::setTrustedCertificates(const QList<QSslCertificate>& pCertificates)
{
	// remove duplicates
	setUniqueTrustedCertificates(QSet<QSslCertificate>(pCertificates.constBegin(), pCertificates.constEnd()));
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


void RemoteServiceSettings::removeTrustedCertificate(const QByteArray& pFingerprint)
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


bool RemoteServiceSettings::checkAndGenerateKey(int pCreateKeySize) const
{
	auto certs = getCertificates();
	const auto& currentCert = certs.isEmpty() ? QSslCertificate() : certs.at(0);
	if (getKey().isNull()
			|| currentCert.isNull()
			|| currentCert.expiryDate() < QDateTime::currentDateTime()
			|| currentCert.publicKey().length() < pCreateKeySize)
	{
		qCDebug(settings) << "Generate local keypair...";
		const auto& pair = KeyPair::generate(pCreateKeySize, getKey().toPem(), currentCert.toPem());
		if (pair.isValid())
		{
			certs.prepend(pair.getCertificate());
			setKey(pair.getKey());
			setCertificates(certs);
			return true;
		}

		return false;
	}

	return true;
}


QList<QSslCertificate> RemoteServiceSettings::getCertificates() const
{
	return QSslCertificate::fromData(mStore->value(SETTINGS_NAME_CERTIFICATE(), QByteArray()).toByteArray());
}


void RemoteServiceSettings::setCertificates(const QList<QSslCertificate>& pCertChain) const
{
	QByteArrayList data;
	for (const auto& cert : pCertChain)
	{
		data << cert.toPem();
	}
	mStore->setValue(SETTINGS_NAME_CERTIFICATE(), data.join());
	save(mStore);
}


QSslKey RemoteServiceSettings::getKey() const
{
	const auto& data = mStore->value(SETTINGS_NAME_KEY(), QByteArray()).toByteArray();
	if (data.contains("BEGIN RSA PRIVATE KEY"))
	{
		return QSslKey(data, QSsl::Rsa);
	}
	else if (data.contains("BEGIN EC PRIVATE KEY"))
	{
		return QSslKey(data, QSsl::Ec);
	}

	return QSslKey();
}


void RemoteServiceSettings::setKey(const QSslKey& pKey) const
{
	mStore->setValue(SETTINGS_NAME_KEY(), pKey.toPem());
	save(mStore);
}


RemoteServiceSettings::RemoteInfo RemoteServiceSettings::getRemoteInfo(const QSslCertificate& pCertificate) const
{
	if (pCertificate.isNull())
	{
		return RemoteInfo();
	}

	return getRemoteInfo(generateFingerprint(pCertificate));
}


RemoteServiceSettings::RemoteInfo RemoteServiceSettings::getRemoteInfo(const QByteArray& pFingerprint) const
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


QList<RemoteServiceSettings::RemoteInfo> RemoteServiceSettings::getRemoteInfos() const
{
	QList<RemoteInfo> infos;

	const auto& data = mStore->value(SETTINGS_NAME_TRUSTED_REMOTE_INFO(), QByteArray()).toByteArray();
	const auto& array = QJsonDocument::fromJson(data).array();
	for (const QJsonValueConstRef item : array)
	{
		infos << RemoteInfo::fromJson(item.toObject());
	}

	return infos;
}


void RemoteServiceSettings::setRemoteInfos(const QList<RemoteInfo>& pInfos)
{
	QJsonArray array;
	for (const auto& item : pInfos)
	{
		array << item.toJson();
	}

	mStore->setValue(SETTINGS_NAME_TRUSTED_REMOTE_INFO(), QJsonDocument(array).toJson(QJsonDocument::Compact));
	save(mStore);
	Q_EMIT fireTrustedRemoteInfosChanged();
}


void RemoteServiceSettings::syncRemoteInfos(const QSet<QSslCertificate>& pCertificates)
{
	QByteArrayList trustedFingerprints;
	for (const auto& cert : pCertificates)
	{
		trustedFingerprints << generateFingerprint(cert);
	}

	QList<RemoteInfo> syncedInfo;

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
	for (const auto& fingerprint : std::as_const(trustedFingerprints))
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
	QMutableListIterator iter(infos);
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


QString RemoteServiceSettings::escapeDeviceName(const QString& pDeviceNameUnescaped)
{
	return pDeviceNameUnescaped.left(33).toHtmlEscaped();
}


RemoteServiceSettings::RemoteInfo::RemoteInfo(const QByteArray& pFingerprint,
		const QDateTime& pLastConnected)
	: mFingerprint(pFingerprint)
	, mName()
	, mLastConnected(pLastConnected)
{
}


RemoteServiceSettings::RemoteInfo RemoteServiceSettings::RemoteInfo::fromJson(const QJsonObject& obj)
{
	RemoteInfo remoteInfo(
		QByteArray::fromHex(obj[QLatin1String("fingerprint")].toString().toLatin1()),
		QDateTime::fromString(obj[QLatin1String("lastConnected")].toString(), Qt::ISODateWithMs));
	remoteInfo.mName = obj[QLatin1String("name")].toString();
	return remoteInfo;
}


QJsonObject RemoteServiceSettings::RemoteInfo::toJson() const
{
	QJsonObject obj;
	obj[QLatin1String("fingerprint")] = QString::fromLatin1(mFingerprint.toHex());
	obj[QLatin1String("name")] = mName;
	obj[QLatin1String("lastConnected")] = mLastConnected.toString(Qt::ISODateWithMs);
	return obj;
}


const QByteArray& RemoteServiceSettings::RemoteInfo::getFingerprint() const
{
	return mFingerprint;
}


QString RemoteServiceSettings::RemoteInfo::getNameEscaped() const
{
	return RemoteServiceSettings::escapeDeviceName(mName);
}


void RemoteServiceSettings::RemoteInfo::setNameUnescaped(const QString& pName)
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
