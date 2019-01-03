/*!
 * \brief Class represents the retrieved PAOS EAC1InputType.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */


#pragma once

#include "asn1/Chat.h"
#include "asn1/CVCertificate.h"
#include "paos/element/ConnectionHandle.h"
#include "paos/element/Eac1InputType.h"
#include "paos/PaosMessage.h"

#include <QVector>


namespace governikus
{
class TestAuthContext;

class DIDAuthenticateEAC1
	: public PaosMessage
{
	friend class DidAuthenticateEac1Parser;
	friend class TestAuthContext;
	friend class ::test_StatePreVerification;
	friend class ::test_StateExtractCvcsFromEac1InputType;
	friend class ::test_StateProcessCertificatesFromEac2;
	friend class ::test_StateCertificateDescriptionCheck;
	friend class ::test_AuthModel;

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
		virtual ~DIDAuthenticateEAC1();

		const QSharedPointer<const AuthenticatedAuxiliaryData>& getAuthenticatedAuxiliaryData() const;
		const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const;
		const QSharedPointer<const CertificateDescription>& getCertificateDescription() const;
		const QByteArray& getCertificateDescriptionAsBinary() const;
		const ConnectionHandle& getConnectionHandle() const;
		const QVector<QSharedPointer<const CVCertificate> >& getCvCertificates() const;
		const QString& getDidName() const;
		const QSharedPointer<const CHAT>& getOptionalChat() const;
		const QSharedPointer<const CHAT>& getRequiredChat() const;
		const QString& getTransactionInfo() const;
};

} // namespace governikus
