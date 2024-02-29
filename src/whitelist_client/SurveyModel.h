/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 */

#pragma once

#include <QAbstractListModel>
#include <QNetworkReply>
#include <QObject>
#include <QPair>
#include <QString>

#include "ReaderInfo.h"

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
		const QString mOsVersion;
		const QString mKernelVersion;
		int mMaximumNfcPacketLength;
		const QString mVendor;
		const QString mModelNumber;
		const QString mModelName;
		const QString mAusweisAppVersionNumber;
		QString mNfcTagType;
		bool mNfcDataAvailable;
		bool mAuthWasSuccessful;

		QList<QPair<QString, QString>> mData;
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
		void resetNfcData();

	private Q_SLOTS:
		void onNetworkReplyFinished();

	public:
		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		void setReaderInfo(const ReaderInfo& pReaderInfo);
		void setAuthWasSuccessful(bool pSuccess);

		[[nodiscard]] Q_INVOKABLE bool askForDeviceSurvey() const;
		[[nodiscard]] bool isDeviceSurveyPending() const;
		Q_INVOKABLE void setDeviceSurveyPending(bool pValue) const;


		void transmitSurvey();
};

} // namespace governikus
