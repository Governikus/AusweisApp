/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AuthModel.h"

#include "Env.h"
#include "ReaderManagerPlugInInfo.h"
#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(AuthModel)


AuthModel::AuthModel()
	: WorkflowModel()
	, mContext()
	, mTransactionInfo()
{
}


void AuthModel::resetContext(const QSharedPointer<AuthContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetContext(pContext);

	if (mContext)
	{
		connect(mContext.data(), &AuthContext::fireDidAuthenticateEac1Changed, this, &AuthModel::onDidAuthenticateEac1Changed);
	}

	if (!mTransactionInfo.isEmpty())
	{
		mTransactionInfo.clear();

		Q_EMIT fireTransactionInfoChanged();
	}
}


const QString& AuthModel::getTransactionInfo() const
{
	return mTransactionInfo;
}


AuthModel& AuthModel::getInstance()
{
	return *Instance;
}


void AuthModel::onDidAuthenticateEac1Changed()
{
	if (mContext)
	{
		const QSharedPointer<DIDAuthenticateEAC1>& didAuthenticateEAC1 = mContext->getDidAuthenticateEac1();
		const QString newTransactionInfo = didAuthenticateEAC1.isNull() ? QString() : didAuthenticateEAC1->getTransactionInfo();
		if (newTransactionInfo != mTransactionInfo)
		{
			mTransactionInfo = newTransactionInfo;

			Q_EMIT fireTransactionInfoChanged();
		}
	}
}


void AuthModel::setSkipRedirect(bool pSkipRedirect)
{
	mContext->setSkipRedirect(pSkipRedirect);
}
