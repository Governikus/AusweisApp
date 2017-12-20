/*!
 * \brief Generate information for DIDListResponse.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
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
		QDomElement createDidListResponseElement();
		QDomElement createDidListElement();

		virtual QDomElement getDocumentStructure() override;

		Q_DISABLE_COPY(DIDListResponse)

	public:
		DIDListResponse();
};

} /* namespace governikus */
