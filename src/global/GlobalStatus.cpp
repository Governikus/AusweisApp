/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "GlobalStatus.h"

#include "Initializer.h"

#include <QDebug>

using namespace governikus;

static Initializer::Entry E([] {
			qRegisterMetaType<GlobalStatus::Code>("GlobalStatus::Code");
			qRegisterMetaType<GlobalStatus>("GlobalStatus");
		});


const QString GlobalStatus::getExternalInfo(int pIndex) const
{
	return d->mExternalInformation.value(pIndex);
}


bool GlobalStatus::isMessageMasked() const
{
	switch (d->mStatusCode)
	{
		case Code::Workflow_Unknown_Paos_Form_EidServer:
		case Code::Workflow_Unexpected_Message_From_EidServer:
		case Code::Workflow_Preverification_Error:
		case Code::Workflow_No_Unique_AtCvc:
		case Code::Workflow_No_Unique_DvCvc:
		case Code::Workflow_Certificate_No_Description:
		case Code::Workflow_Certificate_No_Url_In_Description:
		case Code::Workflow_Certificate_Hash_Error:
		case Code::Workflow_Certificate_Sop_Error:
		case Code::Workflow_Error_Page_Transmission_Error:
		case Code::Workflow_Processing_Error:
		case Code::Workflow_Redirect_Transmission_Error:
		case Code::Workflow_TrustedChannel_Establishment_Error:
		case Code::Workflow_TrustedChannel_Error_From_Server:
		case Code::Workflow_TrustedChannel_No_Data_Received:
		case Code::Network_TimeOut:
		case Code::Workflow_TrustedChannel_TimeOut:
		case Code::Network_Proxy_Error:
		case Code::Workflow_TrustedChannel_Proxy_Error:
		case Code::Workflow_TrustedChannel_Server_Format_Error:
		case Code::Network_Ssl_Establishment_Error:
		case Code::Workflow_TrustedChannel_Ssl_Establishment_Error:
		case Code::Workflow_Wrong_Parameter_Invocation:
		case Code::Network_Other_Error:
		case Code::Workflow_TrustedChannel_Other_Network_Error:
		case Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Empty_Redirect_Url:
		case Code::Workflow_Network_Expected_Redirect:
		case Code::Workflow_Network_Invalid_Scheme:
		case Code::Workflow_Network_Malformed_Redirect_Url:
			return true;

		default:
			return false;
	}
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


QString GlobalStatus::toErrorDescription(const bool pSimplifiedVersion) const
{
	if (pSimplifiedVersion && isMessageMasked())
	{
		const QString supportUrl = tr("https://www.ausweisapp.bund.de/en/qa/support/");
		const QString hyperlink = QStringLiteral("<a href=\"%1\">").arg(supportUrl);

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
		QString message = tr("An error occurred. Please contact our %1support%2.").arg(hyperlink, QStringLiteral("</a>"));
#else
		QString message = tr("An error occurred. Please contact our %1support%2 or feel free to send us an email.").arg(hyperlink, QStringLiteral("</a>"));
#endif
		return message;
	}
	return toErrorDescriptionInternal();
}


QString GlobalStatus::toErrorDescriptionInternal() const
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
			return tr("The program received an unknown message from the server.");

		case Code::Workflow_Unexpected_Message_From_EidServer:
			return tr("The program received an unexpected message from the server.");

		case Code::Workflow_Pin_Blocked_And_Puk_Objectionable:
			return tr("After three wrong entries your PIN is blocked. Please use the PIN management in this app to unblock it with the help of your PUK.");

		case Code::Workflow_Preverification_Developermode_Error:
			return tr("Using the developer mode is only allowed in a test environment.");

		case Code::Workflow_Preverification_Error:
			return tr("Pre-verification failed.");

		case Code::Workflow_No_Unique_AtCvc:
			return tr("No unique AT CVC");

		case Code::Workflow_No_Unique_DvCvc:
			return tr("No unique DV CVC");

		case Code::Workflow_No_Permission_Error:
			return tr("Authentication failed.");

		case Code::Workflow_Certificate_No_Description:
			return tr("No certificate description available.");

		case Code::Workflow_Certificate_No_Url_In_Description:
			return tr("No subject url available in certificate description.");

		case Code::Workflow_Certificate_Hash_Error:
			return tr("The certificate description does not match the certificate.");

		case Code::Workflow_Certificate_Sop_Error:
			return tr("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy.");

		case Code::Workflow_Error_Page_Transmission_Error:
		case Code::Workflow_Processing_Error:
		case Code::Workflow_Redirect_Transmission_Error:
			return getExternalInfo();

		case Code::Workflow_TrustedChannel_Establishment_Error:
			return tr("Failed to establish secure connection");

		case Code::Workflow_TrustedChannel_Error_From_Server:
			return tr("The program received an error from the server.");

		case Code::Workflow_TrustedChannel_Hash_Not_In_Description:
		case Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description:
			return tr("Hash of certificate not in certificate description (issuer: %1). This indicates a misconfiguration or manipulation of the certificate. Please check that your antivirus-software and firewalls are not interfering with SSL traffic.").arg(getExternalInfo());

		case Code::Workflow_TrustedChannel_No_Data_Received:
			return tr("Received no data.");

		case Code::Workflow_TrustedChannel_ServiceUnavailable:
		case Code::Network_ServiceUnavailable:
			return tr("The service is temporarily not available. Please try again later.");

		case Code::Network_TimeOut:
		case Code::Workflow_TrustedChannel_TimeOut:
			return tr("Establishing a connection is taking too long.");

		case Code::Network_Proxy_Error:
		case Code::Workflow_TrustedChannel_Proxy_Error:
			return tr("Establishing a connection via the proxy did not succeed.");

		case Code::Workflow_TrustedChannel_Server_Format_Error:
			return tr("It wasn't possible to connect to the server: the server sent a non-standard response.");

		case Code::Network_Ssl_Establishment_Error:
		case Code::Workflow_TrustedChannel_Ssl_Establishment_Error:
			return tr("It wasn't possible to connect to the server: a secure connection could not be established.");

		case Code::Workflow_Wrong_Parameter_Invocation:
			return tr("Application was invoked with wrong parameters.");

		case Code::Network_Other_Error:
		case Code::Workflow_TrustedChannel_Other_Network_Error:
			return tr("An unknown network error occurred.");

		case Code::Workflow_Reader_Became_Inaccessible:
			return tr("The selected card reader cannot be accessed anymore.");

		case Code::Workflow_Reader_Communication_Error:
			return tr("An error occurred while communicating with the card reader.");

		case Code::Workflow_Server_Incomplete_Information_Provided:
			return tr("The server provided no or incomplete information. Your personal data could not be read out.");

		case Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
			return tr("Error while connecting to the service provider. The SSL connection uses an unsupported key algorithm or length.");

		case Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
			return tr("Error while connecting to the server. The SSL certificate uses an unsupported key algorithm or length. Certificate issuer: %1").arg(getExternalInfo());

		case Code::Workflow_Network_Empty_Redirect_Url:
			return tr("Empty redirect URL");

		case Code::Workflow_Network_Expected_Redirect:
			return tr("Expected redirect, got %1").arg(getExternalInfo());

		case Code::Workflow_Network_Invalid_Scheme:
			return tr("Invalid scheme: %1").arg(getExternalInfo());

		case Code::Workflow_Network_Malformed_Redirect_Url:
			return tr("Malformed redirect URL: %1").arg(getExternalInfo());

		case Code::Workflow_Cancellation_By_User:
		case Code::Card_Cancellation_By_User:
			return tr("The process was cancelled by the user.");

		case Code::Paos_Generic_Server_Error:
		case Code::Paos_Unexpected_Warning:
		case Code::Paos_Error_AL_Unknown_Error:
		case Code::Paos_Error_AL_Internal_Error:
		case Code::Paos_Error_AL_Communication_Error:
		case Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
		case Code::Paos_Error_SAL_Cancellation_by_User:
		case Code::Paos_Error_SAL_Invalid_Key:
			return getExternalInfo();

		case Code::Card_Input_TimeOut:
			return tr("The maximum time was exceeded during input process.");

		case Code::Card_Not_Found:
			return tr("Card does not exist");

		case Code::Card_Communication_Error:
			return tr("An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.");

		case Code::Card_Protocol_Error:
		case Code::Card_Unexpected_Transmit_Status:
			return QStringLiteral("%1 <a href=\"%2\">%3</a>.").arg(
					tr("A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at"),
					tr("https://www.ausweisapp.bund.de/en/qa/support/"),
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
