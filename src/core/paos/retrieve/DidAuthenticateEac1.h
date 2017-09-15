/*!
 * \brief Class represents the retrieved PAOS EAC1InputType.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#pragma once

#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/CertificateDescription.h"
#include "asn1/Chat.h"
#include "asn1/CVCertificate.h"
#include "paos/element/ConnectionHandle.h"
#include "paos/element/Eac1InputType.h"
#include "paos/ElementDetector.h"
#include "paos/PaosMessage.h"

#include <QVector>

class test_StatePrepareChat;
class test_StatePreVerification;
class test_StateExtractCvcsFromEac1InputType;
class test_StateProcessCertificatesFromEac2;
class test_StateCertificateDescriptionCheck;

namespace governikus
{
class TestAuthContext;

class DIDAuthenticateEAC1
	: public PaosMessage
{
	friend class DidAuthenticateEac1Parser;
	friend class ::test_StatePrepareChat;
	friend class TestAuthContext;
	friend class ::test_StatePreVerification;
	friend class ::test_StateExtractCvcsFromEac1InputType;
	friend class ::test_StateProcessCertificatesFromEac2;
	friend class ::test_StateCertificateDescriptionCheck;

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

		QSharedPointer<AuthenticatedAuxiliaryData> getAuthenticatedAuxiliaryData() const;
		const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const;
		QSharedPointer<CertificateDescription> getCertificateDescription() const;
		const QByteArray& getCertificateDescriptionAsBinary() const;
		const ConnectionHandle& getConnectionHandle() const;
		const QVector<QSharedPointer<CVCertificate> >& getCvCertificates() const;
		const QString& getDidName() const;
		const QSharedPointer<CHAT>& getOptionalChat() const;
		const QSharedPointer<CHAT>& getRequiredChat() const;
		const QString& getTransactionInfo() const;
};

} /* namespace governikus */
