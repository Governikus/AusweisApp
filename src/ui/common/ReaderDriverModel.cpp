/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDriverModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "GeneralSettings.h"
#include "HelpAction.h"
#include "LanguageLoader.h"
#include "ReaderConfiguration.h"
#include "ReaderDetector.h"
#include "ReaderManager.h"

using namespace governikus;


ReaderDriverModel::ReaderDriverModel(QObject* pParent)
	: QAbstractTableModel(pParent)
	, mKnownDrivers()
	, mConnectedReaders()
	, mConnectedReadersUpdateTime()
{
	const GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &ReaderDriverModel::fireLanguageChanged);
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &ReaderDriverModel::onUpdateContent);

	const ReaderManager* const readerManager = Env::getSingleton<ReaderManager>();

	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ReaderDriverModel::onUpdateContent);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ReaderDriverModel::onUpdateContent);
	connect(Env::getSingleton<ReaderConfiguration>(), &ReaderConfiguration::fireUpdated, this, &ReaderDriverModel::onUpdateContent);
	connect(Env::getSingleton<ReaderDetector>(), &ReaderDetector::fireReaderChangeDetected, this, &ReaderDriverModel::onUpdateContent);
	connect(Env::getSingleton<AppSettings>(), &AppSettings::fireSettingsChanged, this, &ReaderDriverModel::onUpdateContent);
	onUpdateContent();
}


void ReaderDriverModel::collectReaderData()
{
	mConnectedReaders.clear();

	const QVector<ReaderInfo> installedReaders = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({
				ReaderManagerPlugInType::PCSC
				, ReaderManagerPlugInType::BLUETOOTH
				, ReaderManagerPlugInType::NFC
			}));

	for (const auto& installedReader : installedReaders)
	{
		const auto& readerSettingsInfo = installedReader.getReaderConfigurationInfo();
		if (!readerSettingsInfo.getUrl().isEmpty())
		{
			mKnownDrivers += readerSettingsInfo;
			mConnectedReaders += readerSettingsInfo;
		}
	}

	QVector<ReaderConfigurationInfo> readersWithoutDriver;
	const auto& attachedSupportedDevices = Env::getSingleton<ReaderDetector>()->getAttachedSupportedDevices();
	for (const auto& info : attachedSupportedDevices)
	{
		if (!mConnectedReaders.contains(info))
		{
			readersWithoutDriver.append(info);
		}
	}
	mConnectedReaders += readersWithoutDriver;
}


bool ReaderDriverModel::indexIsValid(const QModelIndex& pIndex) const
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


QString ReaderDriverModel::getStatus(const ReaderConfigurationInfo& pReaderConfigurationInfo) const
{
	if (mConnectedReaders.isEmpty())
	{
		//: LABEL ALL_PLATFORMS
		return tr("Not connected");
	}

	if (mKnownDrivers.contains(pReaderConfigurationInfo))
	{
		//: LABEL ALL_PLATFORMS
		return tr("Driver installed");
	}

	//: LABEL ALL_PLATFORMS
	return tr("No driver installed");
}


void ReaderDriverModel::onUpdateContent()
{
	beginResetModel();

	collectReaderData();
	mConnectedReadersUpdateTime = QTime::currentTime();

	endResetModel();

	Q_EMIT fireModelChanged();
}


QVariant ReaderDriverModel::headerData(int pSection, Qt::Orientation pOrientation, int pRole) const
{
	if (pRole == Qt::DisplayRole && pOrientation == Qt::Horizontal)
	{
		switch (pSection)
		{
			case ColumnId::ReaderName:
				//: LABEL ALL_PLATFORMS
				return tr("Card reader");

			case ColumnId::ReaderStatus:
				//: LABEL ALL_PLATFORMS
				return tr("Status");

			default:
				return QVariant();
		}
	}
	return QVariant();
}


int ReaderDriverModel::rowCount(const QModelIndex&) const
{
	return mConnectedReaders.size();
}


int ReaderDriverModel::columnCount(const QModelIndex&) const
{
	return NUMBER_OF_COLUMNS;
}


QVariant ReaderDriverModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!indexIsValid(pIndex))
	{
		return QVariant();
	}

	const auto& reader = mConnectedReaders.at(pIndex.row());
	switch (pRole)
	{
		case Qt::DisplayRole:
			switch (pIndex.column())
			{
				case ColumnId::ReaderName:
					return reader.getName();

				case ColumnId::ReaderStatus:
					return getStatus(reader);

				default:
					return QVariant();
			}

		case READER_NAME:
			return reader.getName();

		case READER_STATUS:
			return getStatus(reader);

		case READER_IMAGE_PATH:
			return getReaderImageUrl(pIndex);

		case READER_HTML_DESCRIPTION:
			return getHTMLDescription(pIndex);

		case READER_DRIVER_URL:
			return mConnectedReaders.at(pIndex.row()).getUrl();

		case READER_INSTALLED_AND_SUPPORTED:
			return isInstalledSupportedReader(pIndex);

		default:
			return QVariant();
	}
}


QHash<int, QByteArray> ReaderDriverModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
	roles.insert(READER_NAME, "readerName");
	roles.insert(READER_STATUS, "readerStatus");
	roles.insert(READER_IMAGE_PATH, "readerImagePath");
	roles.insert(READER_HTML_DESCRIPTION, "readerHTMLDescription");
	roles.insert(READER_DRIVER_URL, "readerDriverUrl");
	roles.insert(READER_INSTALLED_AND_SUPPORTED, "readerInstalledAndSupported");
	return roles;
}


QString ReaderDriverModel::getNoReaderFoundIconPath() const
{
	return ReaderConfiguration::getNoReaderFoundIconPath();
}


QUrl ReaderDriverModel::getReaderImageUrl(const QModelIndex& pIndex) const
{
	return mConnectedReaders.at(pIndex.row()).getIcon()->lookupUrl();
}


QString ReaderDriverModel::getReaderImagePath(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return QString();
	}

	return mConnectedReaders.at(pIndex.row()).getIcon()->lookupPath();
}


QString ReaderDriverModel::getHTMLDescription(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return QString();
	}

	if (mConnectedReaders.isEmpty())
	{
		return QString();
	}

	if (mKnownDrivers.contains(mConnectedReaders.at(pIndex.row())))
	{
		//: LABEL ALL_PLATFORMS
		return tr("Card reader ready for use.");
	}

	//: INFO ALL_PLATFORMS The driver for card reader needs to be installed, the download link is provided in the message.
	return tr("Please download and install the driver you can find at: %1").
		   arg(QStringLiteral("<a href=\"%1\">%1</a>").arg(mConnectedReaders.at(pIndex.row()).getUrl()));
}


QString ReaderDriverModel::getEmptyListDescriptionStringQml() const
{
	return getEmptyListDescriptionString(false);
}


QString ReaderDriverModel::getEmptyListDescriptionString(bool pWidgetUiHelp) const
{
	const QString& onlineHelpSection = pWidgetUiHelp ? QStringLiteral("readerDeviceTab") : QStringLiteral("settingsPcscReader");
	const QString& url = HelpAction::getOnlineUrl(onlineHelpSection, pWidgetUiHelp);
	//: Is embedded in a sentence.
	const QString& hyperlink = QStringLiteral("<a href=\"%1\">%2</a>").arg(url, tr("online help"));
	//: INFO ALL_PLATFORMS No card reader was found, the message contains a link to the installation section of the manual.
	return tr("No connected card reader found. See %1 for installation of card readers.").arg(hyperlink);
}


bool ReaderDriverModel::isInstalledSupportedReader(const QModelIndex& pIndex) const
{
	if (!indexIsValid(pIndex))
	{
		return false;
	}

	const auto& readerSettingsInfo = mConnectedReaders.at(pIndex.row());
	const bool knownDriver = mKnownDrivers.contains(readerSettingsInfo);
	const bool knownReader = readerSettingsInfo.isKnownReader();
	return knownDriver && knownReader;
}


QString ReaderDriverModel::getLastUpdatedInformation() const
{
	if (!mConnectedReadersUpdateTime.isValid())
	{
		return QString();
	}

	const auto& updateTime = LanguageLoader::getInstance().getUsedLocale().toString(mConnectedReadersUpdateTime, tr("hh:mm:ss AP"));
	//: LABEL ALL_PLATFORMS
	return tr("The list of card readers was last updated at %1.").arg(updateTime);
}
