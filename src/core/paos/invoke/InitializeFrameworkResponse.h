/*!
 * \brief Generate information for InitializeFrameworkResponse.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

		void createVersionElement();
		void createInitializeFrameworkResponse();

		virtual void createBodyElement() override;

		Q_DISABLE_COPY(InitializeFrameworkResponse)

	public:
		InitializeFrameworkResponse();
};

} // namespace governikus
