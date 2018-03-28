/*!
 * \brief Classes that model remote card reader response messages.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessage.h"


namespace governikus
{
class RemoteMessageResponse
	: public RemoteMessage
{
	private:
		QString mResultMajor;
		QString mResultMinor;

	protected:
		virtual QJsonObject createMessageBody(const QString& pContextHandle) const override;

	public:
		RemoteMessageResponse(RemoteCardMessageType pType, const QString& pResultMinor = QString());
		RemoteMessageResponse(const QJsonObject& pMessageObject);
		virtual ~RemoteMessageResponse() override = default;

		bool resultHasError() const;
		const QString& getResultMinor() const;
};


} /* namespace governikus */
