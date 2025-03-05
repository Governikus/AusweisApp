/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "TestAuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"

#include "TestFileHelper.h"

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


TestAuthContext::TestAuthContext(const QString& pFileName)
	: AuthContext()
	, mAcceptedEidTypes({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED})
{
	if (pFileName.isEmpty())
	{
		mAccessRightManager.reset(new AccessRightManager(nullptr, nullptr, nullptr));
	}
	else
	{
		QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(pFileName))));
		setDidAuthenticateEac1(eac1);
		setDvCvc(eac1->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od}).at(0));
		initAccessRightManager(eac1->getCvCertificates({AccessRole::AT}).at(0));

		QSharedPointer<DIDAuthenticateEAC2> eac2(static_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"_L1))));
		setDidAuthenticateEac2(eac2);
	}
}


TestAuthContext::~TestAuthContext()
{
}


void TestAuthContext::setRequiredAccessRights(const QSet<AccessRight>& pAccessRights)
{
	if (!mDIDAuthenticateEAC1->getRequiredChat())
	{
		mDIDAuthenticateEAC1->mEac1InputType.mRequiredChat.reset(new CHAT(getAccessRightManager()->getTerminalCvc()->getBody().getCHAT()));
	}
	qSharedPointerConstCast<CHAT>(mDIDAuthenticateEAC1->getRequiredChat())->removeAllAccessRights();
	qSharedPointerConstCast<CHAT>(mDIDAuthenticateEAC1->getRequiredChat())->setAccessRights(pAccessRights);

	setDidAuthenticateEac1(this->mDIDAuthenticateEAC1);
	setDvCvc(mDIDAuthenticateEAC1->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od}).at(0));
	initAccessRightManager(mDIDAuthenticateEAC1->getCvCertificates({AccessRole::AT}).at(0));
}


void TestAuthContext::setOptionalAccessRights(const QSet<AccessRight>& pAccessRights)
{
	if (!mDIDAuthenticateEAC1->getOptionalChat())
	{
		mDIDAuthenticateEAC1->mEac1InputType.mOptionalChat.reset(new CHAT(getAccessRightManager()->getTerminalCvc()->getBody().getCHAT()));
	}
	qSharedPointerConstCast<CHAT>(mDIDAuthenticateEAC1->getOptionalChat())->removeAllAccessRights();
	qSharedPointerConstCast<CHAT>(mDIDAuthenticateEAC1->getOptionalChat())->setAccessRights(pAccessRights);

	setDidAuthenticateEac1(mDIDAuthenticateEAC1);
	setDvCvc(mDIDAuthenticateEAC1->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od}).at(0));
	initAccessRightManager(mDIDAuthenticateEAC1->getCvCertificates({AccessRole::AT}).at(0));
}


void TestAuthContext::addCvCertificate(const QSharedPointer<const CVCertificate>& pCvCertificate)
{
	mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates += pCvCertificate;
}


void TestAuthContext::clearCvCertificates()
{
	mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.clear();
}


void TestAuthContext::removeCvCertAt(int pPosition)
{
	mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.removeAt(pPosition);
}


QList<AcceptedEidType> TestAuthContext::getAcceptedEidTypes() const
{
	return mAcceptedEidTypes;
}


void TestAuthContext::setAcceptedEidTypes(const QList<AcceptedEidType>& pAcceptedEidTypes)
{
	mAcceptedEidTypes = pAcceptedEidTypes;
}
