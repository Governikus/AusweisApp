/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "ApplicationModel.h"
#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"
#include "ReaderManager.h"
#include "SingletonHelper.h"


using namespace governikus;


defineSingleton(NumberModel)


NumberModel::NumberModel()
	: QObject()
	, mContext()
	, mRequestTransportPin(false)
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardRetryCounterChanged, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardInserted, this, &NumberModel::onReaderInfoChanged);
}


NumberModel& NumberModel::getInstance()
{
	return *Instance;
}


void NumberModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireCanChanged, this, &NumberModel::fireCanChanged);
		connect(mContext.data(), &WorkflowContext::firePinChanged, this, &NumberModel::firePinChanged);
		connect(mContext.data(), &WorkflowContext::fireCanAllowedModeChanged, this, &NumberModel::fireCanAllowedModeChanged);

		const auto changePinContext = mContext.objectCast<ChangePinContext>();
		if (changePinContext)
		{
			connect(changePinContext.data(), &ChangePinContext::fireNewPinChanged, this, &NumberModel::fireNewPinChanged);
			connect(changePinContext.data(), &ChangePinContext::firePukChanged, this, &NumberModel::firePukChanged);
		}

		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireReaderInfoChanged);
		connect(mContext.data(), &WorkflowContext::fireLastPaceResultChanged, this, &NumberModel::fireInputErrorChanged);

		// The length of the pin doesn't matter for the core. Requesting
		// a 5 or 6 digits PIN is only part of the gui. Therefore we handle
		// this state in the NumberModel. The only case where the core need
		// to know if the transport PIN should be requested is, when we
		// are in an authentication workflow and want to start a pin change.
		// Therefore we enqueue a ChangePinContext with the transport PIN
		// request and consider it in this function.
		const QSharedPointer<ChangePinContext> context = mContext.objectCast<ChangePinContext>();
		mRequestTransportPin |= (context && context->requestTransportPin());
	}
	else
	{
		mRequestTransportPin = false;
	}

	Q_EMIT fireCanChanged();
	Q_EMIT firePinChanged();
	Q_EMIT fireNewPinChanged();
	Q_EMIT firePukChanged();
	Q_EMIT fireInputErrorChanged();
	Q_EMIT fireReaderInfoChanged();
	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT fireRequestTransportPinChanged();
}


PacePasswordId NumberModel::getEstablishPaceChannelType() const
{
	return mContext ? mContext->getEstablishPaceChannelType() : PacePasswordId::UNKNOWN;
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
	return mContext ? mContext->getPuk() : QString();
}


void NumberModel::setPuk(const QString& pPuk)
{
	if (mContext)
	{
		mContext->setPuk(pPuk);
	}
}


bool NumberModel::hasError()
{
	return getInputErrorCode() != CardReturnCode::OK || Env::getSingleton<ApplicationModel>()->isExtendedLengthApdusUnsupported() || isPinDeactivated();
}


CardReturnCode NumberModel::getInputErrorCode() const
{
	if (mContext.isNull()
			|| mContext->getLastPaceResult() == CardReturnCode::OK
			|| mContext->getLastPaceResult() == CardReturnCode::CANCELLATION_BY_USER
			|| mContext->getCardConnection().isNull())
	{
		return CardReturnCode::OK;
	}

	return mContext->getLastPaceResult();
}


QString NumberModel::getInputError() const
{
	const CardReturnCode paceResult = getInputErrorCode();
	switch (paceResult)
	{
		case CardReturnCode::OK:
			return QString();

		case CardReturnCode::INVALID_PIN:
			return tr("The given PIN is not correct. You have 2 tries to enter the correct PIN.");

		case CardReturnCode::INVALID_PIN_2:
			return tr("You have entered the wrong PIN twice. "
					  "Prior to a third attempt, you have to enter your six-digit card access number first. "
					  "You can find your card access number on the front of your ID card.");

		case CardReturnCode::INVALID_PIN_3:
			return tr("You have entered a wrong PIN three times. "
					  "Your PIN is now blocked. "
					  "You have to enter the PUK now for unblocking.");

		case CardReturnCode::INVALID_CAN:
			return tr("You have entered a wrong CAN, please try again.");

		case CardReturnCode::INVALID_PUK:
			return tr("You have entered a wrong PUK. "
					  "Please try again.");

		default:
			return CardReturnCodeUtil::toGlobalStatus(paceResult).toErrorDescription(true);
	}
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


bool NumberModel::isPinDeactivated() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		return Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName()).isPinDeactivated();
	}
	return false;
}


bool NumberModel::isCanAllowedMode()
{
	if (mContext)
	{
		return mContext->isCanAllowedMode();
	}
	return false;
}


void NumberModel::setRequestTransportPin(bool pEnabled)
{
	if (mRequestTransportPin != pEnabled)
	{
		mRequestTransportPin = pEnabled;
		Q_EMIT fireRequestTransportPinChanged();
	}
}


bool NumberModel::isRequestTransportPin() const
{
	return mRequestTransportPin;
}


void NumberModel::onReaderInfoChanged(const QString& pReaderName)
{
	if (mContext && pReaderName == mContext->getReaderName())
	{
		Q_EMIT fireReaderInfoChanged();
	}
}
