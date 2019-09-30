/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */
#include "TestAuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"

#include "TestFileHelper.h"


using namespace governikus;


TestAuthContext::TestAuthContext(ActivationContext* pActivationContext, const QString& pFileName)
	: AuthContext(QSharedPointer<ActivationContext>(pActivationContext))
	, mDidAuthenticateEac1()
	, mDidAuthenticateEac2()
{
	mDidAuthenticateEac1.reset(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(pFileName))));
	setDidAuthenticateEac1(mDidAuthenticateEac1);
	setTerminalCvc(mDidAuthenticateEac1->getCvCertificates().at(0));
	setDvCvc(mDidAuthenticateEac1->getCvCertificates().at(1));

	mDidAuthenticateEac2.reset(dynamic_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"))));
	setDidAuthenticateEac2(mDidAuthenticateEac2);
}


TestAuthContext::~TestAuthContext()
{
}


void TestAuthContext::setRequiredAccessRights(const QSet<AccessRight>& pAccessRights)
{
	if (pAccessRights.isEmpty())
	{
		mDidAuthenticateEac1->mEac1InputType.mRequiredChat.reset();
	}
	else
	{
		if (!mDidAuthenticateEac1->getRequiredChat())
		{
			mDidAuthenticateEac1->mEac1InputType.mRequiredChat.reset(new CHAT(getTerminalCvc()->getBody().getCHAT()));
		}
		qSharedPointerConstCast<CHAT>(mDidAuthenticateEac1->getRequiredChat())->removeAllAccessRights();
		qSharedPointerConstCast<CHAT>(mDidAuthenticateEac1->getRequiredChat())->setAccessRights(pAccessRights);
	}
	setDidAuthenticateEac1(mDidAuthenticateEac1);
	setTerminalCvc(mDidAuthenticateEac1->getCvCertificates().at(0));
}


void TestAuthContext::setOptionalAccessRights(const QSet<AccessRight>& pAccessRights)
{
	if (pAccessRights.isEmpty())
	{
		mDidAuthenticateEac1->mEac1InputType.mOptionalChat.reset();
	}
	else
	{
		if (!mDidAuthenticateEac1->getOptionalChat())
		{
			mDidAuthenticateEac1->mEac1InputType.mOptionalChat.reset(new CHAT(getTerminalCvc()->getBody().getCHAT()));
		}
		qSharedPointerConstCast<CHAT>(mDidAuthenticateEac1->getOptionalChat())->removeAllAccessRights();
		qSharedPointerConstCast<CHAT>(mDidAuthenticateEac1->getOptionalChat())->setAccessRights(pAccessRights);
	}
	setDidAuthenticateEac1(mDidAuthenticateEac1);
	setTerminalCvc(mDidAuthenticateEac1->getCvCertificates().at(0));
}


void TestAuthContext::addCvCertificate(const QSharedPointer<const CVCertificate>& pCvCertificate)
{
	mDidAuthenticateEac1->mEac1InputType.mCvCertificates += pCvCertificate;
}


void TestAuthContext::clearCvCertificates()
{
	mDidAuthenticateEac1->mEac1InputType.mCvCertificates.clear();
}


void TestAuthContext::removeCvCertAt(int pPosition)
{
	mDidAuthenticateEac1->mEac1InputType.mCvCertificates.removeAt(pPosition);
}
