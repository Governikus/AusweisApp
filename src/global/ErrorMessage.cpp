/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ErrorMessage.h"


using namespace governikus;

const char* const ErrorMessage::CONTACT_SUPPORT = QT_TR_NOOP("An error occurred. Please contact our support at <a href=\"https://www.ausweisapp.bund.de/en/service/support/\">AusweisApp2 Support</a>.");

const QMap<ErrorMessageId, const char*> ErrorMessage::mMessages = {
	{
		ErrorMessageId::WRONG_PARAMETER_INVOCATION,
		QT_TR_NOOP("Application was invoked with wrong parameters.")
	},
	{
		ErrorMessageId::SSL_ERROR,
		QT_TR_NOOP("It wasn't possible to connect to the server: a secure connection could not be established.")
	},
	{
		ErrorMessageId::FORMAT_ERROR,
		QT_TR_NOOP("It wasn't possible to connect to the server: the server sent a non-standard response.")
	},
	{
		ErrorMessageId::TIME_OUT,
		QT_TR_NOOP("Establishing a connection is taking too long.")
	},
	{
		ErrorMessageId::PROXY_ERROR,
		QT_TR_NOOP("Establishing a connection via the proxy did not succeed.")
	}
};

const QMap<ErrorMessageId, const char*> ErrorMessage::mAdvices = {
	{
		ErrorMessageId::WRONG_PARAMETER_INVOCATION,
		CONTACT_SUPPORT
	},
	{
		ErrorMessageId::SSL_ERROR,
		CONTACT_SUPPORT
	},
	{
		ErrorMessageId::FORMAT_ERROR,
		CONTACT_SUPPORT
	},
	{
		ErrorMessageId::TIME_OUT,
		CONTACT_SUPPORT
	},
	{
		ErrorMessageId::PROXY_ERROR,
		CONTACT_SUPPORT
	}
};

QString ErrorMessage::toString(ErrorMessageId pId)
{
	return tr(mAdvices.value(pId, nullptr));
}
