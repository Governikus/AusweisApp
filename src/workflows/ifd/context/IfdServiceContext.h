/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Remote service context.
 */

#pragma once

#include "IfdServer.h"
#include "asn1/CertificateDescription.h"
#include "context/AccessRightManager.h"
#include "context/WorkflowContext.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdModifyPin.h"
#include "pinpad/EstablishPaceChannel.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include <QSharedPointer>


namespace governikus
{

class IfdServiceContext
	: public WorkflowContext
{
	Q_OBJECT

	private:
		const QSharedPointer<IfdServer> mIfdServer;

		QString mNewPin;

		QString mSlotHandle;
		QString mDisplayText;
		EstablishPaceChannel mEstablishPaceChannel;
		bool mRequestTransportPin;
		bool mAllowToChangePinLength;
		EstablishPaceChannelOutput mEstablishPaceChannelOutput;
		QSharedPointer<AccessRightManager> mAccessRightManager;

		QSharedPointer<const IfdModifyPin> mModifyPinMessage;
		ResponseApdu mModifyPinMessageResponseApdu;

		[[nodiscard]] bool isPaceRequestingRights() const;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler) const;

	Q_SIGNALS:
		void fireCardConnected(const QSharedPointer<CardConnection>& pConnection);
		void fireDisplayTextChanged();
		void fireCardDisconnected(const QSharedPointer<CardConnection>& pConnection);
		void fireUserError(StatusCode pStatusCode);
		void fireEstablishPaceChannelUpdated();
		void fireIsRunningChanged();
		void fireAccessRightManagerCreated(QSharedPointer<AccessRightManager> pAccessRightManager);

	public:
		explicit IfdServiceContext(const QSharedPointer<IfdServer>& pIfdServer);
		~IfdServiceContext() override;

		[[nodiscard]] const QSharedPointer<IfdServer>& getIfdServer() const;
		[[nodiscard]] bool isRunning() const;

		[[nodiscard]] const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		[[nodiscard]] bool isPinChangeWorkflow() const;
		[[nodiscard]] bool isPinAuthentication() const;
		[[nodiscard]] bool isCanAllowedMode() const override;

		void userError(StatusCode pStatusCode);

		void setEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		[[nodiscard]] const QString& getSlotHandle() const;
		void setDisplayText(const QString& pDisplayText);
		[[nodiscard]] const QString& getDisplayText() const;
		[[nodiscard]] const EstablishPaceChannel& getEstablishPaceChannel() const;
		[[nodiscard]] QSharedPointer<AccessRightManager> getAccessRightManager() const;
		[[nodiscard]] QSharedPointer<const CertificateDescription> getCertificateDescription() const;

		void changePinLength();
		[[nodiscard]] bool allowToChangePinLength() const;
		[[nodiscard]] bool isRequestTransportPin() const override;

		void setEstablishPaceChannelOutput(const EstablishPaceChannelOutput& pEstablishPaceChannelOutput);
		[[nodiscard]] const EstablishPaceChannelOutput& getEstablishPaceChannelOutput() const;

		void setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage);
		[[nodiscard]] const QSharedPointer<const IfdModifyPin>& getModifyPinMessage() const;

		void setModifyPinMessageResponseApdu(const ResponseApdu& pModifyPinMessageResponseApdu);
		[[nodiscard]] const ResponseApdu& getModifyPinMessageResponseApdu() const;
		[[nodiscard]] bool modifyPinRunning() const;

		void resetPacePasswords() override;
		Q_INVOKABLE void reset();

		[[nodiscard]] QList<AcceptedEidType> getAcceptedEidTypes() const override;


};

} // namespace governikus
