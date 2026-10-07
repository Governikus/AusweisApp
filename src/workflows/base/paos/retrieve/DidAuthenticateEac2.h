/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once


#include "DidAuthenticateMessage.h"

#include "paos/element/Eac2InputType.h"


namespace governikus
{

class DIDAuthenticateEAC2
	: public DidAuthenticateMessage
{
	friend class DidAuthenticateEac2Parser;
	friend class ::test_StateProcessCertificatesFromEac2;

	private:
		Eac2InputType mEac2;

		void setEac2InputType(const Eac2InputType& pEac2);

	public:
		DIDAuthenticateEAC2();
		~DIDAuthenticateEAC2() override;

		[[nodiscard]] const QString& getSignature() const;
		[[nodiscard]] const QString& getEphemeralPublicKey() const;
		[[nodiscard]] const QList<QSharedPointer<const CVCertificate>>& getCvCertificates() const;
};

} // namespace governikus
