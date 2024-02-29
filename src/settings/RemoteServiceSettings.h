/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief RemoteService settings
 */

#pragma once

#include "AbstractSettings.h"

#include <QDateTime>
#include <QList>
#include <QSet>
#include <QSslCertificate>
#include <QSslKey>
#include <QString>

class test_RemoteServiceSettings;
class test_IfdConnector;
class test_RemoteTlsServer;
class test_RemoteWebSocketServer;

namespace governikus
{

class RemoteServiceSettings
	: public AbstractSettings
{
	Q_OBJECT
	friend class AppSettings;
	friend class ::test_RemoteServiceSettings;
	friend class ::test_IfdConnector;
	friend class ::test_RemoteTlsServer;
	friend class ::test_RemoteWebSocketServer;

	public:
		class RemoteInfo
		{
			friend class RemoteServiceSettings;
			friend class ::test_RemoteServiceSettings;

			private:
				QString mFingerprint;
				QString mName;
				QDateTime mLastConnected;

				RemoteInfo(const QString& pFingerprint,
						const QDateTime& pLastConnected);

			public:
				RemoteInfo() = default;

				static RemoteInfo fromJson(const QJsonObject&);
				[[nodiscard]] QJsonObject toJson() const;

				[[nodiscard]] const QString& getFingerprint() const;

				[[nodiscard]] QString getNameEscaped() const;
				void setNameUnescaped(const QString& pName);

				[[nodiscard]] const QDateTime& getLastConnected() const;
				void setLastConnected(const QDateTime& pLastConnected);

				bool operator==(const RemoteInfo& pOther) const;
				bool operator!=(const RemoteInfo& pOther) const;
		};

	private:
		QSharedPointer<QSettings> mStore;

		RemoteServiceSettings();
		[[nodiscard]] QString getDefaultDeviceName() const;
		void setTrustedCertificates(const QList<QSslCertificate>& pCertificates);
		void setUniqueTrustedCertificates(const QSet<QSslCertificate>& pCertificates);

		void setRemoteInfos(const QList<RemoteInfo>& pInfos);
		void syncRemoteInfos(const QSet<QSslCertificate>& pCertificates);

	public:
		static QString generateFingerprint(const QSslCertificate& pCert);
		~RemoteServiceSettings() override = default;

		[[nodiscard]] QString getDeviceName() const;
		void setDeviceName(const QString& pName);

		[[nodiscard]] bool getPinPadMode() const;
		void setPinPadMode(bool pPinPadMode);

		[[nodiscard]] bool getShowAccessRights() const;
		void setShowAccessRights(bool pShowAccessRights);

		[[nodiscard]] QList<QSslCertificate> getTrustedCertificates() const;
		void addTrustedCertificate(const QSslCertificate& pCertificate);
		void removeTrustedCertificate(const QSslCertificate& pCertificate);
		void removeTrustedCertificate(const QString& pFingerprint);

		bool checkAndGenerateKey(int pCreateKeySize) const;

		[[nodiscard]] QList<QSslCertificate> getCertificates() const;
		void setCertificates(const QList<QSslCertificate>& pCertChain) const;

		[[nodiscard]] QSslKey getKey() const;
		void setKey(const QSslKey& pKey) const;

		[[nodiscard]] RemoteInfo getRemoteInfo(const QSslCertificate& pCertificate) const;
		[[nodiscard]] RemoteInfo getRemoteInfo(const QString& pFingerprint) const;
		[[nodiscard]] QList<RemoteInfo> getRemoteInfos() const;
		bool updateRemoteInfo(const RemoteInfo& pInfo);

		static QString escapeDeviceName(const QString& pDeviceNameUnescaped);

	Q_SIGNALS:
		void fireTrustedCertificatesChanged();
		void fireTrustedRemoteInfosChanged();
		void fireInitialDeviceNameSet(const QString& pName);
};


} // namespace governikus

Q_DECLARE_TYPEINFO(governikus::RemoteServiceSettings::RemoteInfo, Q_MOVABLE_TYPE);
