/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Store information of Eac2InputType.
 */

#pragma once

#include "asn1/CVCertificate.h"

#include <QByteArrayList>
#include <QString>

class test_StateProcessCertificatesFromEac2;

namespace governikus
{

class Eac2InputType
{
	friend class DidAuthenticateEac2Parser;
	friend class ::test_StateProcessCertificatesFromEac2;

	private:
		QList<QSharedPointer<const CVCertificate>> mCvCertificates;
		QString mSignature;
		QString mEphemeralPublicKey;
		void appendCvcert(const QSharedPointer<const CVCertificate>& pCert);
		void setEphemeralPublicKey(const QString& pEphemeralPublicKey);
		void setSignature(const QString& pSignature);

	public:
		Eac2InputType() = default;
		virtual ~Eac2InputType();

		[[nodiscard]] const QString& getSignature() const;
		[[nodiscard]] const QString& getEphemeralPublicKey() const;
		[[nodiscard]] const QList<QSharedPointer<const CVCertificate>>& getCvCertificates() const;
};

} // namespace governikus
