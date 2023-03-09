/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "Eac2InputType.h"

using namespace governikus;

Eac2InputType::~Eac2InputType() = default;


const QString& Eac2InputType::getSignature() const
{
	return mSignature;
}


const QVector<QSharedPointer<const CVCertificate>>& Eac2InputType::getCvCertificates() const
{
	return mCvCertificates;
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
