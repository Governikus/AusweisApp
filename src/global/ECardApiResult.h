/*!
 * \brief Status representation according to TR-03112-6 "eCard-API-Framework"
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QMap>
#include <QPair>
#include <QSharedData>
#include <QString>

class test_ECardApiResult;
class test_UrlUtil;

namespace governikus
{

class ECardApiResult
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(governikus::ECardApiResult)

	friend class RemoteMessageResponse;
	friend class StartPaosResponse;
	friend class ::test_ECardApiResult;
	friend class ::test_UrlUtil;

	public:
		enum class Major
		{
			Unknown,
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
			IFDL_UnknownSlot,
			IFDL_InvalidSlotHandle,
			IFDL_CancellationByUser,
			IFDL_IFD_SharingViolation,
			IFDL_Terminal_NoCard,
			IFDL_IO_RepeatedDataMismatch,
			IFDL_IO_UnknownPINFormat,
			IL_Signature_InvalidCertificatePath,
			KEY_KeyGenerationNotPossible,
			SAL_Cancellation_by_User,
			SAL_Invalid_Key,
			SAL_SecurityConditionNotSatisfied,
			SAL_MEAC_AgeVerificationFailedWarning,
			SAL_MEAC_CommunityVerificationFailedWarning,
			SAL_MEAC_DocumentValidityVerificationFailed,
		};

		enum class Origin
		{
			Server, Client
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
				const Origin mOrigin;

				ResultData(Major pMajor, Minor pMinor, const QString& pMessage, Origin pOrigin);

				bool operator ==(const ResultData& pOther) const
				{
					return mMajor == pOther.mMajor &&
						   mMinor == pOther.mMinor &&
						   mMessage == pOther.mMessage &&
						   mMessageLang == pOther.mMessageLang &&
						   mOrigin == pOther.mOrigin;
				}


		};

		static const QMap<Major, QString> cMajorResults;
		static const QMap<Minor, QString> cMinorResults;

		static QMap<GlobalStatus::Code, Minor> cConversionMap1;
		static QMap<Minor, GlobalStatus::Code> cConversionMap2;

		static void initConversionMaps();
		static void addConversionElement(const GlobalStatus::Code pCode, const Minor pMinor);
		static GlobalStatus::Code toStatus(const Minor pMinor);
		static Minor fromStatus(const GlobalStatus::Code pCode);

		static GlobalStatus::Origin toStatus(const governikus::ECardApiResult::Origin pSelf);
		static ECardApiResult::Origin fromStatus(const GlobalStatus::Origin pSelf);

		static ECardApiResult fromStatus(const GlobalStatus& pStatus);

		static Major parseMajor(const QString& pMajor);
		static Minor parseMinor(const QString& pMinor);

		QSharedDataPointer<ResultData> d;

		ECardApiResult(Major pMajor, Minor pMinor, const QString& pMessage = QString(), Origin pOrigin = Origin::Client);
		ECardApiResult(const QString& pMajor, const QString& pMinor = QString(), const QString& pMessage = QString(), Origin pOrigin = Origin::Client);

	public:
		ECardApiResult(const GlobalStatus& pStatus);

		bool operator ==(const ECardApiResult& pResult) const;

		static ECardApiResult createOk();

		static bool isMajor(const QString& pMajor);
		static bool isMinor(const QString& pMinor);
		static QString getMessage(Minor pMinor);

		Major getMajor() const;
		Minor getMinor() const;
		QString getMessage() const;
		const QString& getMessageLang() const;

		static QString getMajorString(Major pMajor);
		static QString getMinorString(Minor pMinor);

		QString getMajorString() const;
		QString getMinorString() const;

		bool isValid() const;
		bool isOk() const;
		bool isOriginServer() const;
		QJsonObject toJson() const;

		GlobalStatus toStatus() const;
		explicit operator GlobalStatus() const;
};

} // namespace governikus

QDebug operator <<(QDebug pDbg, const governikus::ECardApiResult& pResult);
