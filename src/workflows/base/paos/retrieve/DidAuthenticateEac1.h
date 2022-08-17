/*!
 * \brief Class represents the retrieved PAOS EAC1InputType.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/CVCertificate.h"
#include "asn1/Chat.h"
#include "paos/PaosMessage.h"
#include "paos/element/ConnectionHandle.h"
#include "paos/element/Eac1InputType.h"

#include <QVector>


namespace governikus
{
class TestAuthContext;

class DIDAuthenticateEAC1
	: public PaosMessage
{
	friend class DidAuthenticateEac1Parser;
	friend class TestAuthContext;

	private:
		ConnectionHandle mConnectionHandle;
		QString mDidName;
		Eac1InputType mEac1InputType;

	private:
		void setConnectionHandle(const ConnectionHandle& connectionHandle);
		void setDidName(const QString& didName);
		void setEac1InputType(const Eac1InputType& eac1InputType);

	public:
		DIDAuthenticateEAC1();
		~DIDAuthenticateEAC1() override;

		[[nodiscard]] const QSharedPointer<const AuthenticatedAuxiliaryData>& getAuthenticatedAuxiliaryData() const;
		[[nodiscard]] const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const;
		[[nodiscard]] const QSharedPointer<const CertificateDescription>& getCertificateDescription() const;
		[[nodiscard]] const QByteArray& getCertificateDescriptionAsBinary() const;
		[[nodiscard]] const ConnectionHandle& getConnectionHandle() const;
		[[nodiscard]] const QVector<QSharedPointer<const CVCertificate>>& getCvCertificates() const;
		[[nodiscard]] const QString& getDidName() const;
		[[nodiscard]] const QSharedPointer<const CHAT>& getOptionalChat() const;
		[[nodiscard]] const QSharedPointer<const CHAT>& getRequiredChat() const;
		[[nodiscard]] const QString& getTransactionInfo() const;
		[[nodiscard]] const QVector<AcceptedEidType>& getAcceptedEidTypes() const;
};

} // namespace governikus
