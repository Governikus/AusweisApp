/*!
 * \brief A global mapping for errors
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QCoreApplication>
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
			Network_Ssl_Establishment_Error,
			Network_TimeOut,
			Network_Proxy_Error,
			Network_Other_Error,

			Downloader_File_Not_Found,
			Downloader_Cannot_Save_File,
			Downloader_Data_Corrupted,

			Workflow_AlreadyInProgress_Error,
			Workflow_Communication_Missing_Redirect_Url,
			Workflow_Cancellation_By_User,
			Workflow_Card_Removed,
			Workflow_Cannot_Confirm_IdCard_Authenticity,
			Workflow_Unknown_Paos_Form_EidServer,
			Workflow_Unexpected_Message_From_EidServer,
			Workflow_Pin_Blocked_And_Puk_Objectionable,
			Workflow_Preverification_Developermode_Error,
			Workflow_Preverification_Error,
			Workflow_No_Unique_AtCvc,
			Workflow_No_Unique_DvCvc,
			Workflow_No_Permission_Error,
			Workflow_Certificate_No_Description,
			Workflow_Certificate_No_Url_In_Description,
			Workflow_Certificate_Hash_Error,
			Workflow_Certificate_Sop_Error,
			Workflow_Error_Page_Transmission_Error,
			Workflow_Redirect_Transmission_Error,
			Workflow_Processing_Error,
			Workflow_TrustedChannel_Establishment_Error,
			Workflow_TrustedChannel_Error_From_Server,
			Workflow_TrustedChannel_Hash_Not_In_Description,
			Workflow_TrustedChannel_No_Data_Received,
			Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length,
			Workflow_TrustedChannel_ServiceUnavailable,
			Workflow_TrustedChannel_TimeOut,
			Workflow_TrustedChannel_Proxy_Error,
			Workflow_TrustedChannel_Ssl_Establishment_Error,
			Workflow_TrustedChannel_Server_Format_Error,
			Workflow_TrustedChannel_Other_Network_Error,
			Workflow_Reader_Became_Inaccessible,
			Workflow_Reader_Communication_Error,
			Workflow_Server_Incomplete_Information_Provided,
			Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length,
			Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length,
			Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description,
			Workflow_Network_Empty_Redirect_Url,
			Workflow_Network_Expected_Redirect,
			Workflow_Network_Invalid_Scheme,
			Workflow_Network_Malformed_Redirect_Url,
			Workflow_Wrong_Parameter_Invocation,

			Paos_Unexpected_Warning,

			Paos_Error_AL_Unknown_Error,
			Paos_Error_AL_No_Permission,
			Paos_Error_AL_Internal_Error,
			Paos_Error_AL_Parameter_Error,
			Paos_Error_AL_Unkown_API_Function,
			Paos_Error_AL_Not_Initialized,
			Paos_Error_AL_Warning_Connection_Disconnected,
			Paos_Error_AL_Session_Terminated_Warning,
			Paos_Error_AL_Communication_Error,
			Paos_Error_DP_Timeout_Error,
			Paos_Error_DP_Unknown_Channel_Handle,
			Paos_Error_DP_Communication_Error,
			Paos_Error_DP_Trusted_Channel_Establishment_Failed,
			Paos_Error_DP_Unknown_Protocol,
			Paos_Error_DP_Unknown_Cipher_Suite,
			Paos_Error_DP_Unknown_Webservice_Binding,
			Paos_Error_DP_Node_Not_Reachable,
			Paos_Error_IFDL_Timeout_Error,
			Paos_Error_IFDL_InvalidSlotHandle,
			Paos_Error_IFDL_CancellationByUser,
			Paos_Error_KEY_KeyGenerationNotPossible,
			Paos_Error_SAL_Cancellation_by_User,
			Paos_Error_SAL_CertificateChainInterrupted,
			Paos_Error_SAL_Invalid_Key,
			Paos_Error_SAL_SecurityConditionNotSatisfied,
			Paos_Error_SAL_MEAC_AgeVerificationFailedWarning,
			Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning,
			Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed,

			Workflow_Reader_Device_Connection_Error,
			Workflow_Reader_Device_Scan_Error,

			Card_Not_Found,
			Card_Communication_Error,
			Card_Protocol_Error,
			Card_Cancellation_By_User,
			Card_Input_TimeOut,
			Card_Invalid_Pin,
			Card_Invalid_Can,
			Card_Invalid_Puk,
			Card_Pin_Blocked,
			Card_Pin_Not_Blocked,
			Card_Puk_Blocked,
			Card_NewPin_Mismatch,
			Card_NewPin_Invalid_Length,

			RemoteReader_CloseCode_NormalClose,
			RemoteReader_CloseCode_AbnormalClose,
			RemoteReader_CloseCode_Undefined,

			RemoteConnector_InvalidRequest,
			RemoteConnector_EmptyPassword,
			RemoteConnector_NoSupportedApiLevel,
			RemoteConnector_ConnectionTimeout,
			RemoteConnector_ConnectionError,
			RemoteConnector_RemoteHostRefusedConnection
		};

		enum class Origin
		{
			Server, Client
		};

		Q_ENUM(Code)
		Q_ENUM(Origin)

	private:
		class InternalStatus
			: public QSharedData
		{
			public:
				const Code mStatusCode;
				const QStringList mExternalInformation;
				const Origin mOrigin;

				InternalStatus(Code pStatusCode, const QStringList& pExternalInformation, const Origin pOrigin)
					: mStatusCode(pStatusCode)
					, mExternalInformation(pExternalInformation)
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
		const QString getExternalInfo(int pIndex = 0) const;

		static QString maskMessage(const QString& pMessage, const bool pMask);

	public:
		GlobalStatus(Code pStatusCode = Code::Unknown_Error, const QStringList& pExternalInformation = QStringList(), const Origin pOrigin = Origin::Client)
			: d(new InternalStatus(pStatusCode, pExternalInformation, pOrigin))
		{

		}


		GlobalStatus(Code pStatusCode, const QString& pExternalInformation, const Origin pOrigin = Origin::Client)
			: GlobalStatus(pStatusCode, QStringList(pExternalInformation), pOrigin)
		{

		}


		bool operator ==(const GlobalStatus& pOther) const;
		bool is(const Code pStatusCode) const;

		Code getStatusCode() const;
		operator GlobalStatus::Code() const;

		QString toErrorDescription(const bool pSimplifiedVersion = false) const;

		Origin getOrigin() const;
		bool isOriginServer() const;

		bool isNoError() const;
		bool isError() const;
		bool isCancellationByUser() const;
};

using Origin = GlobalStatus::Origin;

defineEnumOperators(GlobalStatus::Code)

} /* namespace governikus */


QDebug operator <<(QDebug pDbg, const governikus::GlobalStatus& pStatus);
