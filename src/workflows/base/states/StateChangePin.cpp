/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePin.h"

#include "ReaderManager.h"

#include <QMetaObject>


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateChangePin::StateChangePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateChangePin::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke set Eid PIN command";
	mConnections += cardConnection->callSetEidPinCommand(this, &StateChangePin::onSetEidPinDone, getContext()->getNewPin().toLatin1());
}


void StateChangePin::onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<SetEidPinCommand> command = pCommand.objectCast<SetEidPinCommand>();
	if (!command)
	{
		Q_ASSERT(false);
		qCDebug(statemachine) << "Expected a SetEidPinCommand as response!";
		updateStatus(GlobalStatus::Code::Card_Protocol_Error);
		Q_EMIT fireAbort();
		return;
	}

	const auto& context = getContext();
	const CardReturnCode returnCode = command->getReturnCode();
	context->setLastPaceResult(returnCode);
	switch (returnCode)
	{
		case CardReturnCode::OK:
			switch (command->getResponseApdu().getStatusCode())
			{
				case StatusCode::SUCCESS:
					if (context->isSmartCardUsed())
					{
						//: INFO ALL_PLATFORMS The Smart-eID PIN was changed successfully.
						context->setSuccessMessage(tr("You have successfully changed your Smart-eID PIN."));
					}
					else
					{
						//: INFO ALL_PLATFORMS The ID card PIN was changed successfully.
						context->setSuccessMessage(tr("You have successfully changed your ID card PIN."));
					}
					Q_EMIT fireContinue();
					return;

				case StatusCode::INPUT_TIMEOUT:
					updateStatus(GlobalStatus::Code::Card_Input_TimeOut);
					break;

				case StatusCode::INPUT_CANCELLED:
					updateStatus(GlobalStatus::Code::Card_Cancellation_By_User);
					break;

				case StatusCode::PASSWORDS_DIFFER:
					updateStatus(GlobalStatus::Code::Card_NewPin_Mismatch);
					break;

				case StatusCode::PASSWORD_OUTOF_RANGE:
					updateStatus(GlobalStatus::Code::Card_NewPin_Invalid_Length);
					break;

				default:
					updateStatus(GlobalStatus::Code::Card_Unexpected_Transmit_Status);
			}

			Q_EMIT fireAbort();
			return;

		case CardReturnCode::CANCELLATION_BY_USER:
		case CardReturnCode::NEW_PIN_MISMATCH:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort();
			break;

		default:
			Q_EMIT fireRetry();
			break;
	}
}
