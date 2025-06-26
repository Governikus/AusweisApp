/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "ReaderManagerPluginInfo.h"
#include "UiPlugin.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

class test_WorkflowModel;

namespace governikus
{

defineEnumTypeQmlExposed(GAnimation,
		NONE,
		PIN_ERROR,
		CAN_ERROR,
		PUK_ERROR,
		PUK_BLOCKED,
		CARD_ERROR,
		NETWORK_ERROR,
		CHANGEPIN_SUCCESS,
		STATUS_OK,
		STATUS_ERROR
		)

class WorkflowModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(QString resultString READ getResultString NOTIFY fireResultChanged)
	Q_PROPERTY(bool error READ isError NOTIFY fireResultChanged)
	Q_PROPERTY(bool errorIsMasked READ isMaskedError NOTIFY fireResultChanged)
	Q_PROPERTY(bool pukInoperative READ isPukInoperative NOTIFY fireResultChanged)
	Q_PROPERTY(governikus::EnumReaderManagerPluginType::ReaderManagerPluginType readerPluginType READ getReaderPluginType WRITE setReaderPluginType NOTIFY fireReaderPluginTypeChanged)
	Q_PROPERTY(QList<ReaderManagerPluginType> supportedPluginTypes READ getSupportedReaderPluginTypes NOTIFY fireSupportedPluginTypesChanged)
	Q_PROPERTY(bool isBasicReader READ isBasicReader NOTIFY fireSelectedReaderChanged)
	Q_PROPERTY(bool isRemoteReader READ isRemoteReader NOTIFY fireSelectedReaderChanged)
	Q_PROPERTY(bool isCurrentSmartCardAllowed READ isCurrentSmartCardAllowed NOTIFY fireIsCurrentSmartCardAllowedChanged)
	Q_PROPERTY(QString eidTypeMismatchError READ eidTypeMismatchError NOTIFY fireEidTypeMismatchErrorChanged)
	Q_PROPERTY(bool hasNextWorkflowPending READ getNextWorkflowPending NOTIFY fireNextWorkflowPendingChanged)
	Q_PROPERTY(governikus::EnumGlobalStatusCode::GlobalStatusCode statusCode READ getStatusCode NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusCodeDisplayString READ getStatusCodeDisplayString NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusHintText READ getStatusHintText NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusHintTitle READ getStatusHintTitle NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusHintActionText READ getStatusHintActionText NOTIFY fireResultChanged)
	Q_PROPERTY(governikus::EnumGAnimation::GAnimation statusCodeAnimation READ getStatusCodeAnimation NOTIFY fireResultChanged)
	Q_PROPERTY(bool showRemoveCardFeedback READ showRemoveCardFeedback WRITE setRemoveCardFeedback NOTIFY fireRemoveCardFeedbackChanged)
	Q_PROPERTY(bool cardInitiallyAppeared READ getCardInitiallyAppeared NOTIFY fireHasCardChanged)
	Q_PROPERTY(bool hasCard READ hasCard NOTIFY fireHasCardChanged)
	Q_PROPERTY(governikus::EnumCardReturnCode::CardReturnCode lastReturnCode READ getLastReturnCode NOTIFY fireLastReturnCodeChanged)
	friend class ::test_WorkflowModel;

	private:
		QSharedPointer<WorkflowContext> mContext;
#if defined(Q_OS_IOS)
		bool mRemoteScanWasRunning;
#endif
		void insertCard(ReaderManagerPluginType pType) const;

	protected:
		WorkflowModel();
		~WorkflowModel() override = default;

	public:
		void resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] QString getCurrentState() const;
		[[nodiscard]] virtual QString getResultString() const;
		[[nodiscard]] bool isError() const;
		[[nodiscard]] bool isMaskedError() const;
		[[nodiscard]] bool isPukInoperative() const;
		[[nodiscard]] CardReturnCode getLastReturnCode() const;

		[[nodiscard]] ReaderManagerPluginType getReaderPluginType() const;
		void setReaderPluginType(ReaderManagerPluginType pReaderPluginType);

		[[nodiscard]] bool isBasicReader() const;
		[[nodiscard]] bool isRemoteReader() const;
		[[nodiscard]] bool getCardInitiallyAppeared() const;
		[[nodiscard]] bool hasCard() const;

		[[nodiscard]] bool isCurrentSmartCardAllowed() const;

		[[nodiscard]] virtual QList<ReaderManagerPluginType> getSupportedReaderPluginTypes() const;

		[[nodiscard]] bool getNextWorkflowPending() const;

		[[nodiscard]] GlobalStatus::Code getStatusCode() const;
		[[nodiscard]] QString getStatusCodeDisplayString() const;
		[[nodiscard]] virtual GAnimation getStatusCodeAnimation() const;

		[[nodiscard]] QString getStatusHintText() const;
		[[nodiscard]] QString getStatusHintTitle() const;
		[[nodiscard]] QString getStatusHintActionText() const;
		[[nodiscard]] Q_INVOKABLE bool invokeStatusHintAction();

		[[nodiscard]] bool showRemoveCardFeedback() const;
		void setRemoveCardFeedback(bool pEnabled);

		Q_INVOKABLE void insertSmartCard() const;
		Q_INVOKABLE void insertSimulator() const;
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void startScanExplicitly();
		Q_INVOKABLE void continueWorkflow();
		Q_INVOKABLE void setInitialPluginType();
		[[nodiscard]] Q_INVOKABLE bool shouldSkipResultView() const;
		[[nodiscard]] Q_INVOKABLE bool isCancellationByUser() const;
		[[nodiscard]] Q_INVOKABLE QString getEmailHeader() const;
		[[nodiscard]] Q_INVOKABLE QString getEmailBody(bool pPercentEncoding = false, bool pAddLogNotice = false) const;
		Q_INVOKABLE void sendResultMail() const;

		[[nodiscard]] QString eidTypeMismatchError() const;

	private Q_SLOTS:
		void onApplicationStateChanged(bool pIsAppInForeground);
		void onPaceResultUpdated();

	Q_SIGNALS:
		void fireWorkflowStarted();
		void fireCurrentStateChanged(const QString& pState);
		void fireStateEntered(const QString& pState);
		void fireResultChanged();
		void fireReaderPluginTypeChanged(bool pExplicitStart = false);
		void fireSelectedReaderChanged();
		void fireIsCurrentSmartCardAllowedChanged();
		void fireReaderImageChanged();
		void fireNextWorkflowPendingChanged();
		void fireSupportedPluginTypesChanged();
		void fireRemoveCardFeedbackChanged();
		void fireHasCardChanged();
		void fireEidTypeMismatchErrorChanged();
		void fireShowUiRequest(UiModule pModule);
		void fireWorkflowFinished(bool pSuccess);
		void fireOnPinUnlocked();
		void fireOnPasswordUsed();
		void fireOnCanSuccess();
		void fireLastReturnCodeChanged();
};


} // namespace governikus
