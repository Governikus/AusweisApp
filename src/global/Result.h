/*!
 * Result.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ReturnCode.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QMap>
#include <QPair>
#include <QSharedData>
#include <QString>

namespace governikus
{

class Result
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(governikus::Result)

	friend class StartPaosResponse;

	public:
		enum class Major
		{
			null, // Used as undefined state only!
			Ok,
			Warning,
			Error
		};

		enum class Minor
		{
			null, // Used by Major::Ok and undefined state only!
			AL_Unknown_Error,
			AL_No_Permission,
			AL_Internal_Error,
			AL_Parameter_Error,
			AL_Unkown_API_Function,
			AL_Not_Initialized,
			AL_Warning_Connection_Disconnected,
			AL_Session_Terminated_Warning,
			AL_Communication_Error,
			DP_Timeout_Error,
			DP_Unknown_Channel_Handle,
			DP_Communication_Error,
			DP_Trusted_Channel_Establishment_Failed,
			DP_Unknown_Protocol,
			DP_Unknown_Cipher_Suite,
			DP_Unknown_Webservice_Binding,
			DP_Node_Not_Reachable,
			IFDL_Timeout_Error,
			IFDL_InvalidSlotHandle,
			IFDL_CancellationByUser,
			KEY_KeyGenerationNotPossible,
			SAL_Cancellation_by_User,
			SAL_CertificateChainInterrupted,
			SAL_Invalid_Key,
			SAL_SecurityConditionNotSatisfied,
			SAL_DocumentValidityVerificationFailed,
			SAL_MEAC_AgeVerificationFailedWarning,
			SAL_MEAC_CommunityVerificationFailedWarning,
			SAL_MEAC_DocumentValidityVerificationFailed,
		};

		enum class Origin
		{
			SERVER, CLIENT
		};

		Q_ENUM(Major)
		Q_ENUM(Minor)
		Q_ENUM(Origin)

	private:
		class ResultData
			: public QSharedData
		{
			public:
				const Major mMajor;
				const Minor mMinor;
				const QString mMessage;
				const QString mMessageLang;
				const ReturnCode mReturnCode;
				const Origin mOrigin;

				ResultData(Major pMajor, Minor pMinor, const QString& pMessage, const QString& pLang, ReturnCode pReturnCode, Origin pOrigin);
		};

		static const QMap<Major, QString> mMajorResults;
		static const QMap<Minor, QString> mMinorResults;
		static const QMap<Minor, const char*> mMinorDescriptions;

		QSharedDataPointer<ResultData> d;

		Result(Major pMajor, Minor pMinor, const QString& pMessage = QString(), const QString& pLang = QString(), ReturnCode pReturnCode = ReturnCode::UNDEFINED, Origin pOrigin = Origin::CLIENT);
		Result(const QString& pMajor, const QString& pMinor = QString(), const QString& pMessage = QString(), const QString& pLang = QString(), ReturnCode pReturnCode = ReturnCode::UNDEFINED, Origin pOrigin = Origin::CLIENT);

	public:
		static Result createOk();
		static Result createError(ReturnCode pReturnCode);
		static Result createCertChainInterruptedError(const QString& pMessage, const QString& pLang = QString());
		static Result createNoPermissionError(const QString& pMessage, const QString& pLang = QString());
		static Result createInvalidKeyError(const QString& pMessage, const QString& pLang = QString());
		static Result createInternalError(const QString& pMessage = QString(), const QString& pLang = QString());
		static Result createParameterError(const QString& pMessage = QString(), const QString& pLang = QString());
		static Result createCommunicationError(const QString& pMessage = QString(), const QString& pLang = QString());
		static Result createCardRemovedError(const QString& pMessage, const QString& pLang = QString());
		static Result createTrustedChannelEstablishmentError(const QString& pMessage, const QString& pLang = QString());
		static Result createCancelByUserError();

		static Major parseMajor(const QString& pMajor);
		static Minor parseMinor(const QString& pMinor);

		static bool isMajor(const QString& pMajor);
		static bool isMinor(const QString& pMinor);

		Major getMajor() const;
		Minor getMinor() const;
		const QString& getMessage() const;
		const QString& getMessageLang() const;
		ReturnCode getReturnCode() const;

		QString getMajorString() const;
		QString getMinorString() const;
		QString getMinorDescription() const;
		QString toString() const;

		bool isValid() const;
		bool isOk() const;
		bool isOriginServer() const;
		QJsonObject toJson() const;
};

}

QDebug operator <<(QDebug pDbg, const governikus::Result& pResult);
