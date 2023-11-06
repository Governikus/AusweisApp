/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModel.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "LanguageLoader.h"
#include "RemoteIfdClient.h"


using namespace governikus;


RemoteDeviceModel::RemoteDeviceModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mPairedReaders()
	, mAllRemoteReaders()
	, mTimer()
	, mIsDetectingRemoteDevices(false)
#if defined(Q_OS_IOS)
	, mRemoteDetectionWasRunning(false)
#endif
{
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	connect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &RemoteDeviceModel::onKnownRemoteReadersChanged);
	onKnownRemoteReadersChanged();

	const auto& ifdClient = Env::getSingleton<RemoteIfdClient>();
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(ifdClient, &IfdClient::fireDeviceUpdated, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(ifdClient, &IfdClient::fireDeviceVanished, this, &RemoteDeviceModel::onUpdateReaderList);
	connect(ifdClient, &IfdClient::fireDispatcherDestroyed, this, &RemoteDeviceModel::onUpdateReaderList);

	const auto& applicationModel = Env::getSingleton<ApplicationModel>();
	connect(applicationModel, &ApplicationModel::fireApplicationStateChanged, this, &RemoteDeviceModel::onApplicationStateChanged);
}


QHash<int, QByteArray> RemoteDeviceModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(REMOTE_DEVICE_NAME, QByteArrayLiteral("remoteDeviceName"));
	roles.insert(REMOTE_DEVICE_STATUS, QByteArrayLiteral("remoteDeviceStatus"));
	roles.insert(LAST_CONNECTED, QByteArrayLiteral("lastConnected"));
	roles.insert(DEVICE_ID, QByteArrayLiteral("deviceId"));
	roles.insert(IS_NETWORK_VISIBLE, QByteArrayLiteral("isNetworkVisible"));
	roles.insert(IS_SUPPORTED, QByteArrayLiteral("isSupported"));
	roles.insert(IS_PAIRED, QByteArrayLiteral("isPaired"));
	roles.insert(IS_PAIRING, QByteArrayLiteral("isPairing"));
	roles.insert(LINK_QUALITY, QByteArrayLiteral("linkQualityInPercent"));
	roles.insert(IS_LAST_ADDED_DEVICE, QByteArrayLiteral("isLastAddedDevice"));
	return roles;
}


bool RemoteDeviceModel::indexIsValid(const QModelIndex& pIndex) const
{
	if (!pIndex.isValid())
	{
		Q_ASSERT(false && "Invoked with an invalid QModelIndex.");
		return false;
	}

	if (pIndex.row() >= rowCount())
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

	if (pRemoteDeviceModelEntry.isPairing())
	{
		//: LABEL ALL_PLATFORMS
		return tr("Click to pair");
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
		return tr("Unavailable");
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
	const auto& connectedDeviceIDs = Env::getSingleton<RemoteIfdClient>()->getConnectedDeviceIDs();

	for (const auto& pairedReader : std::as_const(mPairedReaders))
	{
		bool visible = false;
		bool connected = false;
		bool supported = true;
		bool isPairing = false;
		QSharedPointer<IfdListEntry> deviceListEntry;
		for (const auto& availableReader : availableReaders)
		{
			if (pairedReader.getFingerprint() == availableReader.getId())
			{
				visible = true;
				connected = connectedDeviceIDs.contains(availableReader.getId());
				supported = availableReader.isSupported();
				isPairing = availableReader.isPairing();
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
				isPairing,
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
	for (int i = static_cast<int>(mAllRemoteReaders.size()) - 1; i >= 0; --i)
	{
		const auto& reader = mAllRemoteReaders.value(i);
		const bool readerIsPaired = mPairedReaders.contains(reader.getId());
		if (readerIsPaired)
		{
			continue;
		}

		const bool readerIsAvailable = availableReaders.contains(reader);
		if (!readerIsAvailable)
		{
			beginRemoveRows(QModelIndex(), i, i);
			mAllRemoteReaders.remove(i);
			endRemoveRows();
		}
	}
}


QVector<RemoteDeviceModelEntry> RemoteDeviceModel::presentReaders() const
{
	const auto& announcingRemoteDevices = Env::getSingleton<RemoteIfdClient>()->getAnnouncingRemoteDevices();
	QVector<RemoteDeviceModelEntry> presentReaders;

	for (auto& deviceListEntry : announcingRemoteDevices)
	{
		auto modelEntry = RemoteDeviceModelEntry(deviceListEntry);
		presentReaders.append(modelEntry);
	}

	return presentReaders;
}


bool RemoteDeviceModel::addOrUpdateReader(const RemoteDeviceModelEntry& pModelEntry)
{
	if (!mAllRemoteReaders.contains(pModelEntry))
	{
		const int readerCount = static_cast<int>(mAllRemoteReaders.size());
		beginInsertRows(index(readerCount, 0), readerCount, readerCount);
		mAllRemoteReaders.append(pModelEntry);
		endInsertRows();
		return true;
	}

	const auto readerIndex = mAllRemoteReaders.indexOf(pModelEntry);
	mAllRemoteReaders[readerIndex] = pModelEntry;
	const auto modelIndex = index(static_cast<int>(readerIndex), 0);
	Q_EMIT dataChanged(modelIndex, modelIndex);
	return false;
}


int RemoteDeviceModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mAllRemoteReaders.size());
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
		case REMOTE_DEVICE_NAME:
			return reader.getDeviceNameEscaped();

		case REMOTE_DEVICE_STATUS:
			return getStatus(reader);

		case LAST_CONNECTED:
		{
			const auto& locale = LanguageLoader::getInstance().getUsedLocale();

			//: LABEL ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString
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

		case IS_PAIRING:
			return isPairing(pIndex);

		case LINK_QUALITY:
			return reader.getLinkQuality();

		case IS_LAST_ADDED_DEVICE:
			return mLastPairedDevice.getFingerprint() == reader.getId();
	}

	return QVariant();
}


QSharedPointer<IfdListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return QSharedPointer<IfdListEntry>();
	}

	const int row = pIndex.row();
	return mAllRemoteReaders.at(row).getRemoteDeviceListEntry();
}


QSharedPointer<IfdListEntry> RemoteDeviceModel::getRemoteDeviceListEntry(const QString& pDeviceId) const
{
	for (const auto& device : mAllRemoteReaders)
	{
		if (device.getId() == pDeviceId)
		{
			return device.getRemoteDeviceListEntry();
		}
	}
	return QSharedPointer<IfdListEntry>();
}


bool RemoteDeviceModel::isPaired(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	return mAllRemoteReaders.at(pIndex.row()).isPaired();
}


bool RemoteDeviceModel::isPairing(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	return mAllRemoteReaders.at(pIndex.row()).isPairing();
}


bool RemoteDeviceModel::isSupported(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	return mAllRemoteReaders.at(pIndex.row()).isSupported();
}


void RemoteDeviceModel::setDetectRemoteDevices(bool pNewStatus)
{
	if (mIsDetectingRemoteDevices == pNewStatus)
	{
		return;
	}

	mIsDetectingRemoteDevices = pNewStatus;

	if (mIsDetectingRemoteDevices)
	{
		qDebug() << "Starting Remote Device Detection";
		Env::getSingleton<RemoteIfdClient>()->startDetection();
	}
	else
	{
		qDebug() << "Stopping Remote Device Detection";
		Env::getSingleton<RemoteIfdClient>()->stopDetection();
	}

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


void RemoteDeviceModel::onApplicationStateChanged(bool pIsAppInForeground)
{
	if (!mIsDetectingRemoteDevices)
	{
		return;
	}

#if defined(Q_OS_IOS)
	if (pIsAppInForeground)
	{
		if (mRemoteDetectionWasRunning)
		{
			Env::getSingleton<RemoteIfdClient>()->startDetection();
			mRemoteDetectionWasRunning = false;
		}
	}
	else
	{
		mRemoteDetectionWasRunning = Env::getSingleton<RemoteIfdClient>()->isDetecting();
		Env::getSingleton<RemoteIfdClient>()->stopDetection();
	}
#else
	Q_UNUSED(pIsAppInForeground)
#endif
}


void RemoteDeviceModel::onUpdateReaderList()
{
	updatePairedReaders();

	updateUnpairedReaders();

	removeVanishedReaders();

	Q_EMIT fireModelChanged();
}


void RemoteDeviceModel::onTranslationChanged()
{
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


void RemoteDeviceModel::setLastPairedReader(const QSslCertificate& pCert)
{
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	mLastPairedDevice = settings.getRemoteInfo(pCert);
	Q_EMIT dataChanged(index(0), index(rowCount() - 1), {IS_LAST_ADDED_DEVICE});
}


void RemoteDeviceModel::onDeviceDisconnected(GlobalStatus::Code pCloseCode, const QString& pId)
{
	Q_UNUSED(pCloseCode)
	Q_UNUSED(pId)
	onUpdateReaderList();
}
