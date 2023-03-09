/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class to hold information of DIDAuthenticateEACAdditional.
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ConnectionHandle.h"

#include <QString>

namespace governikus
{

class DIDAuthenticateEACAdditional
	: public PaosMessage
{
	friend class DidAuthenticateEacAdditionalParser;

	private:
		ConnectionHandle mConnectionHandle;
		QString mDidName;
		QString mSignature;

		void setConnectionHandle(const ConnectionHandle& pConnectionHandle);
		void setDidName(const QString& didName);
		void setSignature(const QString& signature);

	public:
		DIDAuthenticateEACAdditional();
		~DIDAuthenticateEACAdditional() override;

		[[nodiscard]] const QString& getDidName() const;
		[[nodiscard]] const QString& getSignature() const;
};

} // namespace governikus
