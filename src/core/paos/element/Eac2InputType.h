/*!
 * \brief Store information of Eac2InputType.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		QVector<QSharedPointer<CVCertificate> > mCvCertificates;
		QString mSignature;
		QString mEphemeralPublicKey;
		void appendCvcert(QSharedPointer<CVCertificate> cvcert);
		void appendCvcertAsBinary(const QByteArray& cvcertAsBinary);
		void setEphemeralPublicKey(const QString& ephemeralPublicKey);
		void setSignature(const QString& signature);

	public:
		Eac2InputType();
		virtual ~Eac2InputType();

		const QString& getSignature() const;
		const QString& getEphemeralPublicKey() const;
		const QVector<QSharedPointer<CVCertificate> >& getCvCertificates() const;
		const QByteArrayList& getCvCertificatesAsBinary() const;
};

} /* namespace governikus */
