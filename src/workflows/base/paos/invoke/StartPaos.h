/*!
 * \brief Generate information for StartPaos.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ConnectionHandle.h"
#include "paos/invoke/PaosCreator.h"

class test_StartPaos;

namespace governikus
{

class StartPaos
	: public PaosCreator
	, public PaosMessage
{
	Q_DISABLE_COPY(StartPaos)
	friend class ::test_StartPaos;

	private:
		const QByteArray mSessionId;

		void createSessionIdentifierElement();
		void createConnectionHandleElement();
		void createUserAgentElement();
		void createSupportedAPIVersionsElement();

		void createBodyElement() override;

	public:
		explicit StartPaos(const QByteArray& pSessionId);
};

} // namespace governikus
