/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AuthModel.h"

#include "ReaderManagerPlugInInfo.h"

using namespace governikus;

AuthModel::AuthModel()
	: WorkflowModel()
	, mContext()
	, mTransactionInfo()
{
}


void AuthModel::resetAuthContext(const QSharedPointer<AuthContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetWorkflowContext(pContext);

	if (mContext)
	{
		connect(mContext.data(), &AuthContext::fireShowChangePinViewChanged, this, &AuthModel::fireShowChangePinViewChanged);
		connect(mContext.data(), &AuthContext::fireDidAuthenticateEac1Changed, this, &AuthModel::onDidAuthenticateEac1Changed);
		connect(mContext.data(), &AuthContext::fireProgressChanged, this, &AuthModel::fireProgressChanged);
	}

	if (!mTransactionInfo.isEmpty())
	{
		mTransactionInfo.clear();

		Q_EMIT fireTransactionInfoChanged();
	}

	Q_EMIT fireShowChangePinViewChanged();
	Q_EMIT fireProgressChanged();
}


const QString& AuthModel::getTransactionInfo() const
{
	return mTransactionInfo;
}


int AuthModel::getProgressValue() const
{
	if (mContext)
	{
		return mContext->getProgressValue();
	}

	return 0;
}


QString AuthModel::getProgressMessage() const
{
	if (mContext)
	{
		return mContext->getProgressMessage();
	}

	return QString();
}


bool AuthModel::getShowChangePinView() const
{
	if (mContext)
	{
		return mContext->showChangePinView();
	}

	return false;
}


QString AuthModel::getErrorHeader() const
{
	if (!mContext || mContext->getTcTokenUrl().isEmpty())
	{
		return QString();
	}

	const auto& tcTokenUrl = mContext->getTcTokenUrl();
	return tcTokenUrl.scheme() + QStringLiteral("://") + tcTokenUrl.authority();
}


QString AuthModel::getErrorText() const
{
	if (!mContext)
	{
		return QString();
	}

	const auto& status = mContext->getStatus();
	const auto& externalInfo = status.getExternalInfo();
	const auto& errorDescription = status.toErrorDescription(false);
	if (externalInfo.isEmpty() || errorDescription == externalInfo)
	{
		return errorDescription;
	}

	return errorDescription + QStringLiteral("\n(%1)").arg(externalInfo);
}


QString AuthModel::getStatusCodeString() const
{
	return getEnumName(getStatusCode());
}


void AuthModel::cancelWorkflowToChangePin()
{
	if (mContext)
	{
		mContext->requestChangePinView();
		mContext->setStatus(GlobalStatus::Code::Workflow_Cancellation_By_User);
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void AuthModel::requestTransportPinChange()
{
	if (mContext)
	{
		mContext->setPin(QString(5, QLatin1Char(0)));
		continueWorkflow();
	}
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
