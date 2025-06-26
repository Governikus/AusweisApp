/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "GlobalStatus.h"

#include "Initializer.h"
#include "LanguageLoader.h"

#include <QDebug>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<GlobalStatus::Code>("GlobalStatus::Code");
			qRegisterMetaType<GlobalStatus>("GlobalStatus");
		})


QString GlobalStatus::getExternalInfo(ExternalInformation pType) const
{
	return d->mExternalInformation.value(pType);
}


bool GlobalStatus::isMessageMasked() const
{
	switch (d->mStatusCode)
	{
		case Code::Workflow_Unknown_Paos_From_EidServer:
		case Code::Workflow_Start_Paos_Response_Missing:
		case Code::Workflow_Unexpected_Message_From_EidServer:
		case Code::Workflow_Preverification_Error:
		case Code::Workflow_No_Unique_AtCvc:
		case Code::Workflow_No_Unique_DvCvc:
		case Code::Workflow_Certificate_No_Description:
		case Code::Workflow_Certificate_No_Url_In_Description:
		case Code::Workflow_Certificate_Hash_Error:
		case Code::Workflow_Certificate_Sop_Error:
		case Code::Workflow_TrustedChannel_Establishment_Error:
		case Code::Workflow_TrustedChannel_Server_Error:
		case Code::Workflow_TrustedChannel_Client_Error:
		case Code::Workflow_TrustedChannel_No_Data_Received:
		case Code::Workflow_TrustedChannel_Other_Network_Error:
		case Code::Network_Other_Error:
		case Code::Network_TimeOut:
		case Code::Workflow_TrustedChannel_TimeOut:
		case Code::Network_Proxy_Error:
		case Code::Workflow_TrustedChannel_Proxy_Error:
		case Code::Workflow_TrustedChannel_Server_Format_Error:
		case Code::Network_Ssl_Establishment_Error:
		case Code::Workflow_Wrong_Parameter_Invocation:
		case Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Empty_Redirect_Url:
		case Code::Workflow_Network_Expected_Redirect:
		case Code::Workflow_Network_Invalid_Scheme:
		case Code::Workflow_Network_Malformed_Redirect_Url:
		case Code::Paos_Unexpected_Warning:
		case Code::Paos_Error_AL_Unknown_Error:
		case Code::Paos_Error_AL_Internal_Error:
		case Code::Paos_Error_AL_Communication_Error:
		case Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
		case Code::Paos_Error_SAL_Cancellation_by_User:
		case Code::Paos_Error_SAL_Invalid_Key:
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
		const QString hyperlink = QStringLiteral("<a href=\"https://www.ausweisapp.bund.de/%1/aa2/support\">").arg(LanguageLoader::getLocaleCode());
		//: ERROR ALL_PLATFORMS Error message which is used for "masked" errors. Generic message with link to support section of the homepage.
		QString message = tr("A secure connection to the provider could not be established. Check the network connection and try again later. If this doesn't help, contact our %1support%2.").arg(hyperlink, QStringLiteral("</a>"));

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
			//: ERROR ALL_PLATFORMS No actual error occurred, required to provide a message for status code No_Error.
			return tr("No error occurred.");

		case Code::Unknown_Error:
			//: ERROR ALL_PLATFORMS An unknown error from any of the subcomponents (PCSC, EcardAPI, QtNetwork, ...) occurred.
			return tr("An unexpected error has occurred during processing.");

		case Code::Workflow_AlreadyInProgress_Error:
			//: ERROR ALL_PLATFORMS An ActivationHandler is requested even though an operation is currently still running.
			return tr("Cannot start authentication. An operation is already active.");

		case Code::Workflow_Card_Removed:
			//: ERROR ALL_PLATFORMS The card was removed after the PACE channel was established.
			return tr("Restart the authentication process and make sure that the position of the ID card does not change during the reading process.");

		case Code::Workflow_Cannot_Confirm_IdCard_Authenticity:
			//: ERROR ALL_PLATFORMS The certificates supplied by the card did not pass the authenticity check, further operation is not allowed.
			return tr("The authenticity of your ID card could not be confirmed.");

		case Code::Workflow_Unknown_Paos_From_EidServer:
			//: ERROR_MASKED ALL_PLATFORMS The type of a PAOS message could not be determined.
			return tr("The program received an unknown message from the server.");

		case Code::Workflow_Start_Paos_Response_Missing:
			//: ERROR_MASKED ALL_PLATFORMS The PAOS message StartPaosResponse was not received.
			return tr("The program did not receive a StartPaosResponse message from the server.");

		case Code::Workflow_Unexpected_Message_From_EidServer:
			//: ERROR_MASKED ALL_PLATFORMS The server sent a valid PAOS message but its type was unexpected.
			return tr("The program received an unexpected message from the server.");

		case Code::Workflow_Preverification_Developermode_Error:
			//: ERROR ALL_PLATFORMS The developer mode is enabled but a productive environment was detected.
			return tr("Using the developer mode is only allowed in a test environment.");

		case Code::Workflow_Preverification_Error:
			//: ERROR_MASKED ALL_PLATFORMS The certificates submitted by the server failed the authenticity check during pre-verification.
			return tr("Pre-verification failed.");

		case Code::Workflow_No_Unique_AtCvc:
			//: ERROR_MASKED ALL_PLATFORMS The EAC1 request contained more than one At certificate.
			return tr("No unique AT CVC");

		case Code::Workflow_No_Unique_DvCvc:
			//: ERROR_MASKED ALL_PLATFORMS The EAC1 request contained more than one Dv certificate.
			return tr("No unique DV CVC");

		case Code::Workflow_No_Permission_Error:
			//: ERROR ALL_PLATFORMS DidAuthenticateEAC2, AA or the ID card declined the certificates.
			return tr("Authentication failed.");

		case Code::Workflow_Wrong_Length_Error:
			//: ERROR ALL_PLATFORMS A card command failed because the data length was wrong or the card reader does not support Extended Length.
			return tr("The length of the data sent to the ID card was not accepted. Either the data is faulty or your card reader does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.").arg(QCoreApplication::applicationName());

		case Code::Workflow_Certificate_No_Description:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("No certificate description available.");

		case Code::Workflow_Certificate_No_Url_In_Description:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("No subject url available in certificate description.");

		case Code::Workflow_Certificate_Hash_Error:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("The certificate description does not match the certificate.");

		case Code::Workflow_Certificate_Sop_Error:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("The subject URL in the certificate description and the TCToken URL do not satisfy the same origin policy.");

		case Code::Workflow_Browser_Transmission_Error:
			return getExternalInfo();

		case Code::Workflow_TrustedChannel_Server_Error:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("The program received an error from the server.");

		case Code::Workflow_TrustedChannel_Client_Error:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("The server could not process the client request.");

		case Code::Workflow_TrustedChannel_Hash_Not_In_Description:
		case Code::Workflow_Network_Ssl_Hash_Not_In_Certificate_Description:
			//: ERROR ALL_PLATFORMS The TLS certificate was not folded with the Authorization Certificate, thus violating the security requirements. Might also be caused by a firewall and/or the antivirus software.
			return tr("Hash of TLS certificate not in certificate description (issuer: %1). This indicates a misconfiguration or manipulation of the certificate. Please check that your antivirus software and firewall are not interfering with TLS traffic.").arg(getExternalInfo(ExternalInformation::CERTIFICATE_ISSUER_NAME));

		case Code::Workflow_TrustedChannel_No_Data_Received:
			//: ERROR_MASKED ALL_PLATFORMS Received an empty TC token.
			return tr("The TCToken contained no data.");

		case Code::Workflow_TrustedChannel_ServiceUnavailable:
		case Code::Network_ServiceUnavailable:
			//: ERROR ALL_PLATFORMS A server has responded with an HTTP error code 503.
			return tr("The service is temporarily not available. Please try again later.");

		case Code::Network_ServerError:
			//: ERROR ALL_PLATFORMS A server has responded with an HTTP error code 5xx.
			return tr("The service encountered an internal error while processing a request.");

		case Code::Network_ClientError:
			//: ERROR ALL_PLATFORMS A server has responded with an HTTP error code 4xx.
			return tr("The service reported an error while processing a client request.");

		case Code::Workflow_Smart_eID_Unavailable:
			//: ERROR ALL_PLATFORMS The device does not support the Smart-eID function
			return tr("The device does not support Smart-eID.");

		case Code::Workflow_Smart_eID_Applet_Preparation_Failed:
			//: ERROR ANDROID The preparation of the Smart-eID Applet failed
			return tr("The preparation of the Smart-eID failed.");

		case Code::Workflow_Smart_eID_Authentication_Failed:
			//: ERROR ALL_PLATFORMS No sessionID, required for a personalization, was received
			return tr("The authentication to the personalization service failed.");

		case Code::Workflow_Smart_eID_ServiceInformation_Query_Failed:
			//: ERROR ALL_PLATFORMS Failed to get the ServiceInformation of the Smart-eID
			return tr("Failed to get the ServiceInformation of the Smart-eID.");

		case Code::Workflow_Smart_eID_PrePersonalization_Failed:
			//: ERROR ALL_PLATFORMS Initialization of Personalization failed
			return tr("Initialization of Personalization of Smart-eID failed.");

		case Code::Workflow_Smart_eID_Personalization_Failed:
			//: ERROR ALL_PLATFORMS Personalization of Smart-eID failed
			return tr("Personalization of Smart-eID failed.");

		case Code::Workflow_Smart_eID_Personalization_Denied:
			//: ERROR ALL_PLATFORMS Personalization of Smart-eID is not allowed, no remaining attempts are left.
			return tr("You have reached the allowed amount of Smart-eID setups for the current period. You may set up another Smart-eID with your ID card on %1.").arg(getExternalInfo(ExternalInformation::PERSONALIZATION_RESTRICTION_DATE));

		case Code::Network_TimeOut:
		case Code::Workflow_TrustedChannel_TimeOut:
			//: ERROR_MASKED ALL_PLATFORMS The TCP connection to the server timed out.
			return tr("Establishing a connection is taking too long.");

		case Code::Network_Proxy_Error:
		case Code::Workflow_TrustedChannel_Proxy_Error:
			//: ERROR_MASKED ALL_PLATFORMS The connection using a proxy failed, it may be misconfigured..
			return tr("Establishing a connection via the proxy did not succeed.");

		case Code::Workflow_TrustedChannel_Server_Format_Error:
			//: ERROR_MASKED ALL_PLATFORMS An critical error while retrieving or parsing the TC token occurred.
			return tr("It wasn't possible to connect to the server: the server sent a non-standard response.");

		case Code::Network_Ssl_Establishment_Error:
		case Code::Workflow_TrustedChannel_Establishment_Error:
			//: ERROR_MASKED ALL_PLATFORMS Critical error from Qt's TLS API.
			return tr("It wasn't possible to connect to the server: a secure connection could not be established.");

		case Code::Workflow_Wrong_Parameter_Invocation:
			//: ERROR_MASKED ALL_PLATFORMS The TC token URL could not be parsed/validated.
			return tr("Application was invoked with wrong parameters.");

		case Code::Network_Other_Error:
		case Code::Workflow_TrustedChannel_Other_Network_Error:
			//: ERROR ALL_PLATFORMS Other critical network error by Qt.
			return tr("An unknown network error occurred. Check your network connection and try to restart the app.");

		case Code::Workflow_Reader_Became_Inaccessible:
			//: ERROR ALL_PLATFORMS The card reader was removed after the PACE channel was established.
			return tr("The selected card reader cannot be accessed anymore.");

		case Code::Workflow_Server_Incomplete_Information_Provided:
			//: ERROR ALL_PLATFORMS The result of the self authentication could not be retrieved from the eService.
			return tr("The server provided no or incomplete information. Your personal data could not be read out.");

		case Code::Workflow_TrustedChannel_Ssl_Connection_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
			//: ERROR_MASKED ALL_PLATFORMS
			return tr("Error while connecting to the provider. The TLS connection uses an unsupported key algorithm or length.");

		case Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
		case Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
			//: ERROR ALL_PLATFORMS Received a TLS certificate that uses an invalid algorithm or key length.
			return tr("Error while connecting to the server. The TLS certificate uses an unsupported key algorithm or length. Certificate issuer: %1").arg(getExternalInfo(ExternalInformation::CERTIFICATE_ISSUER_NAME));

		case Code::Workflow_Network_Empty_Redirect_Url:
			//: ERROR_MASKED ALL_PLATFORMS The redirect URL could not be determined because the server sent an empty response.
			return tr("Empty redirect URL");

		case Code::Workflow_Network_Expected_Redirect:
			//: ERROR_MASKED ALL_PLATFORMS The redirect URL could not be determined due to an erroneous HTTP code.
			return tr("Expected redirect, got %1").arg(getExternalInfo(ExternalInformation::HTTP_STATUS_CODE));

		case Code::Workflow_Network_Invalid_Scheme:
			//: ERROR_MASKED ALL_PLATFORMS The redirect URL could not be determined because the redirect URL did not adhere to the HTTPS scheme.
			return tr("Invalid scheme: %1").arg(getExternalInfo(ExternalInformation::URL_SCHEME));

		case Code::Workflow_Network_Malformed_Redirect_Url:
			//: ERROR_MASKED ALL_PLATFORMS The redirect URL could not be determined because the redirect URL was invalid.
			return tr("Malformed redirect URL: %1").arg(getExternalInfo(ExternalInformation::REDIRECT_URL));

		case Code::Workflow_Cancellation_By_User:
			//: ERROR ALL_PLATFORMS The user cancelled the authentication in the UI.
			return tr("The process has been cancelled.");

		case Code::Workflow_InternalError_BeforeTcToken:
			//: ERROR ALL_PLATFORMS An internal error occurred before fetching TcToken.
			return tr("The process has been aborted because of an internal error.");

		case Code::Card_Cancellation_By_User:
			//: ERROR ALL_PLATFORMS The user cancelled the authentication on his card reader.
			return tr("The process has been cancelled by the card reader.");

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
			//: ERROR ALL_PLATFORMS The card reader signalled that it timed out.
			return tr("The maximum time was exceeded during input process.");

		case Code::Card_Not_Found:
			//: ERROR ALL_PLATFORMS Internal error, either PCSC, SaK or card reader could not find the ID card.
			return tr("Card does not exist");

		case Code::Card_Pin_Deactivated:
			//: ERROR ALL_PLATFORMS The eID function of the ID card is not active.
			return tr("The eID function of your ID card is not yet activated.");

		case Code::Card_Communication_Error:
			//: ERROR ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie),
			return tr("An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.");

		case Code::Card_Protocol_Error:
		case Code::Card_Unexpected_Transmit_Status:
			//: ERROR ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie). The protocol was faulty or invalid values were requested/received. %1 is a html link to the support.
			return tr("A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at %1.").arg(
					QStringLiteral("<a href=\"https://www.ausweisapp.bund.de/%1/aa2/support\">%2</a>").arg(
					LanguageLoader::getLocaleCode(),
							//: LABEL ALL_PLATFORMS Link text to the app support. %1 is the app name.
					tr("%1 Support").arg(QCoreApplication::applicationName())));

		case Code::Card_Invalid_Pin:
			//: ERROR ALL_PLATFORMS The ID card declined the PIN.
			return tr("The given PIN is not correct.");

		case Code::Card_Invalid_Can:
			//: ERROR ALL_PLATFORMS The ID card declined the CAN.
			return tr("The given Card Access Number (CAN) is not correct.");

		case Code::Card_Invalid_Puk:
			//: ERROR ALL_PLATFORMS The ID card declined the PUK.
			return tr("The given PUK is not correct.");

		case Code::Card_Pin_Blocked:
			//: ERROR ALL_PLATFORMS The ID card refused the PIN since the PIN feature is blocked after too many wrong attempts.
			return tr("The PIN was blocked after too many unsuccessful attempts.");

		case Code::Card_Pin_Not_Blocked:
			//: ERROR ALL_PLATFORMS It was attempted to unlock the ID card via PUK even though it was not locked in the first place. This scenario is avoided in the UI by hiding the respective UI elements.
			return tr("The PIN is not blocked.");

		case Code::Card_Puk_Blocked:
			//: ERROR ALL_PLATFORMS The card declined the PUK since it was entered wrongfully 10 times.
			return tr("The entered PUK has already been used 10 times and can no longer be used to unblock the PIN.");

		case Code::Card_NewPin_Mismatch:
			//: ERROR ALL_PLATFORMS The card reader signalled that the new PIN was not confirmed correctly.
			return tr("The new PIN and the confirmation do not match.");

		case Code::Card_NewPin_Invalid_Length:
			//: ERROR ALL_PLATFORMS The card reader declined the new PIN since its length was invalid.
			return tr("The length of the new PIN is not valid.");

		case Code::Card_ValidityVerificationFailed:
			//: ERROR ALL_PLATFORMS The validity verification of the card failed.
			return tr("The validity verification of the card failed.");

		case Code::Card_Smart_Invalid:
			//: ERROR ALL_PLATFORMS The existing Smart-eID was invalidated.
			return tr("The Smart-eID is no longer ready for use. This might have been caused by entering the wrong Smart-eID PIN 3 times. You may personalize a new Smart-eID to resolve the issue.");

		case Code::RemoteReader_CloseCode_AbnormalClose:
			//: ERROR ALL_PLATFORMS The connection to the smartphone card reader (SaK) was lost.
			return tr("The smartphone as card reader (SaC) connection was aborted.");

		case Code::IfdConnector_InvalidRequest:
			//: ERROR ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (missing device ID).
			return tr("Smartphone as card reader (SaC) connection request was invalid.");

		case Code::IfdConnector_NoSupportedApiLevel:
			//: ERROR ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (API mismatch).
			return tr("Your smartphone as card reader (SaC) version is incompatible with the local version. Please install the latest %1 version on both your smartphone and your computer.").arg(QCoreApplication::applicationName());

		case Code::IfdConnector_ConnectionError:
			//: ERROR ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) failed due to network errors (Host not found, OS error, ...)
			return tr("An error occurred while trying to establish a connection to the smartphone as card reader (SaC).");

		case Code::IfdConnector_RemoteHostRefusedConnection:
			//: ERROR ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was rejected by the device.
			return tr("The smartphone to be paired has rejected the connection. Please check the pairing code.");

		case Code::Downloader_File_Not_Found:
			//: ERROR ALL_PLATFORMS Download of the file failed with HTTP error code 404.
			return tr("File not found.");

		case Code::Downloader_Cannot_Save_File:
			//: ERROR ALL_PLATFORMS Saving the downloaded file on the local disk failed.
			return tr("Cannot save file.");

		case Code::Downloader_Data_Corrupted:
			//: ERROR ALL_PLATFORMS The downloaded file contained unsupported/invalid data.
			return tr("Received data were corrupted.");

		case Code::Downloader_Missing_Platform:
			//: ERROR ALL_PLATFORMS The downloaded file does not contain data for the current platform.
			return tr("Received data does not contain data for the current platform.");

		case Code::Downloader_Aborted:
			//: ERROR ALL_PLATFORMS The download is aborted.
			return tr("Download aborted.");

		case Code::Update_Execution_Failed:
			//: ERROR ALL_PLATFORMS Starting the update failed.
			return tr("A new process to start the update could not be launched.");
	}

	Q_UNREACHABLE();
}


QString GlobalStatus::getExternalInfo(const QString& pToken) const
{
	if (d->mExternalInformation.empty())
	{
		return {};
	}

	if (d->mExternalInformation.size() == 1)
	{
		return d->mExternalInformation.first();
	}

	QStringList keyValue;
	const auto& keys = d->mExternalInformation.keys();
	for (const auto key : keys)
	{
		keyValue << QStringLiteral("%1: %2").arg(Enum<ExternalInformation>::getName(key), d->mExternalInformation[key]);
	}

	return keyValue.join(pToken);
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
	pDbg << pStatus.getStatusCode() << '|' << pStatus.toErrorDescription();
	return pDbg;
}
