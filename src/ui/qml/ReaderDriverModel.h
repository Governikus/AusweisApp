/*!
 * \brief Model implementation for the reader driver table
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderConfigurationInfo.h"
#include "SortedReaderDriverModel.h"

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
	Q_PROPERTY(QString emptyListDescriptionString READ getEmptyListDescriptionString NOTIFY fireLanguageChanged)
	Q_PROPERTY(QString lastUpdatedInformation READ getLastUpdatedInformation NOTIFY fireModelChanged)
	Q_PROPERTY(SortedReaderDriverModel * sortedModel READ getSortedModel CONSTANT)

	private:
		const int NUMBER_OF_COLUMNS = 2;

		QSet<const ReaderConfigurationInfo> mKnownDrivers;
		QVector<ReaderConfigurationInfo> mConnectedReaders;
		QTime mConnectedReadersUpdateTime;
		SortedReaderDriverModel mSortedModel;

		[[nodiscard]] QString getStatus(const ReaderConfigurationInfo& pReaderConfigurationInfo) const;
		void collectReaderData();

		[[nodiscard]] bool indexIsValid(const QModelIndex& pIndex) const;

		[[nodiscard]] QUrl getReaderImageUrl(const QModelIndex& pIndex) const;

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
			READER_INSTALLED,
			READER_SUPPORTED
		};
		explicit ReaderDriverModel(QObject* pParent = nullptr);

		[[nodiscard]] QVariant headerData(int pSection, Qt::Orientation pOrientation, int pRole) const override;
		[[nodiscard]] int rowCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] int columnCount(const QModelIndex& pParent = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] QString getReaderImagePath(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getNoReaderFoundIconPath() const;
		[[nodiscard]] QString getHTMLDescription(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getEmptyListDescriptionString() const;
		[[nodiscard]] bool isSupportedReader(const QModelIndex& pIndex) const;
		[[nodiscard]] bool isInstalledReader(const QModelIndex& pIndex) const;
		[[nodiscard]] QString getLastUpdatedInformation() const;

		SortedReaderDriverModel* getSortedModel();

	public Q_SLOTS:
		void onUpdateContent();

	Q_SIGNALS:
		void fireModelChanged();
		void fireLanguageChanged();
};


} // namespace governikus
