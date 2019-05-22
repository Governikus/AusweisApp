/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"


using namespace governikus;


TestAuthContext::TestAuthContext(ActivationContext* pActivationContext, const QString& pFileName)
	: AuthContext(QSharedPointer<ActivationContext>(pActivationContext))
	, mDidAuthenticateEac1()
{
	mDidAuthenticateEac1.reset(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(pFileName))));
	setDidAuthenticateEac1(mDidAuthenticateEac1);
	setTerminalCvc(mDidAuthenticateEac1->getCvCertificates().at(0));
	setDvCvc(mDidAuthenticateEac1->getCvCertificates().at(1));
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
