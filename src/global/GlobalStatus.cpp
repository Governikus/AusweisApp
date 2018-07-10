/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "GlobalStatus.h"

#include "Initializer.h"

#include <QDebug>

#define MESSAGE_SORRY "Sorry, that should not have happened! Please contact the support team."

using namespace governikus;

static Initializer::Entry E([] {
			qRegisterMetaType<GlobalStatus::Code>("GlobalStatus::Code");
			qRegisterMetaType<GlobalStatus>("GlobalStatus");
		});


const QString GlobalStatus::getExternalInfo(int pIndex) const
{
	return d->mExternalInformation.value(pIndex);
}


QString GlobalStatus::maskMessage(const QString& pMessage, const bool pMask)
{
	return pMask ? tr("An error occurred. Please contact our support at <a href=\"https://www.ausweisapp.bund.de/en/service/support/\">AusweisApp2 Support</a>.") : pMessage;
}


bool GlobalStatus::operator ==(const GlobalStatus& pOther) const
{
	return *d == *pOther.d;
}


bool GlobalStatus::is(const GlobalStatus::Code pStatusCode) const
{
	return d->mStatusCode == pStatusCode;
}


GlobalStatus::Code GlobalStatus::getStatusCode() const
{
	return d->mStatusCode;
}


GlobalStatus::operator GlobalStatus::Code() const
{
	return getStatusCode();
}


QString GlobalStatus::toErrorDescription(const bool pSimplifiedVersion) const
{
	switch (d->mStatusCode)
	{
		case Code::Workflow_Communication_Missing_Redirect_Url:
			return QString();

		case Code::No_Error:
			return tr("No error occurred.");

		case Code::Unknown_Error:
			return tr("An unexpected error has occurred during processing.");

		case Code::Workflow_AlreadyInProgress_Error:
			return tr("Cannot start authentication. An operation is already in progress.");

		case Code::Workflow_Card_Removed:
			return tr("The ID card has been removed. The process is aborted.");

		case Code::Workflow_Cannot_Confirm_IdCard_Authenticity:
			return tr("The authenticity of your ID card could not be confirmed.");

		case Code::Workflow_Unknown_Paos_Form_EidServer:
			return maskMessage(tr("The program received an unknown message from the server."), pSimplifiedVersion);

		case Code::Workflow_Unexpected_Message_From_EidServer:
			return maskMessage(tr("The program received an unexpected message from the server."), pSimplifiedVersion);

		case Code::Workflow_Pin_Blocked_And_Puk_Objectionable:
			return tr("After three wrong entries your PIN is blocked. Please use the PIN management in this app to unblock it with the help of your PUK.");

		case Code::Workflow_Preverification_Developermode_Error:
			return tr("Using the developer mode is only allowed in a test environment.");

		case Code::Workflow_Preverification_Error:
			return maskMessage(tr("Pre-verification failed."), pSimplifiedVersion);

		case Code::Workflow_No_Unique_AtCvc:
			return maskMessage(tr("No unique AT CVC"), pSimplifiedVersion);

		case Code::Workflow_No_Unique_DvCvc:
			return maskMessage(tr("No unique DV CVC"), pSimplifiedVersion);

		case Code::Workflow_No_Permission_Error:
			return tr("Authentication failed.");

		case Code::Workflow_Certificate_No_Description:
			return maskMessage(tr("No certificate description available."), pSimplifiedVersion);

		case Code::Workflow_Certificate_No_Url_In_Description:
			return maskMessage(tr("No subject url available in certificate description."), pSimplifiedVersion);

		case Code::Workflow_Certificate_Hash_Error:
			return maskMessage(tr("The certificate description does not match the certificate."), pSimplifiedVersion);

		case Code::Workflow_Certificate_Sop_Error:
			return maskMessage(tr("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy."), pSimplifiedVersion);

		case Code::Workflow_Error_Page_Transmission_Error:
		case Code::Workflow_Processing_Error:
		case Code::Workflow_Redirect_Transmission_Error:
			return maskMessage(getExternalInfo(), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_Establishment_Error:
			return maskMessage(tr("Failed to establish secure connection"), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_Error_From_Server:
			return maskMessage(tr("The program received an error from the server."), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_Hash_Not_In_Description:
		case Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description:
			return maskMessage(tr("Hash of certificate not in certificate description"), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_No_Data_Received:
			return maskMessage(tr("Received no data."), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_ServiceUnavailable:
		case Code::Network_ServiceUnavailable:
			return tr("The service is temporarily not available. Please try again later.");

		case Code::Network_TimeOut:
		case Code::Workflow_TrustedChannel_TimeOut:
			return maskMessage(tr("Establishing a connection is taking too long."), pSimplifiedVersion);

		case Code::Network_Proxy_Error:
		case Code::Workflow_TrustedChannel_Proxy_Error:
			return maskMessage(tr("Establishing a connection via the proxy did not succeed."), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_Server_Format_Error:
			return maskMessage(tr("It wasn't possible to connect to the server: the server sent a non-standard response."), pSimplifiedVersion);

		case Code::Network_Ssl_Establishment_Error:
		case Code::Workflow_TrustedChannel_Ssl_Establishment_Error:
			return maskMessage(tr("It wasn't possible to connect to the server: a secure connection could not be established."), pSimplifiedVersion);

		case Code::Workflow_Wrong_Parameter_Invocation:
			return maskMessage(tr("Application was invoked with wrong parameters."), pSimplifiedVersion);

		case Code::Network_Other_Error:
		case Code::Workflow_TrustedChannel_Other_Network_Error:
			return maskMessage(tr("An unknown network error occurred."), pSimplifiedVersion);

		case Code::Workflow_Reader_Became_Inaccessible:
			return tr("The selected card reader cannot be accessed anymore.");

		case Code::Workflow_Reader_Communication_Error:
			return tr("An error occurred while communicating with the card reader.");

		case Code::Workflow_Server_Incomplete_Information_Provided:
			return tr("The server provided no or incomplete information. Your personal data could not be read out.");

		case Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
			return maskMessage(tr("Error while connecting to the service provider. The SSL connection uses an unsupported key algorithm or length."), pSimplifiedVersion);

		case Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
			return maskMessage(tr("Error while connecting to the server. The SSL certificate uses an unsupported key algorithm or length."), pSimplifiedVersion);

		case Code::Workflow_Network_Empty_Redirect_Url:
			return maskMessage(tr("Empty redirect URL"), pSimplifiedVersion);

		case Code::Workflow_Network_Expected_Redirect:
			return maskMessage(tr("Expected redirect, got %1").arg(getExternalInfo()), pSimplifiedVersion);

		case Code::Workflow_Network_Invalid_Scheme:
			return maskMessage(tr("Invalid scheme: %1").arg(getExternalInfo()), pSimplifiedVersion);

		case Code::Workflow_Network_Malformed_Redirect_Url:
			return maskMessage(tr("Malformed redirect URL: %1").arg(getExternalInfo()), pSimplifiedVersion);

		case Code::Workflow_Cancellation_By_User:
		case Code::Card_Cancellation_By_User:
			return tr("The process was cancelled by the user.");

		case Code::Paos_Unexpected_Warning:
		case Code::Paos_Error_AL_Unknown_Error:
		case Code::Paos_Error_AL_No_Permission:
		case Code::Paos_Error_AL_Internal_Error:
		case Code::Paos_Error_AL_Parameter_Error:
		case Code::Paos_Error_AL_Unkown_API_Function:
		case Code::Paos_Error_AL_Not_Initialized:
		case Code::Paos_Error_AL_Warning_Connection_Disconnected:
		case Code::Paos_Error_AL_Session_Terminated_Warning:
		case Code::Paos_Error_AL_Communication_Error:
		case Code::Paos_Error_DP_Timeout_Error:
		case Code::Paos_Error_DP_Unknown_Channel_Handle:
		case Code::Paos_Error_DP_Communication_Error:
		case Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
		case Code::Paos_Error_DP_Unknown_Protocol:
		case Code::Paos_Error_DP_Unknown_Cipher_Suite:
		case Code::Paos_Error_DP_Unknown_Webservice_Binding:
		case Code::Paos_Error_DP_Node_Not_Reachable:
		case Code::Paos_Error_IFDL_Timeout_Error:
		case Code::Paos_Error_IFDL_InvalidSlotHandle:
		case Code::Paos_Error_IFDL_CancellationByUser:
		case Code::Paos_Error_KEY_KeyGenerationNotPossible:
		case Code::Paos_Error_SAL_Cancellation_by_User:
		case Code::Paos_Error_SAL_CertificateChainInterrupted:
		case Code::Paos_Error_SAL_Invalid_Key:
		case Code::Paos_Error_SAL_SecurityConditionNotSatisfied:
		case Code::Paos_Error_SAL_MEAC_AgeVerificationFailedWarning:
		case Code::Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning:
		case Code::Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed:
			return getExternalInfo();

		case Code::Card_Input_TimeOut:
			return tr("The maximum time was exceeded during input process.");

		case Code::Card_Not_Found:
			return tr("Card does not exist");

		case Code::Card_Communication_Error:
			return tr("An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.");

		case Code::Card_Protocol_Error:
			return QStringLiteral("%1 <a href=\"%2\">%3</a>.").arg(
					tr("A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at"),
					tr("https://www.ausweisapp.bund.de/en/service/support/"),
					tr("AusweisApp2 Support"));

		case Code::Card_Invalid_Pin:
			return tr("The given PIN is invalid.");

		case Code::Card_Invalid_Can:
			return tr("The given card access number (CAN) is invalid.");

		case Code::Card_Invalid_Puk:
			return tr("The given PUK is invalid.");

		case Code::Card_Pin_Blocked:
			return tr("The PIN was blocked after too many unsuccessful attempts.");

		case Code::Card_Pin_Not_Blocked:
			return tr("The PIN is not blocked.");

		case Code::Card_Puk_Blocked:
			return tr("The PUK was used ten times and is set inoperative. Please contact the competent authority that issued your ID document to unlock the PIN.");

		case Code::Card_NewPin_Mismatch:
			return tr("The new PIN and the confirmation do not match.");

		case Code::Card_NewPin_Invalid_Length:
			return tr("The length of the new PIN is not valid.");

		case Code::Workflow_Reader_Device_Connection_Error:
			return tr("An error occurred while connecting to a reader device.");

		case Code::Workflow_Reader_Device_Scan_Error:
			return tr("An error occurred while scanning for reader devices.");

		case Code::RemoteReader_CloseCode_NormalClose:
			return tr("The remote reader connection was closed properly.");

		case Code::RemoteReader_CloseCode_AbnormalClose:
			return tr("The remote card reader connection was not closed properly.");

		case Code::RemoteReader_CloseCode_Undefined:
			return tr("Undefined error code occured when the remote card reader connection was closed.");

		case Code::RemoteConnector_InvalidRequest:
			return tr("Remote reader connection request contains invalid parameters.");

		case Code::RemoteConnector_EmptyPassword:
			return tr("Empty password in extended encryption of remote reader connection request.");

		case Code::RemoteConnector_NoSupportedApiLevel:
			return tr("Your remote reader version is incompatible with the local version. Please install the latest AusweisApp2 version on both your smartphone and your computer.");

		case Code::RemoteConnector_ConnectionTimeout:
			return tr("A timeout occurred while trying to establish a connection to a remote reader.");

		case Code::RemoteConnector_ConnectionError:
			return tr("An error occurred while trying to establish a connection to a remote reader.");

		case Code::RemoteConnector_RemoteHostRefusedConnection:
			return tr("Remote device has rejected the connection. Please check the pairing code.");

		case Code::Downloader_File_Not_Found:
			return tr("File not found.");

		case Code::Downloader_Cannot_Save_File:
			return tr("Cannot save file.");

		case Code::Downloader_Data_Corrupted:
			return tr("Received data were corrupted.");
	}

	Q_UNREACHABLE();
}


GlobalStatus::Origin GlobalStatus::getOrigin() const
{
	return d->mOrigin;
}


bool GlobalStatus::isOriginServer() const
{
	return d->mOrigin == Origin::Server;
}


bool GlobalStatus::isNoError() const
{
	return d->mStatusCode == Code::No_Error;
}


bool GlobalStatus::isError() const
{
	return !isNoError();
}


bool GlobalStatus::isCancellationByUser() const
{
	return d->mStatusCode == Code::Workflow_Cancellation_By_User ||
		   d->mStatusCode == Code::Card_Cancellation_By_User ||
		   d->mStatusCode == Code::Paos_Error_SAL_Cancellation_by_User;
}


QDebug operator <<(QDebug pDbg, const GlobalStatus& pStatus)
{
	pDbg << pStatus.getStatusCode() << "|" << pStatus.toErrorDescription();
	return pDbg;
}
