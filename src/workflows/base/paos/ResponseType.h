/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "ECardApiResult.h"
#include "PaosMessage.h"


namespace governikus
{

class ResponseType
	: public PaosMessage
{
	private:
		ECardApiResult mResult;

	public:
		explicit ResponseType(PaosType pType);
		~ResponseType() override;

		[[nodiscard]] const ECardApiResult& getResult() const;
		void setResult(const ECardApiResult& result);
};

} // namespace governikus
