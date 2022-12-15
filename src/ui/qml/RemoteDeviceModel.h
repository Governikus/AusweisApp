/*!
 * \brief Model implementation for the remote device table
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "IfdDescriptor.h"
#include "IfdDispatcher.h"
#include "IfdList.h"
#include "ReaderConfigurationInfo.h"
#include "RemoteServiceSettings.h"

#include <QAbstractTableModel>
#include <QSet>
#include <QSslCertificate>
#include <QVector>

class test_RemoteDeviceModel;

namespace governikus
{

class RemoteDeviceModelEntry
{
	friend class ::test_RemoteDeviceModel;

	private:
		QString mDeviceName;
		QString mId;
		bool mPaired;
		bool mNetworkVisible;
		bool mConnected;
		bool mSupported;
		QDateTime mLastConnected;
		QSharedPointer<IfdListEntry> mRemoteDeviceListEntry;

	public:
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped,
				const QString& mId,
				const QSharedPointer<IfdListEntry>& pRemoteDeviceListEntry);
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped,
				const QString& mId,
				bool pNetworkVisible,
				bool pConnected,
				bool pSupported,
				const QDateTime& pLastConnected,
				const QSharedPointer<IfdListEntry>& pRemoteDeviceListEntry);
		explicit RemoteDeviceModelEntry(const QString& pDeviceNameEscaped = QStringLiteral("UnknownReader"));

		[[nodiscard]] bool isPaired() const;
		void setPaired(bool pPaired);
		[[nodiscard]] const QString& getId() const;
		void setId(const QString& pId);
		[[nodiscard]] bool isNetworkVisible() const;
		[[nodiscard]] int getLinkQuality() const;
		[[nodiscard]] bool isSupported() const;
		void setNetworkVisible(bool pNetworkVisible);
		[[nodiscard]] const QDateTime& getLastConnected() const;
		void setLastConnected(const QDateTime& pLastConnected);
		[[nodiscard]] bool operator==(const RemoteDeviceModelEntry& pOther) const;

		[[nodiscard]] const QSharedPointer<IfdListEntry>& getRemoteDeviceListEntry() const;
		[[nodiscard]] QString getDeviceNameEscaped() const;

};

class RemoteDeviceModel
	: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QString emptyListDescriptionString READ getEmptyListDescriptionString NOTIFY fireModelChanged)
	friend class ::test_RemoteDeviceModel;

	private:
		QMap<QString, RemoteServiceSettings::RemoteInfo> mPairedReaders;
		QVector<RemoteDeviceModelEntry> mAllRemoteReaders;
		const bool mShowPairedReaders;
		const bool mShowUnpairedReaders;
		QTimer mTimer;
		bool mIsDetectingRemoteDevices;
#if defined(Q_OS_IOS)
		bool mRemoteDetectionWasRunning;
#endif

		[[nodiscard]] bool indexIsValid(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const;
		void updatePairedReaders();
		void updateUnpairedReaders();
		void removeVanishedReaders();
		[[nodiscard]] QVector<RemoteDeviceModelEntry> presentReaders() const;
		bool addOrUpdateReader(const RemoteDeviceModelEntry& pModelEntry);

	private Q_SLOTS:
		void onApplicationStateChanged(bool pIsAppInForeground);
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
			LINK_QUALITY
		};

		RemoteDeviceModel(QObject* pParent = nullptr, bool pShowPairedReaders = true, bool pShowUnpairedReaders = true);

		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] QSharedPointer<IfdListEntry> getRemoteDeviceListEntry(const QModelIndex& pIndex) const;
		[[nodiscard]] QSharedPointer<IfdListEntry> getRemoteDeviceListEntry(const QString& pDeviceId) const;
		[[nodiscard]] bool isPaired(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isSupported(const QModelIndex& pIndex) const;
		void forgetDevice(const QModelIndex& pIndex);
		void forgetDevice(const QString& pDeviceId);

		[[nodiscard]] QString getEmptyListDescriptionString() const;

	public Q_SLOTS:
		void setDetectRemoteDevices(bool pNewStatus);
		void onKnownRemoteReadersChanged();
		void onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireModelChanged();
};


} // namespace governikus
