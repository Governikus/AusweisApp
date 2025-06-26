/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

namespace
{
QString addBoldFormatting(const QString& inputString)
{
	return inputString.arg(QStringLiteral("<b>"), QStringLiteral("</b>"));
}


} // namespace

using namespace governikus;

NumberModel::NumberModel()
	: QObject()
	, mContext()
	, mNewPin()
	, mNewPinConfirmation()
{
	const auto* readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &NumberModel::onReaderInfoChanged);
	connect(readerManager, &ReaderManager::fireCardInserted, this, &NumberModel::onReaderInfoChanged);
}


void NumberModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	clearNewPinAndConfirmation();

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

#if __has_include("context/PersonalizationContext.h")
		const auto personalizationContext = mContext.objectCast<PersonalizationContext>();
		if (personalizationContext)
		{
			connect(personalizationContext.data(), &PersonalizationContext::fireSessionIdentifierChanged, this, &NumberModel::firePasswordTypeChanged);
		}
#endif

		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &NumberModel::onCardConnectionChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireReaderInfoChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &NumberModel::fireInputErrorChanged);
		connect(mContext.data(), &WorkflowContext::firePaceResultUpdated, this, &NumberModel::fireInputErrorChanged);
		connect(mContext.data(), &WorkflowContext::fireInitialInputErrorShownChanged, this, &NumberModel::firePasswordTypeChanged);
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


NumberModel::PasswordType NumberModel::getPasswordType() const
{
	if (!mContext)
	{
		return PasswordType::PIN;
	}

	const auto& changePinContext = mContext.objectCast<ChangePinContext>();
	const auto& remoteServiceContext = mContext.objectCast<IfdServiceContext>();
	if ((changePinContext && !changePinContext->getPin().isEmpty()) || (remoteServiceContext && remoteServiceContext->modifyPinRunning()))
	{
		if (mNewPin.isEmpty())
		{
			return mContext->isSmartCardUsed() ? PasswordType::NEW_SMART_PIN : PasswordType::NEW_PIN;
		}
		return mContext->isSmartCardUsed() ? PasswordType::NEW_SMART_PIN_CONFIRMATION : PasswordType::NEW_PIN_CONFIRMATION;
	}

#if __has_include("context/PersonalizationContext.h")
	const auto& smartContext = mContext.objectCast<PersonalizationContext>();
	if (smartContext && !smartContext->getSessionIdentifier().isNull())
	{
		if (mNewPin.isEmpty())
		{
			return PasswordType::NEW_SMART_PIN;
		}
		return PasswordType::NEW_SMART_PIN_CONFIRMATION;
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
	return mNewPin;
}


void NumberModel::setNewPin(const QString& pNewPin)
{
	if (!mContext)
	{
		return;
	}

	if (mNewPin != pNewPin)
	{
		mNewPin = pNewPin;
		Q_EMIT fireNewPinChanged();
	}

	Q_EMIT firePasswordTypeChanged();
	if (!mNewPinConfirmation.isEmpty())
	{
		mNewPinConfirmation.clear();
		Q_EMIT fireInputErrorChanged();
	}
}


QString NumberModel::getNewPinConfirmation() const
{
	return mNewPinConfirmation;
}


void NumberModel::setNewPinConfirmation(const QString& pNewPinConfirmation)
{
	if (mNewPinConfirmation != pNewPinConfirmation)
	{
		mNewPinConfirmation = pNewPinConfirmation;
		Q_EMIT fireNewPinConfirmationChanged();
		Q_EMIT firePasswordTypeChanged();
	}
}


bool NumberModel::commitNewPin()
{
	if (!newPinAndConfirmationMatch())
	{
		mNewPin.clear();
		Q_EMIT firePasswordTypeChanged();
		Q_EMIT fireInputErrorChanged();
		return false;
	}

	const auto changePinContext = mContext.objectCast<ChangePinContext>();
	if (changePinContext)
	{
		changePinContext->setNewPin(mNewPin);
	}

	const auto remoteServiceContext = mContext.objectCast<IfdServiceContext>();
	if (remoteServiceContext)
	{
		remoteServiceContext->setNewPin(mNewPin);
	}

#if __has_include("context/PersonalizationContext.h")
	const auto smartContext = mContext.objectCast<PersonalizationContext>();
	if (smartContext)
	{
		smartContext->setNewPin(mNewPin);
	}
#endif

	clearNewPinAndConfirmation();
	Q_EMIT firePasswordTypeChanged();

	return true;
}


void NumberModel::resetInputError()
{
	if (mContext)
	{
		mContext->resetLastPaceResult();
		Q_EMIT fireInputErrorChanged();
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


CardReturnCode NumberModel::getInputErrorCode() const
{
	if (mContext.isNull()
			|| mContext->getLastPaceResult() == CardReturnCode::OK
			|| mContext->getLastPaceResult() == CardReturnCode::OK_PUK
			|| mContext->getLastPaceResult() == CardReturnCode::OK_CAN
			|| mContext->getLastPaceResult() == CardReturnCode::CANCELLATION_BY_USER)
	{
		return CardReturnCode::OK;
	}

	return mContext->getLastPaceResult();
}


void NumberModel::clearNewPinAndConfirmation()
{
	mNewPin.clear();
	mNewPinConfirmation.clear();
}


bool NumberModel::newPinAndConfirmationMatch() const
{
	return !mNewPin.isEmpty() && !mNewPinConfirmation.isEmpty() && mNewPin == mNewPinConfirmation;
}


QString NumberModel::getInitialInputError() const
{
	if (mContext && !mContext->isInitialInputErrorShown())
	{

		if (getPasswordType() == PasswordType::CAN && !isCanAllowedMode())
		{
			return addBoldFormatting(QStringLiteral("%1<br/><br/>%2").arg(
					//: INFO ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 1/2
					tr("%1An incorrect PIN has been entered 2 times%2 at the last use of your ID card."),
					//: INFO ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 2/2
					tr("For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. You can find your CAN %1in the bottom right on the front of your ID card%2.")));
		}
		if (getPasswordType() == PasswordType::PUK)
		{
			return addBoldFormatting(QStringLiteral("%1<br/><br/>%2").arg(
					//: INFO ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 1/2
					tr("%1An incorrect PIN has been entered 3 times%2 at the last use of your ID card."),
					//: INFO ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 2/2
					tr("Therefore you have to enter the %1PUK%2 first to %1unlock the ID card PIN%2.")));
		}
	}
	return QString();
}


void NumberModel::setInitialInputErrorShown()
{
	if (mContext && !mContext->isInitialInputErrorShown())
	{
		mContext->setInitialInputErrorShown();
		Q_EMIT firePasswordTypeChanged();
	}
}


QString NumberModel::getInputError() const
{
	const CardReturnCode paceResult = getInputErrorCode();
	const bool isRequestTransportPin = mContext && mContext->isRequestTransportPin();
	const bool isSmartCard = mContext && mContext->isSmartCardUsed();

	if (!mNewPinConfirmation.isEmpty() && !newPinAndConfirmationMatch())
	{
#if __has_include("context/PersonalizationContext.h")
		return isSmartCard || (mContext && mContext.objectCast<PersonalizationContext>())
#else
		return isSmartCard
#endif
		       //: ALL_PLATFORMS Error message if the new pin confirmation mismatches.
				? tr("The input does not match. Please choose a new Smart-eID PIN.")
		       //: ALL_PLATFORMS Error message if the new pin confirmation mismatches.
				: tr("The input does not match. Please choose a new ID card PIN.");
	}

	switch (paceResult)
	{
		case CardReturnCode::OK:
			return QString();

		case CardReturnCode::INVALID_PIN:
			if (isRequestTransportPin)
			{
				return QStringLiteral("%1<br/><br/>%2").arg(
						//: INFO ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. Part 1/2
						tr("You have entered an incorrect, 5-digit Transport PIN."),
						//: INFO ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2
						addBoldFormatting(tr("You have%1 2 further attempts%2 to enter the correct Transport PIN. "
											 "The 5-digit Transport PIN may be found on the %1bottom left of your PIN letter%2.")));
			}
			else if (isSmartCard)
			{
				//: INFO ALL_PLATFORMS The wrong Smart-eID PIN was entered on the first attempt. %1 + %2 are used to emphasize.
				return(tr("You have entered an incorrect, 6-digit Smart-eID PIN. You have%1 2 further attempts%2 to enter the correct Smart-eID PIN."));
			}
			else
			{
				return QStringLiteral("%1<br/><br/>%2").arg(
						//: INFO ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. Part 1/2
						tr("You have entered an incorrect, 6-digit ID card PIN."),
						//: INFO ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2
						addBoldFormatting(tr("You have%1 2 further attempts%2 to enter the correct ID card PIN.")));
			}

		case CardReturnCode::INVALID_PIN_2:
			if (isRequestTransportPin)
			{
				return addBoldFormatting(QStringLiteral("%1<br/><br/>%2").arg(
						//: INFO ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2
						tr("You have entered an %1incorrect, 5-digit Transport PIN 2 times%2."),
						//: INFO ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2
						tr("For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. "
						   "You can find your CAN in the %1bottom right on the front of your ID card%2.")));

			}
			else if (isSmartCard)
			{
				//: INFO ANDROID IOS The wrong Smart-eID PIN was entered twice, a 3rd wrong attempt could invalidate the Smart-eID. %1 + %2 are used to emphasize.
				return addBoldFormatting(tr("You have entered an %1incorrect, 6-digit Smart-eID PIN 2 times%2. "
											"After the next failed attempt you will no longer be able to use your Smart-eID and will need to set it up again."));
			}
			else
			{
				return addBoldFormatting(QStringLiteral("%1<br/><br/>%2").arg(
						//: INFO ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2
						tr("You have entered an %1incorrect, 6-digit ID card PIN 2 times%2."),
						//: INFO ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2
						tr("For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. "
						   "You can find your CAN in the %1bottom right on the front of your ID card%2.")));
			}

		case CardReturnCode::INVALID_PIN_3:
			if (isRequestTransportPin)
			{
				//: INFO ALL_PLATFORMS The Transport PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. %1 + %2 are used to emphasize.
				return addBoldFormatting(tr("You have entered an incorrect, 5-digit Transport PIN 3 times, your %1Transport PIN is now blocked%2. "
											"To remove the block, the%1 10-digit PUK%2 must be entered first."));
			}
			else if (isSmartCard)
			{
				//: INFO ANDROID IOS The Smart-eID PIN was entered wrongfully three times, the Smart-eID has been invalidated.
				return tr("You have entered an incorrect, 6-digit Smart-eID PIN 3 times. "
						  "Your Smart-eID is now invalidated. "
						  "To use a Smart-eID again you have to set one up in the guided setup on the start page.");
			}
			else
			{
				return addBoldFormatting(QStringLiteral("%1<br/><br/>%2").arg(
						//: INFO ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 1/2
						tr("You have entered an incorrect, 6-digit ID card PIN 3 times. Your %1ID card PIN is now blocked%2."),
						//: INFO ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 2/2
						tr("To remove the block, the%1 10-digit PUK%2 must be entered first. "
						   "You can find the PUK in the bottom %1right next%2 to the Transport PIN in the %1authority's letter%2.")));
			}

		case CardReturnCode::INVALID_CAN:
			//: INFO ALL_PLATFORMS The CAN was entered wrongfully and needs to be supplied again. %1 + %2 are used to emphasize.
			return addBoldFormatting(tr("You have entered an %1incorrect Card Access Number (CAN)%2. Please try again. "
										"You can find your CAN in the %1bottom right on the front of your ID card%2."));

		case CardReturnCode::INVALID_PUK:
			//: INFO ALL_PLATFORMS The PUK entered wrongfully and needs to be supplied again.
			return tr("You have entered an incorrect, 10-digit PUK. Please try again.");

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
