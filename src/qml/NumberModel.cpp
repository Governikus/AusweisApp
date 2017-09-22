/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "context/ChangePinContext.h"
#include "context/WorkflowContext.h"
#include "NumberModel.h"
#include "ReturnCodeUtil.h"


using namespace governikus;


NumberModel::NumberModel(QObject* pParent)
	: QObject(pParent)
{
}


NumberModel::~NumberModel()
{
}


void NumberModel::resetContext(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireCanChanged, this, &NumberModel::fireCanChanged);
		connect(mContext.data(), &WorkflowContext::firePinChanged, this, &NumberModel::firePinChanged);

		const auto changePinContext = mContext.dynamicCast<ChangePinContext>();
		if (changePinContext)
		{
			connect(changePinContext.data(), &ChangePinContext::fireNewPinChanged, this, &NumberModel::fireNewPinChanged);
			connect(changePinContext.data(), &ChangePinContext::firePukChanged, this, &NumberModel::firePukChanged);
		}

		connect(mContext.data(), &ChangePinContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &ChangePinContext::fireReaderNameChanged, this, &NumberModel::fireRetryCounterChanged);
		connect(mContext.data(), &ChangePinContext::fireLastPaceResultChanged, this, &NumberModel::fireInputErrorChanged);
	}

	Q_EMIT fireCanChanged();
	Q_EMIT firePinChanged();
	Q_EMIT fireNewPinChanged();
	Q_EMIT firePukChanged();
	Q_EMIT fireInputErrorChanged();
	Q_EMIT fireRetryCounterChanged();
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
	const auto changePinContext = mContext.dynamicCast<ChangePinContext>();

	return changePinContext ? changePinContext->getNewPin() : QString();
}


void NumberModel::setNewPin(const QString& pNewPin)
{
	const auto changePinContext = mContext.dynamicCast<ChangePinContext>();
	if (changePinContext)
	{
		changePinContext->setNewPin(pNewPin);
	}
}


QString NumberModel::getPuk() const
{
	const auto changePinContext = mContext.dynamicCast<ChangePinContext>();

	return changePinContext ? changePinContext->getPuk() : QString();
}


void NumberModel::setPuk(const QString& pPuk)
{
	const auto changePinContext = mContext.dynamicCast<ChangePinContext>();
	if (changePinContext)
	{
		changePinContext->setPuk(pPuk);
	}
}


QString NumberModel::getInputError() const
{
	if (mContext.isNull() || mContext->getLastPaceResult() == ReturnCode::OK || mContext->getCardConnection().isNull())
	{
		return QString();
	}

	ReturnCode paceResult = mContext->getLastPaceResult();
	if (paceResult == ReturnCode::PIN_INVALID)
	{
		int oldRetryCounter = mContext->getOldRetryCounter();
		if (oldRetryCounter > 2)
		{
			return tr("You have entered a wrong PIN, please try again.");
		}
		if (oldRetryCounter == 2)
		{
			return tr("You have entered a wrong PIN two times. "
					  "You have one try left before the PIN is blocked. "
					  "You have to enter your CAN first.");
		}
		if (oldRetryCounter == 1)
		{
			return tr("You have entered a wrong PIN three times. "
					  "Your PIN is now blocked. "
					  "You have to enter your PUK now for unblocking.");
		}
	}
	if (paceResult == ReturnCode::CAN_INVALID)
	{
		return tr("You have entered a wrong CAN, please try again.");
	}
	if (paceResult == ReturnCode::PUK_INVALID)
	{
		return tr("You have entered a wrong PUK. "
				  "Please try again.");
	}
	return ReturnCodeUtil::toMessage(paceResult);
}


void NumberModel::onCardConnectionChanged()
{
	Q_ASSERT(mContext);
	if (auto cardConnection = mContext->getCardConnection())
	{
		connect(cardConnection.data(), &CardConnection::fireReaderInfoChanged, this, &NumberModel::fireRetryCounterChanged);
	}
	Q_EMIT fireRetryCounterChanged();
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
