/*!
 * \brief Class represents the retrieved PAOS EAC2InputType.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/ConnectionHandle.h"
#include "paos/element/Eac2InputType.h"
#include "paos/PaosMessage.h"

class test_StateProcessCertificatesFromEac2;

namespace governikus
{

class DIDAuthenticateEAC2
	: public PaosMessage
{
	friend class DidAuthenticateEac2Parser;
	friend class ::test_StateProcessCertificatesFromEac2;

	private:
		ConnectionHandle mConnectionHandle;
		QString mDidName;
		Eac2InputType mEac2;

		void setConnectionHandle(const ConnectionHandle& connectionHandle);
		void setDidName(const QString& didName);
		void setEac2InputType(const Eac2InputType& pEac2);

	public:
		DIDAuthenticateEAC2();
		virtual ~DIDAuthenticateEAC2();

		const ConnectionHandle& getConnectionHandle() const;
		const QString& getDidName() const;
		const QString& getSignature() const;
		const QString& getEphemeralPublicKey() const;
		const QVector<QSharedPointer<const CVCertificate> >& getCvCertificates() const;
		const QByteArrayList& getCvCertificatesAsBinary() const;
};

}
