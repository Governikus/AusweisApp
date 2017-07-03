/*!
 * CompositeStateSelectCard.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "CompositeStateSelectCard.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateConnectCard.h"
#include "states/StateSelectBluetoothReader.h"
#include "states/StateSelectNfcReader.h"
#include "states/StateSelectPcscReader.h"
#include "states/StateSelectReaderType.h"


using namespace governikus;


CompositeStateSelectCard::CompositeStateSelectCard(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto selectReaderType = StateBuilder::createState<StateSelectReaderType>(mContext);
	auto selectBluetoothReader = StateBuilder::createState<StateSelectBluetoothReader>(mContext);
	auto selectNfcReader = StateBuilder::createState<StateSelectNfcReader>(mContext);
	auto selectPcscReader = StateBuilder::createState<StateSelectPcscReader>(mContext);
	auto connectCard = StateBuilder::createState<StateConnectCard>(mContext);

	selectReaderType->setParent(this);
	selectBluetoothReader->setParent(this);
	selectNfcReader->setParent(this);
	selectPcscReader->setParent(this);
	connectCard->setParent(this);

	setInitialState(selectReaderType);

	selectReaderType->addTransition(selectReaderType, &StateSelectReaderType::fireSelectBluetoothReader, selectBluetoothReader);
	selectReaderType->addTransition(selectReaderType, &StateSelectReaderType::fireSelectNfcReader, selectNfcReader);
	selectReaderType->addTransition(selectReaderType, &StateSelectReaderType::fireSelectPcscReader, selectPcscReader);
	connect(selectReaderType, &AbstractState::fireError, this, &CompositeStateSelectCard::fireError);
	connect(selectReaderType, &AbstractState::fireCancel, this, &CompositeStateSelectCard::fireCancel);

	selectBluetoothReader->addTransition(selectBluetoothReader, &AbstractState::fireSuccess, connectCard);
	connect(selectBluetoothReader, &AbstractState::fireError, this, &CompositeStateSelectCard::fireError);
	connect(selectBluetoothReader, &AbstractState::fireCancel, this, &CompositeStateSelectCard::fireCancel);
	selectBluetoothReader->addTransition(selectBluetoothReader, &StateSelectBluetoothReader::fireAbort, selectReaderType);

	selectNfcReader->addTransition(selectNfcReader, &AbstractState::fireSuccess, connectCard);
	connect(selectNfcReader, &AbstractState::fireError, this, &CompositeStateSelectCard::fireError);
	connect(selectNfcReader, &AbstractState::fireCancel, this, &CompositeStateSelectCard::fireCancel);
	selectNfcReader->addTransition(selectNfcReader, &StateSelectNfcReader::fireAbort, selectReaderType);

	selectPcscReader->addTransition(selectPcscReader, &AbstractState::fireSuccess, connectCard);
	connect(selectPcscReader, &AbstractState::fireError, this, &CompositeStateSelectCard::fireError);
	connect(selectPcscReader, &AbstractState::fireCancel, this, &CompositeStateSelectCard::fireCancel);
	selectPcscReader->addTransition(selectPcscReader, &StateSelectPcscReader::fireAbort, selectReaderType);

	connect(connectCard, &AbstractState::fireSuccess, this, &CompositeStateSelectCard::fireSuccess);
	connect(connectCard, &AbstractState::fireError, this, &CompositeStateSelectCard::fireError);
	connect(connectCard, &AbstractState::fireCancel, this, &CompositeStateSelectCard::fireCancel);
	connectCard->addTransition(connectCard, &StateConnectCard::fireAbort, selectReaderType);
	connectCard->addTransition(connectCard, &StateConnectCard::fireReaderRemoved, selectReaderType);
}


CompositeStateSelectCard::~CompositeStateSelectCard()
{
}
