/*!
 * \brief Store information of Eac2InputType.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
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
		QByteArrayList mCvCertificatesAsBinary;
		QVector<QSharedPointer<const CVCertificate> > mCvCertificates;
		QString mSignature;
		QString mEphemeralPublicKey;
		void appendCvcert(const QSharedPointer<const CVCertificate>& pCert);
		void appendCvcertAsBinary(const QByteArray& pCvcertAsBinary);
		void setEphemeralPublicKey(const QString& pEphemeralPublicKey);
		void setSignature(const QString& pSignature);

	public:
		Eac2InputType();
		virtual ~Eac2InputType();

		const QString& getSignature() const;
		const QString& getEphemeralPublicKey() const;
		const QVector<QSharedPointer<const CVCertificate> >& getCvCertificates() const;
		const QByteArrayList& getCvCertificatesAsBinary() const;
};

} /* namespace governikus */
