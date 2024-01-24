/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Status class that tracks reasons for aborting a Workflow
 */

#pragma once

#include "EnumHelper.h"

#include <QDebug>
#include <QMap>
#include <QPair>

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
			Prepace_Pace_Smart_Eid_Invalidated,
			Establish_Pace_Channel_Basic_Reader_No_Pin,
			Establish_Pace_Channel_Puk_Inoperative,
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
			Change_Pin_Card_New_Pin_Mismatch,
			Change_Pin_Card_User_Cancelled,
			Start_Ifd_Service_Failed,
			Prepare_Pace_Ifd_Unknown,
			Establish_Pace_Ifd_Unknown,
			Enter_Pace_Password_Ifd_User_Cancelled,
			Enter_New_Pace_Pin_Ifd_User_Cancelled,
			Check_Status_Unavailable,
			Install_Smart_User_Cancelled,
			Delete_Smart_User_Cancelled,
			Delete_Personalization_User_Cancelled,
			Update_Support_Info_User_Cancelled,
			Install_Smart_Service_Response_Fail,
			Install_Smart_Service_Response_Unsupported,
			Install_Smart_Service_Response_Overload,
			Install_Smart_Service_Response_Maintenance,
			Install_Smart_Service_Response_Nfc_Disabled,
			Install_Smart_Service_Response_Integrity_Check_Failed,
			Install_Smart_Service_Response_Not_Authenticated,
			Install_Smart_Service_Response_Network_Connection_Error,
			Update_Support_Info_Call_Failed,
			Update_Support_Info_Service_Response_Fail,
			Update_Support_Info_Service_Response_Unsupported,
			Update_Support_Info_Service_Response_Overload,
			Update_Support_Info_Service_Response_Maintenance,
			Update_Support_Info_Service_Response_Nfc_Disabled,
			Update_Support_Info_Service_Response_Integrity_Check_Failed,
			Update_Support_Info_Service_Response_Not_Authenticated,
			Update_Support_Info_Service_Response_Network_Connection_Error,
			Delete_Smart_Service_Response_Fail,
			Delete_Smart_Service_Response_Unsupported,
			Delete_Smart_Service_Response_Overload,
			Delete_Smart_Service_Response_Maintenance,
			Delete_Smart_Service_Response_Nfc_Disabled,
			Delete_Smart_Service_Response_Integrity_Check_Failed,
			Delete_Smart_Service_Response_Not_Authenticated,
			Delete_Smart_Service_Response_Network_Connection_Error,
			Delete_Personalization_Failed,
			Check_Applet_Internal_Error,
			Get_Session_Id_Invalid,
			Smart_ServiceInformation_Query_Failed,
			Get_Challenge_Invalid,
			Initialize_Personalization_Failed,
			Smart_PrePersonalization_Wrong_Status,
			Smart_PrePersonalization_Incomplete_Information,
			Transmit_Personalization_Size_Mismatch,
			Start_Paos_Response_Personalization_Empty,
			Start_Paos_Response_Personalization_Invalid,
			Finalize_Personalization_Failed,
			Insert_Card_No_SmartReader,
			Insert_Card_Multiple_SmartReader,
			Insert_Card_Unknown_Eid_Type,
			Insert_Card_Invalid_SmartReader,
			Insert_Card_Missing_Card,
			Change_Smart_Pin_Failed
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
			Paos_Type
		};
		Q_ENUM(Info)

		using FailureInfoMap = QMap<Info, QString>;

		FailureCode(Reason pReason);
		FailureCode(Reason pReason, const FailureInfoMap& pInfoMap);
		FailureCode(Reason pReason, const QPair<Info, QString>& pInfo);

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
