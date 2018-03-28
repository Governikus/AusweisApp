/*!
 * \brief IfdStatus mock for tests
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "messages/IfdStatus.h"


namespace governikus
{

class MockIfdStatus
	: public IfdStatus
{
	public:
		MockIfdStatus(const QString& pSlotName,
				const PaceCapabilities& pPaceCapabilities,
				int pMaxApduLength,
				bool pConnected,
				bool pCardAvailable = false);
		~MockIfdStatus();

	private:
		QJsonObject createJsonObject(const QString& pSlotName,
				const PaceCapabilities& pPaceCapabilities,
				int pMaxApduLength,
				bool pConnectedReader,
				bool pCardAvailable);


};

} /* namespace governikus */
