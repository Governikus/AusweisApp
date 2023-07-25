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
			Processing_Send_Status_Failed,
			Parse_TcToken_Invalid_Url,
			Parse_TcToken_Missing_Url,
			Get_TcToken_Invalid_Url,
			Get_TcToken_Invalid_Redirect_Url,
			Get_TcToken_Invalid_Certificate_Key_Length,
			Get_TcToken_Invalid_Ephemeral_Key_Length,
			Get_TcToken_Invalid_Server_Reply,
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
			Establish_Pace_Channel_No_Active_Pin,
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
			Redirect_Browser_Send_Error_Page_Failed,
			Redirect_Browser_Send_Redirect_Failed,
			Generic_Provider_Communication_Network_Error,
			Generic_Provider_Communication_Invalid_Ephemeral_Key_Length,
			Generic_Provider_Communication_Certificate_Error,
			Generic_Provider_Communication_Tls_Error,
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
			Transmit_Personalization_Size_Mismatch,
			Start_Paos_Response_Personalization_Empty,
			Start_Paos_Response_Personalization_Invalid,
			Prepare_Applet_User_Cancelled,
			Prepare_Applet_Status_Call_Failed,
			Prepare_Applet_Installation_Loop,
			Prepare_Applet_Installation_Failed,
			Prepare_Applet_Unavailable,
			Prepare_Applet_Delete_Personalization_Failed,
			Prepare_Applet_UpdateInfo_Call_Failed,
			Prepare_Applet_Delete_Smart_Failed,
			Insert_Card_No_SmartReader,
			Insert_Card_Multiple_SmartReader,
			Insert_Card_Unknown_Eid_Type,
			Insert_Card_HW_Keystore,
			Insert_Card_Invalid_SmartReader,
			Insert_Card_Missing_Card,
			Initialize_Personalization_Failed,
			Get_Session_Id_Invalid,
			Get_Challenge_Invalid,
			Finalize_Personalization_Failed,
			Change_Smart_Pin_Failed,
			Check_Status_Unavailable,
			Check_Applet_Error,
			Check_Applet_Unavailable
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
