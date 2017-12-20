/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDriverModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "ReaderConfiguration.h"
#include "ReaderDetector.h"
#include "ReaderManager.h"


using namespace governikus;


ReaderDriverModel::ReaderDriverModel(QObject* pParent)
	: QAbstractTableModel(pParent)
	, mKnownDrivers()
	, mConnectedReaders()
{
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


QString ReaderDriverModel::getStatus(const ReaderConfigurationInfo& pReaderConfigurationInfo) const
{
	if (mConnectedReaders.isEmpty())
	{
		return tr("Not connected");
	}

	if (mKnownDrivers.contains(pReaderConfigurationInfo))
	{
		return tr("Connected w/ driver");
	}

	return tr("Connected w/o driver");
}


void ReaderDriverModel::onUpdateContent()
{
	beginResetModel();
	collectReaderData();
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
				return tr("Device");

			case ColumnId::ReaderStatus:
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
	if (pRole == Qt::DisplayRole)
	{
		const auto& reader = mConnectedReaders.at(pIndex.row());
		switch (pIndex.column())
		{
			case ColumnId::ReaderName:
				return reader.getName();

			case ColumnId::ReaderStatus:
				return getStatus(reader);
		}
	}

	return QVariant();
}


const ReaderConfigurationInfo& ReaderDriverModel::getReaderConfigurationInfo(const QModelIndex& pIndex) const
{
	return mConnectedReaders.at(pIndex.row());
}


QString ReaderDriverModel::getHTMLDescription(const QModelIndex& pIndex) const
{
	if (mConnectedReaders.isEmpty())
	{
		return QString();
	}

	if (mKnownDrivers.contains(mConnectedReaders.at(pIndex.row())))
	{
		return tr("Device is installed correctly.");
	}

	return tr("Device is not configured, please download and install the driver you can find at url: %1").
		   arg(QStringLiteral("<a href=\"%1\">%1</a>").arg(mConnectedReaders.at(pIndex.row()).getUrl()));
}


bool ReaderDriverModel::isInstalledSupportedReader(const QModelIndex& pIndex) const
{
	const auto& readerSettingsInfo = mConnectedReaders.at(pIndex.row());
	const bool knownDriver = mKnownDrivers.contains(readerSettingsInfo);
	const bool knownReader = readerSettingsInfo.isKnownReader();
	return knownDriver && knownReader;
}
