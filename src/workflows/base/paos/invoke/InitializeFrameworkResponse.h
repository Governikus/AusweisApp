/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
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

		void createBodyElement(QXmlStreamWriter& pWriter) override;
		void createVersionElement(QXmlStreamWriter& pWriter) const;

	public:
		InitializeFrameworkResponse();
};

} // namespace governikus
