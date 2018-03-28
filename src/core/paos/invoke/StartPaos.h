/*!
 * \brief Generate information for StartPaos.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

		QDomElement createStartPaosElement();
		QDomElement createSessionIdentifierElement();
		QDomElement createConnectionHandleElement();
		QDomElement createUserAgentElement();
		QDomElement createSupportedAPIVersionsElement();

		virtual QDomElement getDocumentStructure() override;

		Q_DISABLE_COPY(StartPaos)

	public:
		StartPaos(const QByteArray& pSessionId);
};

} /* namespace governikus */
