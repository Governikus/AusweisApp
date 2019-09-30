/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "GeneralSettings.h"
#include "HelpAction.h"
#include "LanguageLoader.h"
#include "RemoteClient.h"

#include <QtGlobal>


using namespace governikus;

RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceName, const QString& pId, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceName)
	, mId(pId)
	, mPaired(false)
	, mNetworkVisible(false)
	, mSupported(pRemoteDeviceListEntry->getRemoteDeviceDescriptor().isSupported())
	, mLastConnected()
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceName, const QString& pId, bool pPaired, bool pNetworkVisible, bool pSupported, const QDateTime& pLastConnected, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceName)
	, mId(pId)
	, mPaired(pPaired)
	, mNetworkVisible(pNetworkVisible)
	, mSupported(pSupported)
	, mLastConnected(pLastConnected)
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceName)
	: mDeviceName(pDeviceName)
	, mId()
	, mPaired(false)
	, mNetworkVisible(false)
	, mSupported(false)
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


void RemoteDeviceModelEntry::setId(const QString& pId)
{
	mId = pId;
}


bool RemoteDeviceModelEntry::isNetworkVisible() const
{
	return mNetworkVisible;
}


int RemoteDeviceModelEntry::getLinkQuality() const
{
	if (mRemoteDeviceListEntry.isNull())
	{
		return 0;
	}
	return mRemoteDeviceListEntry->getPercentSeen();
}


bool RemoteDeviceModelEntry::isSupported() const
{
	return mSupported;
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
	, mTimer()
{
	AppSettings* const appSettings = Env::getSingleton<AppSettings>();

	const GeneralSettings& generalSettings = appSettings->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &RemoteDeviceModel::fireLanguageChanged);
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &RemoteDeviceModel::onKnownRemoteReadersChanged);

	const RemoteServiceSettings& settings = appSettings->getRemoteServiceSettings();
	connect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &RemoteDeviceModel::onKnownRemoteReadersChanged);
	onKnownRemoteReadersChanged();

	const auto& remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireDeviceAppeared, this, &RemoteDeviceModel::onConstructReaderList);
	connect(remoteClient, &RemoteClient::fireDeviceUpdated, this, &RemoteDeviceModel::onConstructReaderList);
	connect(remoteClient, &RemoteClient::fireDeviceVanished, this, &RemoteDeviceModel::onConstructReaderList);
}


QHash<int, QByteArray> RemoteDeviceModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
	roles.insert(REMOTE_DEVICE_NAME, QByteArrayLiteral("remoteDeviceName"));
	roles.insert(REMOTE_DEVICE_STATUS, QByteArrayLiteral("remoteDeviceStatus"));
	roles.insert(LAST_CONNECTED, QByteArrayLiteral("lastConnected"));
	roles.insert(DEVICE_ID, QByteArrayLiteral("deviceId"));
	roles.insert(IS_NETWORK_VISIBLE, QByteArrayLiteral("isNetworkVisible"));
	roles.insert(IS_SUPPORTED, QByteArrayLiteral("isSupported"));
	roles.insert(IS_PAIRED, QByteArrayLiteral("isPaired"));
	roles.insert(LINK_QUALITY, QByteArrayLiteral("linkQualityInPercent"));
	return roles;
}


bool RemoteDeviceModel::indexIsValid(const QModelIndex& pIndex) const
{
	if (!pIndex.isValid())
	{
		Q_ASSERT(false && "Invoked with an invalid QModelIndex.");
		return false;
	}

	if (pIndex.row() >= rowCount() || pIndex.column() >= columnCount())
	{
		Q_ASSERT(false && "Invoked with a row or a column which is out of bounds.");
		return false;
	}

	return true;
}


QString RemoteDeviceModel::getStatus(const RemoteDeviceModelEntry& pRemoteDeviceModelEntry) const
{
	if (mAllRemoteReaders.isEmpty())
	{
		//: LABEL ALL_PLATFORMS
		return tr("Not connected");
	}

	if (pRemoteDeviceModelEntry.isPaired())
	{
		if (pRemoteDeviceModelEntry.isNetworkVisible())
		{
			if (pRemoteDeviceModelEntry.isSupported())
			{
				//: LABEL ALL_PLATFORMS
				return tr("Available");
			}
			//: LABEL ALL_PLATFORMS
			return tr("Paired, but unsupported");
		}
		//: LABEL ALL_PLATFORMS
		return tr("Paired, but unavailable");
	}

	if (!pRemoteDeviceModelEntry.isSupported())
	{
		//: LABEL ALL_PLATFORMS
		return tr("Unsupported");
	}

	//: LABEL ALL_PLATFORMS
	return tr("Not paired");
}


QVariant RemoteDeviceModel::headerData(int pSection, Qt::Orientation pOrientation, int pRole) const
{
	if (pRole == Qt::DisplayRole && pOrientation == Qt::Horizontal)
	{
		switch (pSection)
		{
			case ColumnId::ReaderName:
				//: LABEL ALL_PLATFORMS
				return tr("Device");

			case ColumnId::ReaderStatus:
				//: LABEL ALL_PLATFORMS
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
	if (!indexIsValid(pIndex))
	{
		return QVariant();
	}

	const auto& reader = mAllRemoteReaders.at(pIndex.row());
	switch (pRole)
	{
		case Qt::DisplayRole:
			if (pIndex.column() == ColumnId::ReaderStatus)
			{
				return getStatus(reader);
			}
			else
			{
				return reader.getDeviceName();
			}

		case REMOTE_DEVICE_NAME:
			return reader.getDeviceName();

		case REMOTE_DEVICE_STATUS:
			return getStatus(reader);

		case LAST_CONNECTED:
		{
			const auto& locale = LanguageLoader::getInstance().getUsedLocale();
			const auto& dateTimeFormat = tr("dd.MM.yyyy hh:mm AP");
			return locale.toString(reader.getLastConnected(), dateTimeFormat);
		}

		case DEVICE_ID:
			return reader.getId();

		case IS_NETWORK_VISIBLE:
			return reader.isNetworkVisible();

		case IS_SUPPORTED:
			return reader.isSupported();

		case IS_PAIRED:
			return isPaired(pIndex);

		case LINK_QUALITY:
			return reader.getLinkQuality();
	}

	return QVariant();
}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return QSharedPointer<RemoteDeviceListEntry>();
	}

	const int row = pIndex.row();
	return mAllRemoteReaders.at(row).getRemoteDeviceListEntry();
}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(const QString& pDeviceId) const
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
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	return mAllRemoteReaders.at(pIndex.row()).isPaired();
}


bool RemoteDeviceModel::isSupported(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	return mAllRemoteReaders.at(pIndex.row()).isSupported();
}


void RemoteDeviceModel::onUiShown()
{
	if (!mShowUnpairedReaders)
	{
		return;
	}

	qDebug() << "Starting Remote Device Detection";
	Env::getSingleton<RemoteClient>()->startDetection();
	onConstructReaderList();
}


void RemoteDeviceModel::onUiHidden()
{
	if (!mShowUnpairedReaders)
	{
		return;
	}

	qDebug() << "Stopping Remote Device Detection";
	Env::getSingleton<RemoteClient>()->stopDetection();
	onConstructReaderList();
}


void RemoteDeviceModel::onKnownRemoteReadersChanged()
{
	mPairedReaders.clear();

	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto pairedReaders = settings.getRemoteInfos();
	for (const auto& reader : pairedReaders)
	{
		mPairedReaders[reader.getFingerprint()] = reader;
	}

	onConstructReaderList();
}


void RemoteDeviceModel::onConstructReaderList()
{
	beginResetModel();
	mAllRemoteReaders.clear();

	auto* const remoteClient = Env::getSingleton<RemoteClient>();

	if (mShowPairedReaders)
	{
		const QVector<QSharedPointer<RemoteDeviceListEntry> >& announcingDevices = remoteClient->getAnnouncingRemoteDevices();

		for (const auto& pairedReader : qAsConst(mPairedReaders))
		{
			bool networkVisible = false;
			bool supported = true;
			QSharedPointer<RemoteDeviceListEntry> deviceListEntry;

			for (const auto& announcingDevice : announcingDevices)
			{
				if (announcingDevice && announcingDevice->getRemoteDeviceDescriptor().getIfdId() == pairedReader.getFingerprint())
				{
					networkVisible = true;
					supported = announcingDevice->getRemoteDeviceDescriptor().isSupported();
					deviceListEntry = announcingDevice;

					break;
				}
			}

			if (!networkVisible)
			{
				const QStringList& connectedDeviceIDs = remoteClient->getConnectedDeviceIDs();
				for (const auto& id : connectedDeviceIDs)
				{
					if (id == pairedReader.getFingerprint())
					{
						networkVisible = true;
						supported = true;

						break;
					}
				}
			}

			auto newEntry = RemoteDeviceModelEntry(pairedReader.getName()
					, pairedReader.getFingerprint()
					, true
					, networkVisible
					, supported
					, pairedReader.getLastConnected()
					, deviceListEntry);
			mAllRemoteReaders.append(newEntry);
		}
	}

	if (mShowUnpairedReaders)
	{
		const QVector<QSharedPointer<RemoteDeviceListEntry> >& announcingRemoteDevices = remoteClient->getAnnouncingRemoteDevices();

		for (auto deviceListEntry : announcingRemoteDevices)
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
	if (!indexIsValid(pIndex))
	{
		return;
	}

	auto& modelEntry = mAllRemoteReaders.at(pIndex.row());
	const QString& id = modelEntry.getId();

	forgetDevice(id);
}


void RemoteDeviceModel::forgetDevice(const QString& pDeviceId)
{
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	settings.removeTrustedCertificate(pDeviceId);
}


QString RemoteDeviceModel::getEmptyListDescriptionString() const
{
	const QString& url = HelpAction::getOnlineUrl(QStringLiteral("readerDeviceTab"));
	//: Is embedded in a sentence.
	const QString& hyperlink = QStringLiteral("<a href=\"%1\">%2</a>").arg(url, tr("online help"));
	//: INFO ALL_PLATFORMS No smartphone with enabled remote service was found on the same network.
	return tr("No smartphone with enabled remote service found. See %1 for details of use.").arg(hyperlink);
}


void RemoteDeviceModel::onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId)
{
	Q_UNUSED(pCloseCode)
	Q_UNUSED(pId)
	onConstructReaderList();
}
