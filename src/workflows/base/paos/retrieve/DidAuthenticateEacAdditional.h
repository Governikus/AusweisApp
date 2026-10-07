/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "DidAuthenticateMessage.h"

#include <QString>

namespace governikus
{

class DIDAuthenticateEACAdditional
	: public DidAuthenticateMessage
{
	friend class DidAuthenticateEacAdditionalParser;

	private:
		QString mSignature;

		void setSignature(const QString& signature);

	public:
		DIDAuthenticateEACAdditional();
		~DIDAuthenticateEACAdditional() override;

		[[nodiscard]] const QString& getSignature() const;
};

} // namespace governikus
