/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 *
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QPair>
#include <QString>

namespace governikus
{
class SurveyModel
	: public QAbstractListModel
{
	Q_OBJECT

	private:
		const QString mBuildNumber;
		const QString mAndroidVersion;
		const QString mKernelVersion;
		int mMaximumNfcPacketLength;
		const QString mVendor;
		const QString mModelNumber;
		const QString mModelName;
		const QString mAusweisAppVersionNumber;

		QVector<QPair<QString, QString> > mData;

		enum UserRoles
		{
			TITLE = Qt::UserRole + 1,
			VALUE
		};


		void buildDataObject();
		QByteArray toJsonByteArray() const;

	public:
		explicit SurveyModel(QObject* pParent = nullptr);

		int rowCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		static SurveyModel& getInstance();

		void setMaximumNfcPacketLength(int pMaximumNfcPacketLength);

		void transmitSurvey() const;
};

} // namespace governikus
