/*!
 * \brief Generate information for InitializeFrameworkResponse.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "paos/element/SupportedApi.h"
#include "paos/ResponseType.h"
#include "PaosCreator.h"

namespace governikus
{

class InitializeFrameworkResponse
	: public PaosCreator
	, public ResponseType
{
	private:
		SupportedAPI mSupportedAPI;

		QDomElement createVersionElement();
		QDomElement createInitializeFrameworkResponse();

		virtual QDomElement getDocumentStructure();

		Q_DISABLE_COPY(InitializeFrameworkResponse)

	public:
		InitializeFrameworkResponse();
};

} /* namespace governikus */
