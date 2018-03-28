/*!
 * \brief This state executes the remote message IfdModifyPin.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{


class StateChangePinRemote
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		QSharedPointer<const IfdModifyPin> mModifyPinMessage;
		QSharedPointer<ServerMessageHandler> mMessageHandler;

		StateChangePinRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onCancelChangePin();
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);
		void onChangePinDone(QSharedPointer<BaseCardCommand> pCommand);

	protected:
		void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateChangePinRemote() override;

		void onEntry(QEvent* pEvent) override;

};

} /* namespace governikus */
