/*!
 * \brief Class represents the retrieved PAOS EAC2InputType.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/ConnectionHandle.h"
#include "paos/element/Eac2InputType.h"
#include "paos/PaosMessage.h"


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
		~DIDAuthenticateEAC2() override;

		[[nodiscard]] const ConnectionHandle& getConnectionHandle() const;
		[[nodiscard]] const QString& getDidName() const;
		[[nodiscard]] const QString& getSignature() const;
		[[nodiscard]] const QString& getEphemeralPublicKey() const;
		[[nodiscard]] const QVector<QSharedPointer<const CVCertificate>>& getCvCertificates() const;
		[[nodiscard]] const QByteArrayList& getCvCertificatesAsBinary() const;
};

} // namespace governikus
