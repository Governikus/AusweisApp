/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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

RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceNameEscaped, const QString& pId, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceNameEscaped)
	, mId(pId)
	, mPaired(false)
	, mNetworkVisible(false)
	, mConnected(false)
	, mSupported(pRemoteDeviceListEntry->getRemoteDeviceDescriptor().isSupported())
	, mLastConnected()
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceNameEscaped, const QString& pId, bool pNetworkVisible, bool pConnected, bool pSupported, const QDateTime& pLastConnected, QSharedPointer<RemoteDeviceListEntry>& pRemoteDeviceListEntry)
	: mDeviceName(pDeviceNameEscaped)
	, mId(pId)
	, mPaired(true)
	, mNetworkVisible(pNetworkVisible)
	, mConnected(pConnected)
	, mSupported(pSupported)
	, mLastConnected(pLastConnected)
	, mRemoteDeviceListEntry(pRemoteDeviceListEntry)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


RemoteDeviceModelEntry::RemoteDeviceModelEntry(const QString& pDeviceNameEscaped)
	: mDeviceName(pDeviceNameEscaped)
	, mId()
	, mPaired(false)
	, mNetworkVisible(false)
	, mConnected(false)
	, mSupported(false)
	, mLastConnected()
	, mRemoteDeviceListEntry(nullptr)
{
	Q_ASSERT(!mDeviceName.contains(QLatin1Char('<')));
}


const QSharedPointer<RemoteDeviceListEntry> RemoteDeviceModelEntry::getRemoteDeviceListEntry() const
{
	return mRemoteDeviceListEntry;
}


QString RemoteDeviceModelEntry::getDeviceNameEscaped() const
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
	if (mConnected)
	{
		return 100;
	}

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


bool RemoteDeviceModelEntry::operator==(const RemoteDeviceModelEntry& pOther) const
{
	return getId() == pOther.getId();
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
	connect(remoteClient, &RemoteClient::fireDeviceAppeared, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(remoteClient, &RemoteClient::fireDeviceUpdated, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(remoteClient, &RemoteClient::fireDeviceVanished, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(remoteClient, &RemoteClient::fireDispatcherDestroyed, this, &RemoteDeviceModel::onUpdateReaderList);
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


void RemoteDeviceModel::updatePairedReaders()
{
	const auto& availableReaders = presentReaders();
	const auto& connectedDeviceIDs = Env::getSingleton<RemoteClient>()->getConnectedDeviceIDs();

	for (const auto& pairedReader : qAsConst(mPairedReaders))
	{
		bool visible = false;
		bool connected = false;
		bool supported = true;
		QSharedPointer<RemoteDeviceListEntry> deviceListEntry;
		for (const auto& availableReader : availableReaders)
		{
			if (pairedReader.getFingerprint() == availableReader.getId())
			{
				visible = true;
				connected = connectedDeviceIDs.contains(availableReader.getId());
				supported = availableReader.isSupported();
				deviceListEntry = availableReader.getRemoteDeviceListEntry();
				break;
			}
		}

		if (!visible && connectedDeviceIDs.contains(pairedReader.getFingerprint()))
		{
			visible = true;
			connected = true;
			supported = true;
		}
		auto modelEntry = RemoteDeviceModelEntry(
				pairedReader.getNameEscaped(),
				pairedReader.getFingerprint(),
				visible,
				connected,
				supported,
				pairedReader.getLastConnected(),
				deviceListEntry);
		addOrUpdateReader(modelEntry);
	}
}


void RemoteDeviceModel::updateUnpairedReaders()
{
	const auto availableReaders = presentReaders();
	for (const auto& modelEntry : availableReaders)
	{
		if (mPairedReaders.contains(modelEntry.getId()))
		{
			continue;
		}

		addOrUpdateReader(modelEntry);
	}
}


void RemoteDeviceModel::removeVanishedReaders()
{
	const auto availableReaders = presentReaders();
	for (int i = mAllRemoteReaders.size() - 1; i >= 0; --i)
	{
		const auto& reader = mAllRemoteReaders.value(i);
		const bool readerIsPaired = mPairedReaders.contains(reader.getId());
		if (mShowPairedReaders && readerIsPaired)
		{
			continue;
		}

		const bool readerIsAvailable = availableReaders.contains(reader);
		const bool showOnlyPairedReader = mShowPairedReaders && !mShowUnpairedReaders;
		const bool showOnlyUnpairedReader = mShowUnpairedReaders && !mShowPairedReaders;
		if (!readerIsAvailable || (showOnlyPairedReader && !readerIsPaired) || (showOnlyUnpairedReader && readerIsPaired))
		{
			beginRemoveRows(QModelIndex(), i, i);
			mAllRemoteReaders.remove(i);
			endRemoveRows();
		}
	}
}


QVector<RemoteDeviceModelEntry> RemoteDeviceModel::presentReaders() const
{
	const auto& announcingRemoteDevices = Env::getSingleton<RemoteClient>()->getAnnouncingRemoteDevices();
	QVector<RemoteDeviceModelEntry> presentReaders;

	for (auto deviceListEntry : announcingRemoteDevices)
	{
		const auto& deviceDescriptor = deviceListEntry->getRemoteDeviceDescriptor();
		auto modelEntry = RemoteDeviceModelEntry(RemoteServiceSettings::escapeDeviceName(deviceDescriptor.getIfdName()), deviceDescriptor.getIfdId(), deviceListEntry);
		presentReaders.append(modelEntry);
	}

	return presentReaders;
}


bool RemoteDeviceModel::addOrUpdateReader(const RemoteDeviceModelEntry& pModelEntry)
{
	if (!mAllRemoteReaders.contains(pModelEntry))
	{
		const int readerCount = mAllRemoteReaders.size();
		beginInsertRows(index(readerCount, 0), readerCount, readerCount);
		mAllRemoteReaders.append(pModelEntry);
		endInsertRows();
		return true;
	}

	const int readerIndex = mAllRemoteReaders.indexOf(pModelEntry);
	mAllRemoteReaders[readerIndex] = pModelEntry;
	const auto modelIndex = index(readerIndex, 0);
	Q_EMIT dataChanged(modelIndex, modelIndex);
	return false;
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
				return reader.getDeviceNameEscaped();
			}

		case REMOTE_DEVICE_NAME:
			return reader.getDeviceNameEscaped();

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
	onUpdateReaderList();
}


void RemoteDeviceModel::onUiHidden()
{
	if (!mShowUnpairedReaders)
	{
		return;
	}

	qDebug() << "Stopping Remote Device Detection";
	Env::getSingleton<RemoteClient>()->stopDetection();
	onUpdateReaderList();
}


void RemoteDeviceModel::onKnownRemoteReadersChanged()
{
	mPairedReaders.clear();

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto pairedReaders = settings.getRemoteInfos();
	for (const auto& reader : pairedReaders)
	{
		mPairedReaders[reader.getFingerprint()] = reader;
	}

	onUpdateReaderList();
}


void RemoteDeviceModel::onUpdateReaderList()
{
	if (mShowPairedReaders)
	{
		updatePairedReaders();
	}

	if (mShowUnpairedReaders)
	{
		updateUnpairedReaders();
	}

	removeVanishedReaders();

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
	onKnownRemoteReadersChanged();
}


QString RemoteDeviceModel::getEmptyListDescriptionString() const
{
	const QString& onlineHelpSection = QStringLiteral("settingsRemoteReader");
	const QString& url = HelpAction::getOnlineUrl(onlineHelpSection);
	//: Is embedded in a sentence.
	const QString& hyperlink = QStringLiteral("<a href=\"%1\">%2</a>").arg(url, tr("online help"));
	//: INFO ALL_PLATFORMS No smartphone with enabled remote service was found on the same network.
	return tr("No smartphone as card reader (Sac) available. Please make sure to activate the \"remote service\" on your smartphone and to connect both devices to the same WiFi. See %1 for details of use.").arg(hyperlink);
}


void RemoteDeviceModel::onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId)
{
	Q_UNUSED(pCloseCode)
	Q_UNUSED(pId)
	onUpdateReaderList();
}
