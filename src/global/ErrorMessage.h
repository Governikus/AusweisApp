/*!
 * \brief Localized error messages to be used in different workflow steps.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

#include <QCoreApplication>
#include <QMap>

namespace governikus
{

defineEnumType(ErrorMessageId,
		WRONG_PARAMETER_INVOCATION,
		SSL_ERROR,
		FORMAT_ERROR,
		TIME_OUT,
		PROXY_ERROR)


class ErrorMessage
{
	Q_DECLARE_TR_FUNCTIONS(governikus::ErrorMessage)

	public:
		static QString toString(ErrorMessageId pId);

		static const char* const CONTACT_SUPPORT;

	private:
		ErrorMessage() = delete;

		~ErrorMessage() = delete;

		static const QMap<ErrorMessageId, const char*> mMessages;
		static const QMap<ErrorMessageId, const char*> mAdvices;
};


} /* namespace governikus */
