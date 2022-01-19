/*!
 * \brief Generate information for DIDListResponse.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"
#include "PaosCreator.h"

namespace governikus
{

class DIDListResponse
	: public PaosCreator
	, public ResponseType
{
	private:
		void createBodyElement() override;

		Q_DISABLE_COPY(DIDListResponse)

	public:
		DIDListResponse();
};

} // namespace governikus
