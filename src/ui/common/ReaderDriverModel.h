/*!
 * \brief Model implementation for the reader driver table
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderDetector.h"

#include <QAbstractTableModel>
#include <QHash>
#include <QSet>
#include <QTime>
#include <QVector>


namespace governikus
{

class ReaderDriverModel
	: public QAbstractTableModel
{
	Q_OBJECT

	Q_PROPERTY(QString noReaderFoundIconPath READ getNoReaderFoundIconPath CONSTANT)
	Q_PROPERTY(QString emptyListDescriptionString READ getEmptyListDescriptionStringQml NOTIFY fireLanguageChanged)
	Q_PROPERTY(QString lastUpdatedInformation READ getLastUpdatedInformation NOTIFY fireModelChanged)

	private:
		const int NUMBER_OF_COLUMNS = 2;

		QSet<const ReaderConfigurationInfo> mKnownDrivers;
		QVector<ReaderConfigurationInfo> mConnectedReaders;
		QTime mConnectedReadersUpdateTime;

		QString getStatus(const ReaderConfigurationInfo& pReaderConfigurationInfo) const;
		void collectReaderData();

		bool indexIsValid(const QModelIndex& pIndex) const;

		QUrl getReaderImageUrl(const QModelIndex& pIndex) const;

	public:
		enum ColumnId : int
		{
			ReaderName = 0,
			ReaderStatus = 1
		};
		enum UserRoles
		{
			READER_NAME = Qt::UserRole + 1,
			READER_STATUS,
			READER_IMAGE_PATH,
			READER_HTML_DESCRIPTION,
			READER_DRIVER_URL,
			READER_INSTALLED_AND_SUPPORTED
		};
		ReaderDriverModel(QObject* pParent = nullptr);

		virtual QVariant headerData(int pSection, Qt::Orientation pOrientation, int pRole) const override;
		virtual int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex& pParent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		QString getReaderImagePath(const QModelIndex& pIndex) const;
		QString getNoReaderFoundIconPath() const;
		QString getHTMLDescription(const QModelIndex& pIndex) const;
		QString getEmptyListDescriptionStringQml() const;
		QString getEmptyListDescriptionString(bool pWidgetUiHelp = true) const;
		bool isInstalledSupportedReader(const QModelIndex& pIndex) const;
		QString getLastUpdatedInformation() const;

	public Q_SLOTS:
		void onUpdateContent();

	Q_SIGNALS:
		void fireModelChanged();
		void fireLanguageChanged();
};


} // namespace governikus
