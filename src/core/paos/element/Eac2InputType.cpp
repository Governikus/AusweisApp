/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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


const QVector<QSharedPointer<CVCertificate> >& Eac2InputType::getCvCertificates() const
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


void Eac2InputType::setEphemeralPublicKey(const QString& ephemeralPublicKey)
{
	mEphemeralPublicKey = ephemeralPublicKey;
}


void Eac2InputType::setSignature(const QString& signature)
{
	mSignature = signature;
}


void Eac2InputType::appendCvcert(QSharedPointer<CVCertificate> cvcert)
{
	mCvCertificates += cvcert;
}


void Eac2InputType::appendCvcertAsBinary(const QByteArray& cvcertAsBinary)
{
	mCvCertificatesAsBinary += cvcertAsBinary;
}
