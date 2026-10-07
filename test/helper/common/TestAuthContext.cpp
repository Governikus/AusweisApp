/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "TestAuthContext.h"

#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"


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
		const auto& parserEac1 = TestParserHelper::create(pFileName);
		auto* pm = PaosParser().parse(parserEac1).release();
		const QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(pm));

		setDidAuthenticateEac1(eac1);
		setDvCvc(getDidAuthenticateEac1()->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od}).at(0));
		initAccessRightManager(getDidAuthenticateEac1()->getCvCertificates({AccessRole::AT}).at(0));


		const auto& parserEac2 = TestParserHelper::create(":/paos/DIDAuthenticateEAC2.xml"_L1);
		pm = PaosParser().parse(parserEac2).release();
		const QSharedPointer<DIDAuthenticateEAC2> eac2(static_cast<DIDAuthenticateEAC2*>(pm));
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
