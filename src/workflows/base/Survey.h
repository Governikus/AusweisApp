/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class holding information about an Android device to be sent to
 * the whitelist server.
 */

#pragma once

#include <QNetworkReply>
#include <QPair>
#include <QString>

#include "ReaderInfo.h"

class test_Survey;

namespace governikus
{

class Survey
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_Survey;

	public:
		using SurveyData = QList<QPair<QString, QString>>;

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

		SurveyData mData;
		QSharedPointer<QNetworkReply> mReply;

		enum UserRoles
		{
			TITLE = Qt::UserRole + 1,
			VALUE
		};

		Survey();
		~Survey() override = default;

		void buildDataObject();
		[[nodiscard]] QByteArray toJsonByteArray() const;
		void resetNfcData();

	private Q_SLOTS:
		void onNetworkReplyFinished();

	public:
		[[nodiscard]] SurveyData getSurveyData() const;

		void setReaderInfo(const ReaderInfo& pReaderInfo);
		void setAuthWasSuccessful(bool pSuccess);

		[[nodiscard]] bool askForDeviceSurvey() const;
		[[nodiscard]] bool isDeviceSurveyPending() const;
		void setDeviceSurveyPending(bool pValue) const;

		void transmitSurvey();

	Q_SIGNALS:
		void fireSurveyDataChanged();
};

} // namespace governikus
