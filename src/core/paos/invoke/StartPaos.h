/*!
 * \brief Generate information for StartPaos.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/ConnectionHandle.h"
#include "paos/element/SupportedApi.h"
#include "paos/element/UserAgent.h"
#include "paos/invoke/PaosCreator.h"
#include "paos/PaosMessage.h"

class test_StartPaos;

namespace governikus
{

class StartPaos
	: public PaosCreator
	, public PaosMessage
{
	friend class ::test_StartPaos;

	private:
		const QByteArray mSessionId;
		const UserAgent mUserAgent;
		const SupportedAPI mSupportedAPI;

		void createSessionIdentifierElement();
		void createConnectionHandleElement();
		void createUserAgentElement();
		void createSupportedAPIVersionsElement();

		virtual void createBodyElement() override;

		Q_DISABLE_COPY(StartPaos)

	public:
		explicit StartPaos(const QByteArray& pSessionId);
};

} // namespace governikus
