/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Generate information for InitializeFrameworkResponse.
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
