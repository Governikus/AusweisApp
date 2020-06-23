/*!
 * \brief Model implementation for the remote device table
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
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
		bool mConnected;
		bool mSupported;
		QDateTime mLastConnected;
		QSharedPointer<RemoteDeviceListEntry> mRemoteDeviceListEntry;

	public:
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped, const QString& mId, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry);
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped, const QString& mId, bool pNetworkVisible, bool pConnected, bool pSupported, const QDateTime& pLastConnected, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry);
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped = QStringLiteral("UnknownReader"));

		bool isPaired() const;
		void setPaired(bool pPaired);
		const QString& getId() const;
		void setId(const QString& pId);
		bool isNetworkVisible() const;
		int getLinkQuality() const;
		bool isSupported() const;
		void setNetworkVisible(bool pNetworkVisible);
		const QDateTime& getLastConnected() const;
		void setLastConnected(const QDateTime& pLastConnected);
		bool operator==(const RemoteDeviceModelEntry& pOther) const;

		const QSharedPointer<RemoteDeviceListEntry> getRemoteDeviceListEntry() const;
		QString getDeviceNameEscaped() const;

};

class RemoteDeviceModel
	: public QAbstractTableModel
{
	Q_OBJECT

	Q_PROPERTY(QString emptyListDescriptionString READ getEmptyListDescriptionStringQml NOTIFY fireLanguageChanged)

	private:
		friend class ::test_RemoteDeviceModel;

		const int NUMBER_OF_COLUMNS = 2;

		QMap<QString, RemoteServiceSettings::RemoteInfo> mPairedReaders;
		QVector<RemoteDeviceModelEntry> mAllRemoteReaders;
		const bool mShowPairedReaders;
		const bool mShowUnpairedReaders;
		QTimer mTimer;

		bool indexIsValid(const QModelIndex& pIndex) const;
		QString getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const;
		void updatePairedReaders();
		void updateUnpairedReaders();
		void removeVanishedReaders();
		QVector<RemoteDeviceModelEntry> presentReaders() const;
		bool addOrUpdateReader(const RemoteDeviceModelEntry& pModelEntry);

	private Q_SLOTS:
		void onUpdateReaderList();

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
		const QSharedPointer<RemoteDeviceListEntry> getRemoteDeviceListEntry(const QString& pDeviceId) const;
		bool isPaired(const QModelIndex& pIndex) const;
		bool isSupported(const QModelIndex& pIndex) const;
		void forgetDevice(const QModelIndex& pIndex);
		void forgetDevice(const QString& pDeviceId);

		QString getEmptyListDescriptionStringQml() const;
		QString getEmptyListDescriptionString(bool pWidgetUiHelp = true) const;

	public Q_SLOTS:
		void onUiShown();
		void onUiHidden();
		void onKnownRemoteReadersChanged();
		void onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireModelChanged();
		void fireLanguageChanged();
};


} // namespace governikus
