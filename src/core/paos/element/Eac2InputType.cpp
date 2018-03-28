/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Eac2InputType.h"

using namespace governikus;

Eac2InputType::Eac2InputType()
{
}


Eac2InputType::~Eac2InputType()
{
}


const QString& Eac2InputType::getSignature() const
{
	return mSignature;
}


const QVector<QSharedPointer<const CVCertificate> >& Eac2InputType::getCvCertificates() const
{
	return mCvCertificates;
}


const QByteArrayList& Eac2InputType::getCvCertificatesAsBinary() const
{
	return mCvCertificatesAsBinary;
}


const QString& Eac2InputType::getEphemeralPublicKey() const
{
	return mEphemeralPublicKey;
}


void Eac2InputType::setEphemeralPublicKey(const QString& pEphemeralPublicKey)
{
	mEphemeralPublicKey = pEphemeralPublicKey;
}


void Eac2InputType::setSignature(const QString& pSignature)
{
	mSignature = pSignature;
}


void Eac2InputType::appendCvcert(const QSharedPointer<const CVCertificate>& pCert)
{
	mCvCertificates += pCert;
}


void Eac2InputType::appendCvcertAsBinary(const QByteArray& pCvcertAsBinary)
{
	mCvCertificatesAsBinary += pCvcertAsBinary;
}
