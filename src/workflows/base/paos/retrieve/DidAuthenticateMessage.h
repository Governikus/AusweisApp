/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ConnectionHandle.h"

class test_DidAuthenticateMessage;

namespace governikus
{

class DidAuthenticateMessage
	: public PaosMessage
{
	friend class DidAuthenticateParser;
	friend class ::test_DidAuthenticateMessage;

	private:
		ConnectionHandle mConnectionHandle;
		QString mDidName;

		void setConnectionHandle(const ConnectionHandle& pConnectionHandle);
		void setDidName(const QString& pDidName);

	public:
		explicit DidAuthenticateMessage(PaosType pType);
		~DidAuthenticateMessage() override;

		[[nodiscard]] const ConnectionHandle& getConnectionHandle() const;
		[[nodiscard]] const QString& getDidName() const;

};

} // namespace governikus
