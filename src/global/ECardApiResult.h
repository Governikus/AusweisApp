/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Status representation according to TR-03112-6 "eCard-API-Framework"
 */

#pragma once

#include "FailureCode.h"
#include "GlobalStatus.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QMap>
#include <QPair>
#include <QSharedData>
#include <QString>
#include <optional>


class test_ECardApiResult;
class test_UrlUtil;


namespace governikus
{

class ECardApiResult
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(governikus::ECardApiResult)

	friend class IfdMessageResponse;
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
			AL_Unknown_API_Function,
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
				const std::optional<FailureCode> mFailureCode;

				ResultData(Major pMajor, Minor pMinor, const QString& pMessage, Origin pOrigin, const std::optional<FailureCode>& pFailureCode = {});

				bool operator ==(const ResultData& pOther) const
				{
					return mMajor == pOther.mMajor &&
						   mMinor == pOther.mMinor &&
						   mMessage == pOther.mMessage &&
						   mMessageLang == pOther.mMessageLang &&
						   mOrigin == pOther.mOrigin &&
						   mFailureCode == pOther.mFailureCode;
				}


		};

		static const QMap<Major, QString> cMajorResults;
		static const QMap<Minor, QString> cMinorResults;

		static QMap<GlobalStatus::Code, Minor> cConversionMap1;
		static QMap<Minor, GlobalStatus::Code> cConversionMap2;

		static void initConversionMaps();
		static void addConversionElement(GlobalStatus::Code pCode, Minor pMinor);
		static GlobalStatus::Code toStatus(Minor pMinor);
		static Minor fromStatus(GlobalStatus::Code pCode);

		static GlobalStatus::Origin toStatus(governikus::ECardApiResult::Origin pSelf);
		static ECardApiResult::Origin fromStatus(GlobalStatus::Origin pSelf);

		static ECardApiResult fromStatus(const GlobalStatus& pStatus, const std::optional<FailureCode>& pFailureCode);

		static Major parseMajor(const QString& pMajor);
		static Minor parseMinor(const QString& pMinor);

		QSharedDataPointer<ResultData> d;

		ECardApiResult(Major pMajor, Minor pMinor, const QString& pMessage = QString(), Origin pOrigin = Origin::Client, const std::optional<FailureCode>& pFailureCode = {});
		explicit ECardApiResult(const QString& pMajor, const QString& pMinor = QString(), const QString& pMessage = QString(), Origin pOrigin = Origin::Client);

	public:
		explicit ECardApiResult(const GlobalStatus& pStatus, const std::optional<FailureCode>& pFailureCode = {});

		bool operator ==(const ECardApiResult& pResult) const;

		[[nodiscard]] static ECardApiResult createOk();

		static bool isMajor(const QString& pMajor);
		static bool isMinor(const QString& pMinor);
		static QString getMessage(Minor pMinor);

		[[nodiscard]] Major getMajor() const;
		[[nodiscard]] Minor getMinor() const;
		[[nodiscard]] QString getMessage() const;
		[[nodiscard]] const QString& getMessageLang() const;
		[[nodiscard]] const std::optional<FailureCode>& getFailureCode() const;

		[[nodiscard]] static QString getMajorString(Major pMajor);
		[[nodiscard]] static QString getMinorString(Minor pMinor);

		[[nodiscard]] QString getMajorString() const;
		[[nodiscard]] QString getMinorString() const;

		[[nodiscard]] bool isValid() const;
		[[nodiscard]] bool isOk() const;
		[[nodiscard]] bool isOriginServer() const;
		[[nodiscard]] QJsonObject toJson() const;

		[[nodiscard]] GlobalStatus toStatus() const;
		explicit operator GlobalStatus() const;
};

} // namespace governikus

QDebug operator <<(QDebug pDbg, const governikus::ECardApiResult& pResult);
