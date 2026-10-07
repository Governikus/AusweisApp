/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */


#pragma once

#include "DidAuthenticateMessage.h"
#include "SmartCardDefinitions.h"
#include "asn1/CVCertificate.h"
#include "asn1/Chat.h"
#include "paos/element/Eac1InputType.h"

#include <QList>


namespace governikus
{
class TestAuthContext;

class DIDAuthenticateEAC1
	: public DidAuthenticateMessage
{
	friend class DidAuthenticateEac1Parser;
	friend class TestAuthContext;

	private:
		Eac1InputType mEac1InputType;

		void setEac1InputType(const Eac1InputType& eac1InputType);

	public:
		DIDAuthenticateEAC1();
		~DIDAuthenticateEAC1() override;

		[[nodiscard]] const QSharedPointer<const AuthenticatedAuxiliaryData>& getAuthenticatedAuxiliaryData() const;
		[[nodiscard]] const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const;
		[[nodiscard]] const QSharedPointer<const CertificateDescription>& getCertificateDescription() const;
		[[nodiscard]] const QByteArray& getCertificateDescriptionAsBinary() const;
		[[nodiscard]] const QList<QSharedPointer<const CVCertificate>>& getCvCertificates() const;
		[[nodiscard]] QList<QSharedPointer<const CVCertificate>> getCvCertificates(const QList<AccessRole>& pAccessRoles) const;
		[[nodiscard]] const QSharedPointer<const CHAT>& getOptionalChat() const;
		[[nodiscard]] const QSharedPointer<const CHAT>& getRequiredChat() const;
		[[nodiscard]] const QString& getTransactionInfo() const;
		[[nodiscard]] const QList<AcceptedEidType>& getAcceptedEidTypes() const;
};

} // namespace governikus
