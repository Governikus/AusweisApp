/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QCoreApplication>
#include <QMap>
#include <QPair>
#include <QSharedData>


namespace governikus
{

class GlobalStatus
{
	Q_GADGET
	Q_DECLARE_TR_FUNCTIONS(governikus::GlobalStatus)

	public:
		enum class Code
		{
			Unknown_Error,
			No_Error,

			Network_ServiceUnavailable,
			Network_ServerError,
			Network_ClientError,
			Network_Ssl_Establishment_Error,
			Network_TimeOut,
			Network_Proxy_Error,
			Network_Other_Error,

			Downloader_File_Not_Found,
			Downloader_Cannot_Save_File,
			Downloader_Data_Corrupted,
			Downloader_Missing_Platform,
			Downloader_Aborted,

			Update_Execution_Failed,

			Workflow_AlreadyInProgress_Error,
			Workflow_Communication_Missing_Redirect_Url,
			Workflow_InternalError_BeforeTcToken,
			Workflow_Cancellation_By_User,
			Workflow_Card_Removed,
			Workflow_Cannot_Confirm_IdCard_Authenticity,
			Workflow_Unknown_Paos_From_EidServer,
			Workflow_Start_Paos_Response_Missing,
			Workflow_Unexpected_Message_From_EidServer,
			Workflow_Preverification_Developermode_Error,
			Workflow_Preverification_Error,
			Workflow_No_Unique_AtCvc,
			Workflow_No_Unique_DvCvc,
			Workflow_No_Permission_Error,
			Workflow_Wrong_Length_Error,
			Workflow_Certificate_No_Description,
			Workflow_Certificate_No_Url_In_Description,
			Workflow_Certificate_Hash_Error,
			Workflow_Certificate_Sop_Error,
			Workflow_Browser_Transmission_Error,
			Workflow_TrustedChannel_Establishment_Error,
			Workflow_TrustedChannel_Server_Error,
			Workflow_TrustedChannel_Client_Error,
			Workflow_TrustedChannel_Hash_Not_In_Description,
			Workflow_TrustedChannel_No_Data_Received,
			Workflow_TrustedChannel_Ssl_Connection_Unsupported_Algorithm_Or_Length,
			Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length,
			Workflow_TrustedChannel_ServiceUnavailable,
			Workflow_TrustedChannel_TimeOut,
			Workflow_TrustedChannel_Proxy_Error,
			Workflow_TrustedChannel_Server_Format_Error,
			Workflow_TrustedChannel_Other_Network_Error,
			Workflow_Reader_Became_Inaccessible,
			Workflow_Server_Incomplete_Information_Provided,
			Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length,
			Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length,
			Workflow_Network_Ssl_Hash_Not_In_Certificate_Description,
			Workflow_Network_Empty_Redirect_Url,
			Workflow_Network_Expected_Redirect,
			Workflow_Network_Invalid_Scheme,
			Workflow_Network_Malformed_Redirect_Url,
			Workflow_Wrong_Parameter_Invocation,
			Workflow_Smart_eID_Unavailable,
			Workflow_Smart_eID_Applet_Preparation_Failed,
			Workflow_Smart_eID_Authentication_Failed,
			Workflow_Smart_eID_ServiceInformation_Query_Failed,
			Workflow_Smart_eID_PrePersonalization_Failed,
			Workflow_Smart_eID_Personalization_Failed,
			Workflow_Smart_eID_Personalization_Denied,

			Paos_Unexpected_Warning,

			Paos_Generic_Server_Error,

			Paos_Error_AL_Unknown_Error,
			Paos_Error_AL_Internal_Error,
			Paos_Error_AL_Communication_Error,
			Paos_Error_DP_Trusted_Channel_Establishment_Failed,
			Paos_Error_SAL_Cancellation_by_User,
			Paos_Error_SAL_Invalid_Key,

			Card_Not_Found,
			Card_Communication_Error,
			Card_Protocol_Error,
			Card_Unexpected_Transmit_Status,
			Card_Cancellation_By_User,
			Card_Input_TimeOut,
			Card_Pin_Deactivated,
			Card_Invalid_Pin,
			Card_Invalid_Can,
			Card_Invalid_Puk,
			Card_Pin_Blocked,
			Card_Pin_Not_Blocked,
			Card_Puk_Blocked,
			Card_NewPin_Mismatch,
			Card_NewPin_Invalid_Length,
			Card_ValidityVerificationFailed,
			Card_Smart_Invalid,

			RemoteReader_CloseCode_AbnormalClose,

			IfdConnector_InvalidRequest,
			IfdConnector_NoSupportedApiLevel,
			IfdConnector_ConnectionTimeout,
			IfdConnector_ConnectionError,
			IfdConnector_RemoteHostRefusedConnection
		};

		enum class Origin
		{
			Server, Client
		};

		enum class ExternalInformation
		{
			ECARDAPI_ERROR,
			ECARDAPI_SERVERMESSAGE,
			LAST_URL,
			HTTP_STATUS_CODE,
			REDIRECT_URL,
			CERTIFICATE_ISSUER_NAME,
			URL_SCHEME,
			ACTIVATION_ERROR,
			PERSONALIZATION_RESTRICTION_DATE
		};

		using ExternalInfoMap = QMap<ExternalInformation, QString>;

		Q_ENUM(Code)
		Q_ENUM(Origin)
		Q_ENUM(ExternalInformation)

	private:
		class InternalStatus
			: public QSharedData
		{
			public:
				const Code mStatusCode;
				const ExternalInfoMap mExternalInformation;
				const Origin mOrigin;

				InternalStatus(Code pStatusCode, const ExternalInfoMap& pExternalInformation, const Origin pOrigin)
					: mStatusCode(pStatusCode)
					, mExternalInformation(pExternalInformation)
					, mOrigin(pOrigin)
				{
				}


				InternalStatus(Code pStatusCode, const QPair<ExternalInformation, QString>& pExternalInformation, const Origin pOrigin)
					: mStatusCode(pStatusCode)
					, mExternalInformation({
								{pExternalInformation.first, pExternalInformation.second}
							})
					, mOrigin(pOrigin)
				{
				}


				bool operator ==(const InternalStatus& pOther) const
				{
					return mStatusCode == pOther.mStatusCode &&
						   mExternalInformation == pOther.mExternalInformation &&
						   mOrigin == pOther.mOrigin;
				}


		};

		QSharedDataPointer<InternalStatus> d;
		[[nodiscard]] QString getExternalInfo(ExternalInformation pType) const;

		[[nodiscard]] QString toErrorDescriptionInternal() const;

	public:
		GlobalStatus(Code pStatusCode, const ExternalInfoMap& pExternalInformation, const Origin pOrigin = Origin::Client)
			: d(new InternalStatus(pStatusCode, pExternalInformation, pOrigin))
		{
		}


		GlobalStatus(Code pStatusCode, const QPair<ExternalInformation, QString>& pExternalInformation, const Origin pOrigin = Origin::Client)
			: d(new InternalStatus(pStatusCode, pExternalInformation, pOrigin))
		{
		}


		GlobalStatus(Code pStatusCode = Code::Unknown_Error, const Origin pOrigin = Origin::Client)
			: d(new InternalStatus(pStatusCode, ExternalInfoMap(), pOrigin))
		{
		}


		bool operator ==(const GlobalStatus& pOther) const;
		[[nodiscard]] bool is(const Code pStatusCode) const;

		[[nodiscard]] Code getStatusCode() const;

		[[nodiscard]] QString toErrorDescription(const bool pSimplifiedVersion = false) const;
		[[nodiscard]] QString getExternalInfo(const QString& pToken = QStringLiteral("; ")) const;

		[[nodiscard]] Origin getOrigin() const;
		[[nodiscard]] bool isOriginServer() const;

		[[nodiscard]] bool isNoError() const;
		[[nodiscard]] bool isError() const;
		[[nodiscard]] bool isCancellationByUser() const;
		[[nodiscard]] bool isMessageMasked() const;
};

using Origin = GlobalStatus::Origin;

defineEnumOperators(GlobalStatus::Code)

} // namespace governikus


QDebug operator <<(QDebug pDbg, const governikus::GlobalStatus& pStatus);
