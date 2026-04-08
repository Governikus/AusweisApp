/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QDebug>
#include <QMap>

#include <utility>


namespace governikus
{

class FailureCode
{
	Q_GADGET

	public:
		enum class Reason
		{
			User_Cancelled,
			Card_Removed,
			Parse_TcToken_Invalid_Url,
			Parse_TcToken_Missing_Url,
			Get_TcToken_Invalid_Url,
			Get_TcToken_Invalid_Redirect_Url,
			Get_TcToken_Invalid_Certificate_Key_Length,
			Get_TcToken_Invalid_Ephemeral_Key_Length,
			Get_TcToken_Invalid_Server_Reply,
			Get_TcToken_ServiceUnavailable,
			Get_TcToken_Server_Error,
			Get_TcToken_Client_Error,
			Get_TcToken_Empty_Data,
			Get_TcToken_Invalid_Data,
			Get_TcToken_Network_Error,
			Certificate_Check_Failed_No_Description,
			Certificate_Check_Failed_No_SubjectUrl_In_Description,
			Certificate_Check_Failed_Hash_Mismatch,
			Certificate_Check_Failed_Same_Origin_Policy_Violation,
			Certificate_Check_Failed_Hash_Missing_In_Description,
			Pre_Verification_No_Test_Environment,
			Pre_Verification_Invalid_Certificate_Chain,
			Pre_Verification_Invalid_Certificate_Signature,
			Pre_Verification_Certificate_Expired,
			Extract_Cvcs_From_Eac1_No_Unique_At,
			Extract_Cvcs_From_Eac1_No_Unique_Dv,
			Extract_Cvcs_From_Eac1_At_Missing,
			Extract_Cvcs_From_Eac1_Dv_Missing,
			Connect_Card_Connection_Failed,
			Connect_Card_Eid_Inactive,
			Establish_Pace_Channel_Basic_Reader_No_Pin,
			Establish_Pace_Channel_Puk_Inoperative,
			Establish_Pace_Channel_Unknown_Password_Id,
			Establish_Pace_Channel_User_Cancelled,
			Maintain_Card_Connection_Pace_Unrecoverable,
			Did_Authenticate_Eac1_Card_Command_Failed,
			Process_Certificates_From_Eac2_Cvc_Chain_Missing,
			Did_Authenticate_Eac2_Invalid_Cvc_Chain,
			Did_Authenticate_Eac2_Card_Command_Failed,
			Generic_Send_Receive_Paos_Unhandled,
			Generic_Send_Receive_Network_Error,
			Generic_Send_Receive_Tls_Error,
			Generic_Send_Receive_Server_Error,
			Generic_Send_Receive_Service_Unavailable,
			Generic_Send_Receive_Client_Error,
			Generic_Send_Receive_Paos_Unknown,
			Generic_Send_Receive_Paos_Unexpected,
			Generic_Send_Receive_Invalid_Ephemeral_Key_Length,
			Generic_Send_Receive_Certificate_Error,
			Generic_Send_Receive_Session_Resumption_Failed,
			Transmit_Card_Command_Failed,
			Start_Paos_Response_Missing,
			Start_Paos_Response_Error,
			Check_Refresh_Address_Fatal_Tls_Error_Before_Reply,
			Check_Refresh_Address_Invalid_Ephemeral_Key_Length,
			Check_Refresh_Address_Service_Unavailable,
			Check_Refresh_Address_Server_Error,
			Check_Refresh_Address_Client_Error,
			Check_Refresh_Address_Service_Timeout,
			Check_Refresh_Address_Proxy_Error,
			Check_Refresh_Address_Fatal_Tls_Error_After_Reply,
			Check_Refresh_Address_Unknown_Network_Error,
			Check_Refresh_Address_Invalid_Http_Response,
			Check_Refresh_Address_Empty,
			Check_Refresh_Address_Invalid_Url,
			Check_Refresh_Address_No_Https_Scheme,
			Check_Refresh_Address_Fetch_Certificate_Error,
			Check_Refresh_Address_Unsupported_Certificate,
			Check_Refresh_Address_Hash_Missing_In_Certificate,
			Browser_Send_Failed,
			Generic_Provider_Communication_Network_Error,
			Generic_Provider_Communication_Invalid_Ephemeral_Key_Length,
			Generic_Provider_Communication_Certificate_Error,
			Generic_Provider_Communication_Tls_Error,
			Generic_Provider_Communication_ServiceUnavailable,
			Generic_Provider_Communication_Server_Error,
			Generic_Provider_Communication_Client_Error,
			Get_SelfAuthData_Invalid_Or_Empty,
			Change_Pin_No_SetEidPinCommand_Response,
			Change_Pin_Input_Timeout,
			Change_Pin_User_Cancelled,
			Change_Pin_New_Pin_Mismatch,
			Change_Pin_New_Pin_Invalid_Length,
			Change_Pin_Unexpected_Transmit_Status,
			Change_Pin_Card_User_Cancelled,
			Change_Pin_Unrecoverable,
			Start_Ifd_Service_Failed,
			Prepare_Pace_Ifd_Unknown,
			Establish_Pace_Ifd_Unknown,
			Enter_Pace_Password_Ifd_User_Cancelled,
			Enter_New_Pace_Pin_Ifd_User_Cancelled,
		};
		Q_ENUM(Reason)


		enum class Info
		{
			State_Name,
			Card_Return_Code,
			Network_Error,
			Http_Status_Code,
			Certificate_Status,
			Ssl_Errors,
			Paos_Type,
			Ephemeral_Server_Key_Algorithm,
			Ephemeral_Server_Key_Length
		};
		Q_ENUM(Info)

		using FailureInfoMap = QMap<Info, QString>;

		FailureCode(Reason pReason);
		FailureCode(Reason pReason, const FailureInfoMap& pInfoMap);
		FailureCode(Reason pReason, const std::pair<Info, QString>& pInfo);

		[[nodiscard]] Reason getReason() const;
		[[nodiscard]] bool operator==(const FailureCode& pFailure) const;
		[[nodiscard]] bool operator!=(const FailureCode& pFailure) const;
		[[nodiscard]] const FailureInfoMap& getFailureInfoMap() const;

		[[nodiscard]] QString toString() const;

	private:
		Reason mReason;
		FailureInfoMap mFailureInfoMap;
};

defineEnumOperators(FailureCode::Reason)

} // namespace governikus

QDebug operator <<(QDebug pDbg, const governikus::FailureCode& pFailure);
