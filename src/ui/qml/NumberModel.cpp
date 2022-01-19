/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "ApplicationModel.h"
#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"
#include "ReaderManager.h"

using namespace governikus;

NumberModel::NumberModel()
	: QObject()
	, mContext()
	, mRequestNewPin(false)
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardRetryCounterChanged, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardInserted, this, &NumberModel::onReaderInfoChanged);
}


void NumberModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireCanChanged, this, &NumberModel::fireCanChanged);
		connect(mContext.data(), &WorkflowContext::firePinChanged, this, &NumberModel::firePinChanged);
		connect(mContext.data(), &WorkflowContext::firePukChanged, this, &NumberModel::firePukChanged);
		connect(mContext.data(), &WorkflowContext::fireCanAllowedModeChanged, this, &NumberModel::fireCanAllowedModeChanged);
		connect(mContext.data(), &WorkflowContext::firePasswordTypeChanged, this, &NumberModel::firePasswordTypeChanged);

		const auto changePinContext = mContext.objectCast<ChangePinContext>();
		if (changePinContext)
		{
			connect(changePinContext.data(), &ChangePinContext::fireNewPinChanged, this, &NumberModel::fireNewPinChanged);
		}

		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireReaderInfoChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireInputErrorChanged);
		connect(mContext.data(), &WorkflowContext::firePaceResultUpdated, this, &NumberModel::fireInputErrorChanged);
	}
	mRequestNewPin = false;

	Q_EMIT fireCanChanged();
	Q_EMIT firePinChanged();
	Q_EMIT fireNewPinChanged();
	Q_EMIT firePukChanged();
	Q_EMIT fireInputErrorChanged();
	Q_EMIT fireReaderInfoChanged();
	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT fireRequestTransportPinChanged();
	Q_EMIT firePasswordTypeChanged();
}


NumberModel::QmlPasswordType NumberModel::getPasswordType() const
{
	if (!mContext)
	{
		return QmlPasswordType::PASSWORD_PIN;
	}

	if (mRequestNewPin)
	{
		return QmlPasswordType::PASSWORD_NEW_PIN;
	}

	switch (mContext->getEstablishPaceChannelType())
	{
		case PacePasswordId::UNKNOWN:
		case PacePasswordId::PACE_MRZ:
		case PacePasswordId::PACE_PIN:
			return QmlPasswordType::PASSWORD_PIN;

		case PacePasswordId::PACE_CAN:
			return QmlPasswordType::PASSWORD_CAN;

		case PacePasswordId::PACE_PUK:
			return QmlPasswordType::PASSWORD_PUK;
	}

	Q_UNREACHABLE();
	return QmlPasswordType::PASSWORD_PIN;
}


void NumberModel::requestNewPin()
{
	if (!mRequestNewPin)
	{
		mRequestNewPin = true;
		Q_EMIT firePasswordTypeChanged();
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

	mRequestNewPin = false;
	Q_EMIT firePasswordTypeChanged();
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


bool NumberModel::hasError() const
{
	return getInputErrorCode() != CardReturnCode::OK || Env::getSingleton<ApplicationModel>()->isExtendedLengthApdusUnsupported();
}


bool NumberModel::hasPasswordError() const
{
	return CardReturnCodeUtil::equalsWrongPacePassword(getInputErrorCode());
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
			if (isRequestTransportPin())
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN. "
							   "You have two further attempts to enter the correct Transport PIN."))
				       //: INFO ALL_PLATFORMS
					   .arg(tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
							   "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));
			}
			else
			{
				//: INFO ALL_PLATFORMS The wrong PIN was entered on the first attempt.
				return tr("You have entered an incorrect, six-digit PIN. You have two further attempts to enter the correct PIN.");
			}

		case CardReturnCode::INVALID_PIN_2:
			if (isRequestTransportPin())
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN twice. "
							   "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
							   "You can find your CAN in the bottom right on the front of your ID card."))
				       //: INFO ALL_PLATFORMS
					   .arg(tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
							   "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));

			}
			else
			{
				//: INFO ALL_PLATFORMS The wrong PIN was entered twice, the next attempt requires the CAN for additional verification.
				return tr("You have entered an incorrect, six-digit PIN twice. "
						  "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
						  "You can find your CAN in the bottom right on the front of your ID card.");
			}

		case CardReturnCode::INVALID_PIN_3:
			if (isRequestTransportPin())
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The Transport PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN thrice, your Transport PIN is now blocked. "
							   "To remove the block, the ten-digit PUK must be entered first."))
				       //: INFO ALL_PLATFORMS
					   .arg(tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
							   "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));
			}
			else
			{
				//: INFO ALL_PLATFORMS The PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK.
				return tr("You have entered an incorrect, six-digit PIN thrice, your PIN is now blocked. "
						  "To remove the block, the ten-digit PUK must be entered first.");
			}

		case CardReturnCode::INVALID_CAN:
			//: INFO ALL_PLATFORMS The CAN was entered wrongfully and needs to be supplied again.
			return tr("You have entered an incorrect, six-digit Card Access Number (CAN). Please try again.");

		case CardReturnCode::INVALID_PUK:
			//: INFO ALL_PLATFORMS The PUK entered wrongfully and needs to be supplied again.
			return tr("You have entered an incorrect, ten-digit PUK. Please try again.");

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


bool NumberModel::isCanAllowedMode() const
{
	if (mContext)
	{
		return mContext->isCanAllowedMode();
	}
	return false;
}


bool NumberModel::isRequestTransportPin() const
{
	const QSharedPointer<ChangePinContext> pinContext = mContext.objectCast<ChangePinContext>();
	if (pinContext)
	{
		return pinContext->isRequestTransportPin();
	}

	return false;
}


void NumberModel::onReaderInfoChanged(const ReaderInfo& pInfo)
{
	if (mContext && pInfo.getName() == mContext->getReaderName())
	{
		Q_EMIT fireReaderInfoChanged();
	}
}
