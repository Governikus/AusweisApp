/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "ApplicationModel.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

using namespace governikus;

NumberModel::NumberModel()
	: QObject()
	, mContext()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &NumberModel::onReaderInfoChanged);
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
		connect(mContext.data(), &WorkflowContext::firePinChanged, this, &NumberModel::firePasswordTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireCanChanged, this, &NumberModel::firePasswordTypeChanged);
		connect(mContext.data(), &WorkflowContext::firePukChanged, this, &NumberModel::firePasswordTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireCanAllowedModeChanged, this, &NumberModel::fireCanAllowedModeChanged);
		connect(mContext.data(), &WorkflowContext::firePasswordTypeChanged, this, &NumberModel::firePasswordTypeChanged);

		const auto changePinContext = mContext.objectCast<ChangePinContext>();
		if (changePinContext)
		{
			connect(changePinContext.data(), &ChangePinContext::fireNewPinChanged, this, &NumberModel::fireNewPinChanged);
		}

		const auto remoteServiceContext = mContext.objectCast<IfdServiceContext>();
		if (remoteServiceContext)
		{
			connect(remoteServiceContext.data(), &IfdServiceContext::fireEstablishPaceChannelUpdated, this, &NumberModel::fireInputErrorChanged);
		}

		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireReaderInfoChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireInputErrorChanged);
		connect(mContext.data(), &WorkflowContext::firePaceResultUpdated, this, &NumberModel::fireInputErrorChanged);
	}

	Q_EMIT fireCanChanged();
	Q_EMIT firePinChanged();
	Q_EMIT fireNewPinChanged();
	Q_EMIT firePukChanged();
	Q_EMIT fireInputErrorChanged();
	Q_EMIT fireReaderInfoChanged();
	Q_EMIT fireCanAllowedModeChanged();
	Q_EMIT firePasswordTypeChanged();
}


PasswordType NumberModel::getPasswordType() const
{
	if (!mContext)
	{
		return PasswordType::PIN;
	}

	const auto& changePinContext = mContext.objectCast<ChangePinContext>();
	if (changePinContext && !changePinContext->getPin().isEmpty())
	{
		return mContext->isSmartCardUsed() ? PasswordType::NEW_SMART_PIN : PasswordType::NEW_PIN;
	}

	const auto& remoteServiceContext = mContext.objectCast<IfdServiceContext>();
	if (remoteServiceContext && remoteServiceContext->modifyPinRunning())
	{
		return PasswordType::NEW_PIN;
	}

#if __has_include("context/PersonalizationContext.h")
	const auto& smartContext = mContext.objectCast<PersonalizationContext>();
	if (smartContext && !smartContext->getSessionIdentifier().isNull())
	{
		return PasswordType::NEW_SMART_PIN;
	}
#endif

	switch (mContext->getEstablishPaceChannelType())
	{
		case PacePasswordId::UNKNOWN:
		case PacePasswordId::PACE_MRZ:
		case PacePasswordId::PACE_PIN:
		{
			if (mContext->isRequestTransportPin())
			{
				return PasswordType::TRANSPORT_PIN;
			}
			return mContext->isSmartCardUsed() ? PasswordType::SMART_PIN : PasswordType::PIN;
		}

		case PacePasswordId::PACE_CAN:
			return PasswordType::CAN;

		case PacePasswordId::PACE_PUK:
			return PasswordType::PUK;
	}

	Q_UNREACHABLE();
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

	const auto remoteServiceContext = mContext.objectCast<IfdServiceContext>();
	if (remoteServiceContext)
	{
		remoteServiceContext->setNewPin(pNewPin);
	}

#if __has_include("context/PersonalizationContext.h")
	const auto smartContext = mContext.objectCast<PersonalizationContext>();
	if (smartContext)
	{
		smartContext->setNewPin(pNewPin);
	}
#endif

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
	const bool isRequestTransportPin = mContext && mContext->isRequestTransportPin();
	const bool isSmartCard = mContext && mContext->isSmartCardUsed();

	switch (paceResult)
	{
		case CardReturnCode::OK:
			return QString();

		case CardReturnCode::INVALID_PIN:
			if (isRequestTransportPin)
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN. "
							   "You have two further attempts to enter the correct Transport PIN."),
						//: INFO ALL_PLATFORMS
						tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
						   "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));
			}
			else if (isSmartCard)
			{
				//: INFO ALL_PLATFORMS The wrong Smart-eID PIN was entered on the first attempt.
				return tr("You have entered an incorrect, six-digit Smart-eID PIN. You have two further attempts to enter the correct Smart-eID PIN.");
			}
			else
			{
				//: INFO ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt.
				return tr("You have entered an incorrect, six-digit ID card PIN. You have two further attempts to enter the correct ID card PIN.");
			}

		case CardReturnCode::INVALID_PIN_2:
			if (isRequestTransportPin)
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN twice. "
							   "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
							   "You can find your CAN in the bottom right on the front of your ID card."),
						//: INFO ALL_PLATFORMS
						tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
						   "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));

			}
			else if (isSmartCard)
			{
				//: INFO ANDROID IOS The wrong Smart-eID PIN was entered twice, a third wrong attempt could invalidate the Smart-eID.
				return tr("You have entered an incorrect, six-digit Smart-eID PIN twice. "
						  "An incorrect third attempt will invalidate your Smart-eID and you will have to set it up again.");
			}
			else
			{
				//: INFO ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification.
				return tr("You have entered an incorrect, six-digit ID card PIN twice. "
						  "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
						  "You can find your CAN in the bottom right on the front of your ID card.");
			}

		case CardReturnCode::INVALID_PIN_3:
			if (isRequestTransportPin)
			{
				return QStringLiteral("%1<br><br>%2")
				       //: INFO ALL_PLATFORMS The Transport PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK.
					   .arg(tr("You have entered an incorrect, five-digit Transport PIN thrice, your Transport PIN is now blocked. "
							   "To remove the block, the ten-digit PUK must be entered first."),
						//: INFO ALL_PLATFORMS
						tr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
						   "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));
			}
			else if (isSmartCard)
			{
				//: INFO ANDROID IOS The Smart-eID PIN was entered wrongfully three times, the Smart-eID has been invalidated.
				return tr("You have entered an incorrect, six-digit Smart-eID PIN thrice. "
						  "Your Smart-eID is now invalidated. "
						  "To use a Smart-eID again you have to set one up in the guided setup on the start page.");
			}
			else
			{
				//: INFO ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK.
				return tr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. "
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
	Q_EMIT firePasswordTypeChanged();
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


void NumberModel::onReaderInfoChanged(const ReaderInfo& pInfo)
{
	if (mContext && pInfo.getName() == mContext->getReaderName())
	{
		Q_EMIT fireReaderInfoChanged();
	}
}
