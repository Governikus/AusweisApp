/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 *
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QNetworkReply>
#include <QObject>
#include <QPair>
#include <QString>

class test_SurveyModel;

namespace governikus
{

class SurveyModel
	: public QAbstractListModel
{
	Q_OBJECT
	friend class Env;
	friend class ::test_SurveyModel;

	private:
		const QString mBuildNumber;
		const QString mAndroidVersion;
		const QString mKernelVersion;
		int mMaximumNfcPacketLength;
		const QString mVendor;
		const QString mModelNumber;
		const QString mModelName;
		const QString mAusweisAppVersionNumber;

		QVector<QPair<QString, QString>> mData;
		QSharedPointer<QNetworkReply> mReply;

		enum UserRoles
		{
			TITLE = Qt::UserRole + 1,
			VALUE
		};

		SurveyModel();
		~SurveyModel() override = default;

		void buildDataObject();
		[[nodiscard]] QByteArray toJsonByteArray() const;

	private Q_SLOTS:
		void onSslErrors(const QList<QSslError>& pErrors);
		void onSslHandshakeDone();
		void onNetworkReplyFinished();

	public:
		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		void setMaximumNfcPacketLength(int pMaximumNfcPacketLength);

		void transmitSurvey();
};

} // namespace governikus
