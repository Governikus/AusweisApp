/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "HttpStatusCode.h"
#include "Result.h"

#include <QObject>
#include <QString>
#include <QUrl>

namespace governikus
{


class ActivationContext
	: public QObject
{
	Q_OBJECT

	protected:
		QString mSendError;

	public:
		ActivationContext();
		virtual ~ActivationContext();

		virtual QUrl getActivationURL() = 0;

		/*!
		 * Sends a processing status response to the caller.
		 *
		 * \return true, if sending succeeded, false otherwise. On failure an error message can be retrieved via getSendError.
		 */
		virtual bool sendProcessing() = 0;

		/*!
		 * Sends a response to the caller indicating that another operation is already in progress
		 *
		 * \return true, if sending succeeded, false otherwise. On failure an error message can be retrieved via getSendError.
		 */
		virtual bool sendOperationAlreadyActive() = 0;

		/*!
		 * Sends an error page to the caller.
		 *
		 * \return true, if sending succeeded, false otherwise. On failure an error message can be retrieved via getSendError.
		 */
		virtual bool sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult) = 0;

		/*!
		 * Sends a redirect to the caller.
		 *
		 * \return true, if sending succeeded, false otherwise. On failure an error message can be retrieved via getSendError.
		 */
		virtual bool sendRedirect(const QUrl& pRedirectAddress, const Result& pResult) = 0;

		/*!
		 * Returns the last error that occurred during a send operation.
		 */
		const QString& getSendError() const
		{
			return mSendError;
		}


};

} /* namespace governikus */
