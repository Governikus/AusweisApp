/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "ReaderModel.h"

#include "Env.h"
#include "LanguageLoader.h"
#include "ReaderConfiguration.h"
#include "ReaderManager.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include "ReaderDetector.h"
#endif

#include <QQmlEngine>


using namespace governikus;


QString ReaderModel::getLastUpdatedInformation() const
{
	if (!mConnectedReadersUpdateTime.isValid())
	{
		return QString();
	}

	//: ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString
	const auto& updateTime = LanguageLoader::getInstance().getUsedLocale().toString(mConnectedReadersUpdateTime, tr("hh:mm:ss AP"));
	//: ALL_PLATFORMS
	return tr("The list of card readers was last updated at %1.").arg(updateTime);
}


SortedReaderModel* ReaderModel::getSortedModel()
{
	return &mSortedModel;
}


QList<ReaderConfigurationInfo> ReaderModel::collectReaderData()
{
	QList<ReaderConfigurationInfo> connectedReaders;

	const QList<ReaderInfo> installedReaders = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({
				ReaderManagerPluginType::PCSC, ReaderManagerPluginType::NFC
			}));

	for (const auto& installedReader : installedReaders)
	{
		const auto& readerSettingsInfo = installedReader.getReaderConfigurationInfo();
		mKnownDrivers += readerSettingsInfo;
		connectedReaders += readerSettingsInfo;
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	QList<ReaderConfigurationInfo> readersWithoutDriver;
	const auto& attachedSupportedDevices = Env::getSingleton<ReaderDetector>()->getAttachedSupportedDevices();
	for (const auto& info : attachedSupportedDevices)
	{
		if (!connectedReaders.contains(info))
		{
			readersWithoutDriver.append(info);
		}
	}
	connectedReaders += readersWithoutDriver;
#endif

	return connectedReaders;
}


bool ReaderModel::indexIsValid(const QModelIndex& pIndex) const
{
	if (!pIndex.isValid())
	{
		Q_ASSERT(false && "Invoked with an invalid QModelIndex.");
		return false;
	}

	if (pIndex.row() >= rowCount(pIndex))
	{
		Q_ASSERT(false && "Invoked with a row which is out of bounds.");
		return false;
	}

	return true;
}


QUrl ReaderModel::getReaderImageUrl(const QModelIndex& pIndex) const
{
	return mConnectedReaders.at(pIndex.row()).getIcon()->lookupUrl();
}


QString ReaderModel::getHTMLDescription(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return QString();
	}

	if (mConnectedReaders.isEmpty())
	{
		return QString();
	}

	if (!isPcscScanRunning())
	{
		//: ALL_PLATFORMS
		return tr("The smartcard service of your system is not reachable.");
	}

	if (isSupportedReader(pIndex))
	{
		if (isInstalledReader(pIndex))
		{
			//: ALL_PLATFORMS
			return tr("Driver installed");
		}
		//: ALL_PLATFORMS
		return tr("No driver installed") + QStringLiteral("<br>") +
		       //: ALL_PLATFORMS
			   tr("Please download and install the driver you can find at:");
	}

	//: ALL_PLATFORMS
	return tr("This card reader is not officially supported and may not work as expected.");
}


bool ReaderModel::isSupportedReader(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	const auto& readerSettingsInfo = mConnectedReaders.at(pIndex.row());
	return readerSettingsInfo.isKnownReader();
}


bool ReaderModel::isInstalledReader(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex) || !isPcscScanRunning())
	{
		return false;
	}

	const auto& readerSettingsInfo = mConnectedReaders.at(pIndex.row());
	return mKnownDrivers.contains(readerSettingsInfo);
}


bool ReaderModel::isPcscScanRunning() const
{
	return Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::PCSC).isScanRunning();
}


void ReaderModel::onUpdateContent()
{
	const int oldCount = ReaderModel::rowCount();
	const auto connectedReaders = collectReaderData();
	const int newCount = std::max(static_cast<int>(connectedReaders.size()), 1);

	mConnectedReadersUpdateTime = QTime::currentTime();

	if (oldCount == newCount)
	{
		mConnectedReaders = connectedReaders;
	}
	else if (newCount > oldCount)
	{
		beginInsertRows(QModelIndex(), oldCount, newCount - 1);
		mConnectedReaders = connectedReaders;
		endInsertRows();
	}
	else
	{
		beginRemoveRows(QModelIndex(), newCount, oldCount - 1);
		mConnectedReaders = connectedReaders;
		endRemoveRows();
	}

	Q_EMIT dataChanged(ReaderModel::index(0, 0), ReaderModel::index(ReaderModel::rowCount() - 1, 0));
	Q_EMIT fireModelChanged();
}


ReaderModel::ReaderModel()
	: QAbstractListModel()
	, mKnownDrivers()
	, mConnectedReaders()
	, mConnectedReadersUpdateTime()
	, mSortedModel()
{
	const ReaderManager* const readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ReaderModel::onUpdateContent);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ReaderModel::onUpdateContent);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ReaderModel::onUpdateContent);
	connect(Env::getSingleton<ReaderConfiguration>(), &ReaderConfiguration::fireUpdated, this, &ReaderModel::onUpdateContent);
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	connect(Env::getSingleton<ReaderDetector>(), &ReaderDetector::fireReaderChangeDetected, this, &ReaderModel::onUpdateContent);
#endif

	mSortedModel.setSourceModel(this);
	QQmlEngine::setObjectOwnership(&mSortedModel, QQmlEngine::CppOwnership);
	connect(this, &ReaderModel::fireModelChanged, &mSortedModel, &SortedReaderModel::onDataChanged);

	onUpdateContent();
}


int ReaderModel::rowCount(const QModelIndex&) const
{
	return std::max(static_cast<int>(mConnectedReaders.size()), 1);
}


QVariant ReaderModel::handleDummyReaderInfo(int pRole) const
{
	switch (pRole)
	{
		case ReaderModel::READER_NAME:
			//: DESKTOP
			return tr("No card reader connected");

		case ReaderModel::READER_IMAGE_PATH:
			return QStringLiteral("qrc:///images/desktop/default_reader.png");

		case ReaderModel::READER_INSTALLED:
		case ReaderModel::SHOW_STATUS_ICON:
		case ReaderModel::READER_SUPPORTED:
			return false;

		default:
			return QString();
	}
}


QVariant ReaderModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!indexIsValid(pIndex))
	{
		return QVariant();
	}

	if (mConnectedReaders.isEmpty())
	{
		return handleDummyReaderInfo(pRole);
	}

	const auto& reader = mConnectedReaders.at(pIndex.row());
	switch (pRole)
	{
		case READER_NAME:
			return reader.getName();

		case READER_IMAGE_PATH:
			return getReaderImageUrl(pIndex);

		case READER_HTML_DESCRIPTION:
			return getHTMLDescription(pIndex);

		case READER_DRIVER_URL:
		{
			if (isSupportedReader(pIndex) && !isInstalledReader(pIndex))
			{
				return mConnectedReaders.at(pIndex.row()).getUrl();
			}
			return QString();
		}

		case READER_SUPPORTED:
			return isSupportedReader(pIndex);

		case READER_INSTALLED:
			return isInstalledReader(pIndex);

		case SHOW_STATUS_ICON:
			return true;

		default:
			return QVariant();
	}
}


QHash<int, QByteArray> ReaderModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(READER_NAME, "readerName");
	roles.insert(READER_IMAGE_PATH, "readerImagePath");
	roles.insert(READER_HTML_DESCRIPTION, "readerHTMLDescription");
	roles.insert(READER_DRIVER_URL, "readerDriverUrl");
	roles.insert(READER_SUPPORTED, "readerSupported");
	roles.insert(READER_INSTALLED, "readerInstalled");
	roles.insert(SHOW_STATUS_ICON, "showStatusIcon");
	return roles;
}


void ReaderModel::onTranslationChanged()
{
	onUpdateContent();
}


bool ReaderModel::hasConnectedReader() const
{
	return !mConnectedReaders.isEmpty();
}
