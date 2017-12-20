/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "ReaderManager.h"

#include <QtGlobal>


using namespace governikus;

RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString pDeviceName, const QString pId, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceName)
	, mId(pId)
	, mPaired(false)
	, mNetworkVisible(false)
	, mLastConnected()
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{

}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString pDeviceName, const QString pId, bool pPaired, bool pNetworkVisible, const QDateTime& pLastConnected)
	: mDeviceName(pDeviceName)
	, mId(pId)
	, mPaired(pPaired)
	, mNetworkVisible(pNetworkVisible)
	, mLastConnected(pLastConnected)
	, mRemoteDeviceListEntry(nullptr)
{

}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString pDeviceName)
	: mDeviceName(pDeviceName)
	, mId()
	, mPaired(false)
	, mNetworkVisible(false)
	, mLastConnected()
	, mRemoteDeviceListEntry(nullptr)
{

}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModelEntry::getRemoteDeviceListEntry() const
{
	return mRemoteDeviceListEntry;
}


QString RemoteDeviceModelEntry::getDeviceName() const
{
	return mDeviceName;
}


bool RemoteDeviceModelEntry::isPaired() const
{
	return mPaired;
}


void RemoteDeviceModelEntry::setPaired(bool pPaired)
{
	mPaired = pPaired;
}


const QString& RemoteDeviceModelEntry::getId() const
{
	return mId;
}


void RemoteDeviceModelEntry::setId(QString pId)
{
	mId = pId;
}


bool RemoteDeviceModelEntry::isNetworkVisible() const
{
	return mNetworkVisible;
}


void RemoteDeviceModelEntry::setNetworkVisible(bool pNetworkVisible)
{
	mNetworkVisible = pNetworkVisible;
}


const QDateTime& RemoteDeviceModelEntry::getLastConnected() const
{
	return mLastConnected;
}


void RemoteDeviceModelEntry::setLastConnected(const QDateTime& pLastConnected)
{
	mLastConnected = pLastConnected;
}


RemoteDeviceModel::RemoteDeviceModel(QObject* pParent, bool pShowPairedReaders, bool pShowUnpairedReaders)
	: QAbstractTableModel(pParent)
	, mPairedReaders()
	, mAllRemoteReaders()
	, mShowPairedReaders(pShowPairedReaders)
	, mShowUnpairedReaders(pShowUnpairedReaders)
{
	RemoteServiceSettings& settings = AppSettings::getInstance().getRemoteServiceSettings();
	connect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &RemoteDeviceModel::onKnownRemoteReadersChanged);
	onKnownRemoteReadersChanged();
}


RemoteDeviceModel::~RemoteDeviceModel()
{
	if (mShowUnpairedReaders)
	{
		onWidgetHidden();
	}
}


QHash<int, QByteArray> RemoteDeviceModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
	roles.insert(REMOTE_DEVICE_NAME, QByteArrayLiteral("remoteDeviceName"));
	roles.insert(LAST_CONNECTED, QByteArrayLiteral("lastConnected"));
	roles.insert(DEVICE_ID, QByteArrayLiteral("deviceId"));
	roles.insert(IS_NETWORK_VISIBLE, QByteArrayLiteral("isNetworkVisible"));
	return roles;
}


QString RemoteDeviceModel::getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const
{
	if (mAllRemoteReaders.isEmpty())
	{
		return tr("Not connected");
	}

	if (pRemoteDeviceModelEntry.isPaired())
	{
		if (pRemoteDeviceModelEntry.isNetworkVisible())
		{
			return tr("Paired and available");
		}
		else
		{
			return tr("Paired and not available");
		}
	}
	else
	{
		return tr("Not paired");
	}
}


QVariant RemoteDeviceModel::headerData(int pSection, Qt::Orientation pOrientation, int pRole) const
{
	if (pRole == Qt::DisplayRole && pOrientation == Qt::Horizontal)
	{
		switch (pSection)
		{
			case ColumnId::ReaderName:
				return tr("Device");

			case ColumnId::ReaderStatus:
				return tr("Status");

			default:
				return QVariant();
		}
	}
	return QVariant();
}


int RemoteDeviceModel::rowCount(const QModelIndex&) const
{
	return mAllRemoteReaders.size();
}


int RemoteDeviceModel::columnCount(const QModelIndex&) const
{
	return NUMBER_OF_COLUMNS;
}


QVariant RemoteDeviceModel::data(const QModelIndex& pIndex, int pRole) const
{
	const auto& reader = mAllRemoteReaders.at(pIndex.row());
	switch (pRole)
	{
		case Qt::DisplayRole:
			switch (pIndex.column())
			{
				case ColumnId::ReaderName:
					return reader.getDeviceName();

				case ColumnId::ReaderStatus:
					return getStatus(reader);
			}

		case REMOTE_DEVICE_NAME:
			return reader.getDeviceName();

		case LAST_CONNECTED:
		{
			const auto& locale = LanguageLoader::getInstance().getUsedLocale();
			const auto& dateTimeFormat = tr("dd.MM.YYYY hh:mm AP");
			return locale.toString(reader.getLastConnected(), dateTimeFormat);
		}

		case DEVICE_ID:
			return reader.getId();

		case IS_NETWORK_VISIBLE:
			return reader.isNetworkVisible();

		default:
			return QVariant();
	}

	Q_UNREACHABLE();
}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(const QModelIndex& pIndex) const
{
	const int row = pIndex.row();
	if (row < 0 || row >= mAllRemoteReaders.size())
	{
		return QSharedPointer<RemoteDeviceListEntry>();
	}

	return mAllRemoteReaders.at(row).getRemoteDeviceListEntry();
}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(QString pDeviceId) const
{
	for (const auto& device : mAllRemoteReaders)
	{
		if (device.getId() == pDeviceId)
		{
			return device.getRemoteDeviceListEntry();
		}
	}
	return QSharedPointer<RemoteDeviceListEntry>();
}


bool RemoteDeviceModel::isPaired(const QModelIndex& pIndex) const
{
	return mAllRemoteReaders.at(pIndex.row()).isPaired();
}


void RemoteDeviceModel::onWidgetShown()
{
	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	connect(remoteClient.data(), &RemoteClient::fireDeviceAppeared, this, &RemoteDeviceModel::constructReaderList);
	connect(remoteClient.data(), &RemoteClient::fireDeviceVanished, this, &RemoteDeviceModel::constructReaderList);

	if (!mShowUnpairedReaders)
	{
		return;
	}

	qDebug() << "Starting Remote Device Detection";
	remoteClient->startDetection();
	constructReaderList();
}


void RemoteDeviceModel::onWidgetHidden()
{
	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	disconnect(remoteClient.data(), &RemoteClient::fireDeviceAppeared, this, &RemoteDeviceModel::constructReaderList);
	disconnect(remoteClient.data(), &RemoteClient::fireDeviceVanished, this, &RemoteDeviceModel::constructReaderList);

	if (!mShowUnpairedReaders)
	{
		return;
	}

	qDebug() << "Stopping Remote Device Detection";
	remoteClient->stopDetection();
}


void RemoteDeviceModel::onKnownRemoteReadersChanged()
{
	mPairedReaders.clear();

	RemoteServiceSettings& settings = AppSettings::getInstance().getRemoteServiceSettings();
	auto pairedReaders = settings.getRemoteInfos();
	for (const auto& reader : pairedReaders)
	{
		mPairedReaders[reader.getFingerprint()] = reader;
	}

	constructReaderList();
}


void RemoteDeviceModel::constructReaderList()
{
	beginResetModel();
	mAllRemoteReaders.clear();

	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();

	if (mShowPairedReaders)
	{
		for (const auto& pairedReader : qAsConst(mPairedReaders))
		{
			bool found = false;
			if (remoteClient)
			{
				const QVector<QSharedPointer<RemoteDeviceListEntry> >& foundDevices = remoteClient->getRemoteDevices();

				for (const auto& foundDevice : foundDevices)
				{
					if (foundDevice && foundDevice->getRemoteDeviceDescriptor().getIfdId() == pairedReader.getFingerprint())
					{
						found = true;
					}
				}
			}

			auto newEntry = RemoteDeviceModelEntry(pairedReader.getName()
					, pairedReader.getFingerprint()
					, true
					, found
					, pairedReader.getLastConnected());
			mAllRemoteReaders.append(newEntry);
		}
	}

	if (mShowUnpairedReaders && remoteClient)
	{
		const QVector<QSharedPointer<RemoteDeviceListEntry> >& remoteDevices = remoteClient->getRemoteDevices();

		for (auto deviceListEntry : remoteDevices)
		{
			if (!mPairedReaders.contains(deviceListEntry->getRemoteDeviceDescriptor().getIfdId()))
			{
				const RemoteDeviceDescriptor& descriptor = deviceListEntry->getRemoteDeviceDescriptor();
				mAllRemoteReaders.append(RemoteDeviceModelEntry(descriptor.getIfdName(), descriptor.getIfdId(), deviceListEntry));
			}
		}
	}

	std::sort(mAllRemoteReaders.begin(), mAllRemoteReaders.end(), [](const RemoteDeviceModelEntry& pFirst, const RemoteDeviceModelEntry& pSecond){
				return pFirst.getDeviceName() < pSecond.getDeviceName();
			});

	endResetModel();
	Q_EMIT fireModelChanged();
}


void RemoteDeviceModel::forgetDevice(const QModelIndex& pIndex)
{
	auto& modelEntry = mAllRemoteReaders.at(pIndex.row());
	const QString& id = modelEntry.getId();

	forgetDevice(id);
}


void RemoteDeviceModel::forgetDevice(const QString& pDeviceId)
{
	RemoteServiceSettings& settings = AppSettings::getInstance().getRemoteServiceSettings();
	settings.removeTrustedCertificate(pDeviceId);
}


void RemoteDeviceModel::onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	Q_UNUSED(pCloseCode);
	Q_UNUSED(pRemoteDispatcher);
	constructReaderList();
}
