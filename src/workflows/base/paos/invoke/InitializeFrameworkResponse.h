/*!
 * \brief Generate information for InitializeFrameworkResponse.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"
#include "paos/element/SupportedApi.h"

namespace governikus
{

class InitializeFrameworkResponse
	: public PaosCreator
	, public ResponseType
{
	Q_DISABLE_COPY(InitializeFrameworkResponse)

	private:
		SupportedAPI mSupportedAPI;

		void createVersionElement();
		void createInitializeFrameworkResponse();

		void createBodyElement() override;

	public:
		InitializeFrameworkResponse();
};

} // namespace governikus
