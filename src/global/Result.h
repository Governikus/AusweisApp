/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QMap>
#include <QPair>
#include <QSharedData>
#include <QString>

class test_result;

namespace governikus
{

class Result
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(governikus::Result)

	friend class StartPaosResponse;
	friend class ::test_result;

	public:
		enum class Major
		{
			Unknown,
			Ok,
			Warning,
			Error
		};

		Q_ENUM(Major)

	private:
		class ResultData
			: public QSharedData
		{
			public:
				const Major mMajor;
				const GlobalStatus::Code mMinor;
				const QString mMessage;
				const QString mMessageLang;
				const Origin mOrigin;

				ResultData(Major pMajor, GlobalStatus::Code pMinor, const QString& pMessage, Origin pOrigin);

				bool operator ==(const ResultData& pOther) const
				{
					return mMajor == pOther.mMajor &&
						   mMinor == pOther.mMinor &&
						   mMessage == pOther.mMessage &&
						   mMessageLang == pOther.mMessageLang &&
						   mOrigin == pOther.mOrigin;
				}


		};

		static const QMap<Major, QString> mMajorResults;
		static const QMap<GlobalStatus::Code, QString> mMinorResults;

		QSharedDataPointer<ResultData> d;

		static Result fromStatus(const GlobalStatus& pStatus);

		Result(Major pMajor, GlobalStatus::Code pMinor, const QString& pMessage = QString(), Origin pOrigin = Origin::Client);
		Result(const QString& pMajor, const QString& pMinor = QString(), const QString& pMessage = QString(), Origin pOrigin = Origin::Client);

	public:
		Result(const GlobalStatus& pStatus);

		bool operator ==(const Result& pResult) const;

		static Result createOk();

		static Major parseMajor(const QString& pMajor);
		static GlobalStatus::Code parseMinor(const QString& pMinor);

		static bool isMajor(const QString& pMajor);
		static bool isMinor(const QString& pMinor);
		static QString getMessage(GlobalStatus::Code pMinor);

		Major getMajor() const;
		GlobalStatus::Code getMinor() const;
		QString getMessage() const;
		const QString& getMessageLang() const;

		QString getMajorString() const;
		QString getMinorString() const;

		bool isValid() const;
		bool isOk() const;
		bool isOriginServer() const;
		QJsonObject toJson() const;

		GlobalStatus toStatus() const;
		operator GlobalStatus() const;
};

}

QDebug operator <<(QDebug pDbg, const governikus::Result& pResult);
