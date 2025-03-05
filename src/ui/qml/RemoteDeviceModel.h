/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "RemoteDeviceModelEntry.h"
#include "RemoteServiceSettings.h"

#include <QAbstractListModel>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>


class test_RemoteDeviceModel;
class test_RemoteDeviceFilterModel;


namespace governikus
{

class RemoteDeviceModel
	: public QAbstractListModel
{
	Q_OBJECT
	QML_UNCREATABLE("Used by RemoteServiceModel only")
	QML_ELEMENT

	friend class ::test_RemoteDeviceModel;
	friend class ::test_RemoteDeviceFilterModel;

	private:
		QMap<QByteArray, RemoteServiceSettings::RemoteInfo> mPairedReaders;
		QList<RemoteDeviceModelEntry> mAllRemoteReaders;
		RemoteServiceSettings::RemoteInfo mLastPairedDevice;
		QTimer mTimer;
		bool mIsDetectingRemoteDevices;
#if defined(Q_OS_IOS)
		bool mRemoteDetectionWasRunning;
#endif

		[[nodiscard]] bool indexIsValid(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const;
		[[nodiscard]] QString getCurrentDeviceName(const QModelIndex& pIndex) const;
		[[nodiscard]] QString constructDisplayDeviceName(const QModelIndex& pIndex) const;
		void updatePairedReaders();
		void updateUnpairedReaders();
		void removeVanishedReaders();
		[[nodiscard]] virtual QList<RemoteDeviceModelEntry> presentReaders() const;
		bool addOrUpdateReader(const RemoteDeviceModelEntry& pModelEntry);

	private Q_SLOTS:
#ifdef Q_OS_IOS
		void onApplicationStateChanged(bool pIsAppInForeground);
#else
		void onApplicationStateChanged(bool pIsAppInForeground) const;
#endif
		void onUpdateReaderList();

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		enum SettingsRemoteRoles
		{
			REMOTE_DEVICE_NAME = Qt::UserRole + 1,
			REMOTE_DEVICE_STATUS,
			LAST_CONNECTED,
			DEVICE_ID,
			IS_NETWORK_VISIBLE,
			IS_SUPPORTED,
			IS_PAIRED,
			IS_PAIRING,
			LINK_QUALITY,
			IS_LAST_ADDED_DEVICE
		};

		explicit RemoteDeviceModel(QObject* pParent = nullptr);

		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] QSharedPointer<IfdListEntry> getRemoteDeviceListEntry(const QModelIndex& pIndex) const;
		[[nodiscard]] QSharedPointer<IfdListEntry> getRemoteDeviceListEntry(const QByteArray& pDeviceId) const;
		[[nodiscard]] bool isPaired(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isPairing(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isSupported(const QModelIndex& pIndex) const;
		void forgetDevice(const QModelIndex& pIndex);
		void forgetDevice(const QByteArray& pDeviceId);
		void setLastPairedReader(const QSslCertificate& pCert);
		void startDetection();
		void stopDetection(bool pStopScan);

	public Q_SLOTS:
		void onKnownRemoteReadersChanged();
		void onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireModelChanged();
};


} // namespace governikus
