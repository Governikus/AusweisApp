/*!
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <http_parser.h>

namespace governikus
{

class ActivationContext
	: public QObject
{
	Q_OBJECT

	private:
		QString mSendError;

	protected:
		void setSendError(const QString& pError);

	public:
		ActivationContext();
		~ActivationContext() override = default;

		[[nodiscard]] virtual QUrl getActivationURL() const = 0;

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
		virtual bool sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus) = 0;

		/*!
		 * Sends a redirect to the caller.
		 *
		 * \return true, if sending succeeded, false otherwise. On failure an error message can be retrieved via getSendError.
		 */
		virtual bool sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus) = 0;

		/*!
		 * Returns the last error that occurred during a send operation.
		 */
		[[nodiscard]] const QString& getSendError() const
		{
			return mSendError;
		}


};

} // namespace governikus
