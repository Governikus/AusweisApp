/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"
#include "context/WorkflowContext.h"
#include "ReaderManager.h"


using namespace governikus;


NumberModel::NumberModel(QObject* pParent)
	: QObject(pParent)
{
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderPropertiesUpdated, this, &NumberModel::onReaderInfoChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRetryCounterChanged, this, &NumberModel::onReaderInfoChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &NumberModel::onReaderInfoChanged);
}


NumberModel::~NumberModel()
{
}


void NumberModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireCanChanged, this, &NumberModel::fireCanChanged);
		connect(mContext.data(), &WorkflowContext::firePinChanged, this, &NumberModel::firePinChanged);

		const auto changePinContext = mContext.objectCast<ChangePinContext>();
		if (changePinContext)
		{
			connect(changePinContext.data(), &ChangePinContext::fireNewPinChanged, this, &NumberModel::fireNewPinChanged);
			connect(changePinContext.data(), &ChangePinContext::firePukChanged, this, &NumberModel::firePukChanged);
		}

		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireReaderInfoChanged);
		connect(mContext.data(), &WorkflowContext::fireLastPaceResultChanged, this, &NumberModel::fireInputErrorChanged);
	}

	Q_EMIT fireCanChanged();
	Q_EMIT firePinChanged();
	Q_EMIT fireNewPinChanged();
	Q_EMIT firePukChanged();
	Q_EMIT fireInputErrorChanged();
	Q_EMIT fireReaderInfoChanged();
}


void NumberModel::continueWorkflow()
{
	if (mContext)
	{
		mContext->setStateApproved();
	}
}


QString NumberModel::getCan() const
{
	return mContext ? mContext->getCan() : QString();
}


void NumberModel::setCan(const QString& pCan)
{
	if (mContext)
	{
		mContext->setCan(pCan);
	}
}


QString NumberModel::getPin() const
{
	return mContext ? mContext->getPin() : QString();
}


void NumberModel::setPin(const QString& pPin)
{
	if (mContext)
	{
		mContext->setPin(pPin);
	}
}


QString NumberModel::getNewPin() const
{
	const auto changePinContext = mContext.objectCast<ChangePinContext>();

	return changePinContext ? changePinContext->getNewPin() : QString();
}


void NumberModel::setNewPin(const QString& pNewPin)
{
	const auto changePinContext = mContext.objectCast<ChangePinContext>();
	if (changePinContext)
	{
		changePinContext->setNewPin(pNewPin);
	}

	const auto remoteServiceContext = mContext.objectCast<RemoteServiceContext>();
	if (remoteServiceContext)
	{
		remoteServiceContext->setNewPin(pNewPin);
	}
}


QString NumberModel::getPuk() const
{
	const auto changePinContext = mContext.objectCast<ChangePinContext>();
	return changePinContext ? changePinContext->getPuk() : QString();
}


void NumberModel::setPuk(const QString& pPuk)
{
	const auto changePinContext = mContext.objectCast<ChangePinContext>();
	if (changePinContext)
	{
		changePinContext->setPuk(pPuk);
	}
}


QString NumberModel::getInputError() const
{
	if (mContext.isNull()
			|| mContext->getLastPaceResult() == CardReturnCode::OK
			|| mContext->getLastPaceResult() == CardReturnCode::CANCELLATION_BY_USER
			|| mContext->getCardConnection().isNull())
	{
		return QString();
	}

	CardReturnCode paceResult = mContext->getLastPaceResult();
	if (paceResult == CardReturnCode::INVALID_PIN)
	{
		int oldRetryCounter = mContext->getOldRetryCounter();
		if (oldRetryCounter > 2)
		{
			return tr("The given PIN is not correct. You have 2 tries to enter the correct PIN.");
		}
		if (oldRetryCounter == 2)
		{
			return tr("You have entered the wrong PIN twice. "
					  "Prior to a third attempt, you have to enter your six-digit card access number first. "
					  "You can find your card access number on the front of your ID card.");
		}
		if (oldRetryCounter == 1)
		{
			return tr("You have entered a wrong PIN three times. "
					  "Your PIN is now blocked. "
					  "You have to enter the PUK now for unblocking.");
		}
	}
	if (paceResult == CardReturnCode::INVALID_CAN)
	{
		return tr("You have entered a wrong CAN, please try again.");
	}
	if (paceResult == CardReturnCode::INVALID_PUK)
	{
		return tr("You have entered a wrong PUK. "
				  "Please try again.");
	}
	return CardReturnCodeUtil::toGlobalStatus(paceResult).toErrorDescription(true);
}


void NumberModel::onCardConnectionChanged()
{
	Q_ASSERT(mContext);
	if (auto cardConnection = mContext->getCardConnection())
	{
		connect(cardConnection.data(), &CardConnection::fireReaderInfoChanged, this, &NumberModel::fireReaderInfoChanged);
	}
	Q_EMIT fireReaderInfoChanged();
}


int NumberModel::getRetryCounter() const
{
	if (mContext.isNull() || mContext->getCardConnection().isNull())
	{
		return -1;
	}
	else
	{
		return mContext->getCardConnection()->getReaderInfo().getRetryCounter();
	}
}


bool NumberModel::isExtendedLengthApdusUnsupported() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(mContext->getReaderName());
		return !readerInfo.sufficientApduLength();
	}
	return false;
}


bool NumberModel::isPinDeactivated() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		return ReaderManager::getInstance().getReaderInfo(mContext->getReaderName()).isPinDeactivated();
	}
	return false;
}


bool NumberModel::isCardConnected() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		return ReaderManager::getInstance().getReaderInfo(mContext->getReaderName()).hasCard();
	}
	return false;
}


void NumberModel::onReaderInfoChanged(const QString& pReaderName)
{
	if (mContext && pReaderName == mContext->getReaderName())
	{
		Q_EMIT fireReaderInfoChanged();
	}
}
