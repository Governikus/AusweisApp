/*!
 * \brief Handle MessageIDs from XML header to use it in next paos message.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

class MessageIdHandler
{
	private:
		QString mRemoteMsgId;

	public:
		QString createNewMsgId();
		const QString& getRemoteMsgId() const;
		void setRemoteMsgId(const QString& pNewRemoteMsgId);
};

} /* namespace governikus */
