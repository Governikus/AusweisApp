/*!
 * \brief Model implementation for the remote device table
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "ReaderConfigurationInfo.h"
#include "RemoteDeviceDescriptor.h"
#include "RemoteDeviceList.h"
#include "RemoteDispatcher.h"
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
	private:
		friend class ::test_RemoteDeviceModel;
		QString mDeviceName;
		QString mId;
		bool mPaired;
		bool mNetworkVisible;
		bool mSupported;
		QDateTime mLastConnected;
		QSharedPointer<RemoteDeviceListEntry> mRemoteDeviceListEntry;

	public:
		RemoteDeviceModelEntry(const QString pDeviceName, const QString mId, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry);
		RemoteDeviceModelEntry(const QString pDeviceName, const QString mId, bool pPaired, bool pNetworkVisible, bool pSupported, const QDateTime& pLastConnected);
		RemoteDeviceModelEntry(const QString pDeviceName = QStringLiteral("UnknownReader"));

		bool isPaired() const;
		void setPaired(bool pPaired);
		const QString& getId() const;
		void setId(QString pId);
		bool isNetworkVisible() const;
		bool isSupported() const;
		void setNetworkVisible(bool pNetworkVisible);
		const QDateTime& getLastConnected() const;
		void setLastConnected(const QDateTime& pLastConnected);

		const QSharedPointer<RemoteDeviceListEntry> getRemoteDeviceListEntry() const;
		QString getDeviceName() const;

};

class RemoteDeviceModel
	: public QAbstractTableModel
{
	Q_OBJECT

	private:
		friend class ::test_RemoteDeviceModel;

		const int NUMBER_OF_COLUMNS = 2;

		QMap<QString, RemoteServiceSettings::RemoteInfo> mPairedReaders;
		QVector<RemoteDeviceModelEntry> mAllRemoteReaders;
		const bool mShowPairedReaders;
		const bool mShowUnpairedReaders;

		QString getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const;
		void constructReaderList();

	public:
		enum SettingsRemoteRoles
		{
			REMOTE_DEVICE_NAME = Qt::UserRole + 1,
			LAST_CONNECTED,
			DEVICE_ID,
			IS_NETWORK_VISIBLE,
			IS_SUPPORTED
		};

		enum ColumnId : int
		{
			ReaderName = 0,
			ReaderStatus = 1
		};

		RemoteDeviceModel(QObject* pParent = nullptr, bool pShowPairedReaders = true, bool pShowUnpairedReaders = true);

		virtual QVariant headerData(int pSection, Qt::Orientation pOrientation, int pRole) const override;
		virtual int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		virtual QHash<int, QByteArray> roleNames() const override;

		const QSharedPointer<RemoteDeviceListEntry> getRemoteDeviceListEntry(const QModelIndex& pIndex) const;
		const QSharedPointer<RemoteDeviceListEntry> getRemoteDeviceListEntry(QString pDeviceId) const;
		bool isPaired(const QModelIndex& pIndex) const;
		bool isSupported(const QModelIndex& pIndex) const;
		void forgetDevice(const QModelIndex& pIndex);
		void forgetDevice(const QString& pDeviceId);

	public Q_SLOTS:
		void onWidgetShown();
		void onWidgetHidden();
		void onKnownRemoteReadersChanged();
		void onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireModelChanged();

};


} // namespace governikus
