/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the authentication action.
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"
#include "UIPlugIn.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

class test_WorkflowModel;

namespace governikus
{

class WorkflowModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(QString resultString READ getResultString NOTIFY fireResultChanged)
	Q_PROPERTY(bool error READ isError NOTIFY fireResultChanged)
	Q_PROPERTY(bool errorIsMasked READ isMaskedError NOTIFY fireResultChanged)
	Q_PROPERTY(ReaderManagerPlugInType readerPlugInType READ getReaderPlugInType WRITE setReaderPlugInType NOTIFY fireReaderPlugInTypeChanged)
	Q_PROPERTY(bool isSmartSupported READ isSmartSupported NOTIFY fireSupportedPlugInTypesChanged)
	Q_PROPERTY(QList<ReaderManagerPlugInType> supportedPlugInTypes READ getSupportedReaderPlugInTypes NOTIFY fireSupportedPlugInTypesChanged)
	Q_PROPERTY(bool isBasicReader READ isBasicReader NOTIFY fireSelectedReaderChanged)
	Q_PROPERTY(bool isRemoteReader READ isRemoteReader NOTIFY fireSelectedReaderChanged)
	Q_PROPERTY(bool isCurrentSmartCardAllowed READ isCurrentSmartCardAllowed NOTIFY fireIsCurrentSmartCardAllowedChanged)
	Q_PROPERTY(QString eidTypeMismatchError READ eidTypeMismatchError NOTIFY fireEidTypeMismatchErrorChanged)
	Q_PROPERTY(bool hasNextWorkflowPending READ getNextWorkflowPending NOTIFY fireNextWorkflowPendingChanged)
	Q_PROPERTY(QString statusHintText READ getStatusHintText NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusHintActionText READ getStatusHintActionText NOTIFY fireResultChanged)
	Q_PROPERTY(QString statusCodeImage READ getStatusCodeImage NOTIFY fireResultChanged)
	Q_PROPERTY(bool showRemoveCardFeedback READ showRemoveCardFeedback WRITE setRemoveCardFeedback NOTIFY fireRemoveCardFeedbackChanged)
	Q_PROPERTY(bool hasCard READ hasCard NOTIFY fireHasCardChanged)
	friend class ::test_WorkflowModel;

	private:
		QSharedPointer<WorkflowContext> mContext;
#if defined(Q_OS_IOS)
		bool mRemoteScanWasRunning;
#endif
		void insertCard(ReaderManagerPlugInType pType) const;

	public:
		explicit WorkflowModel(QObject* pParent = nullptr);
		~WorkflowModel() override = default;

		void resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] QString getCurrentState() const;
		[[nodiscard]] virtual QString getResultString() const;
		[[nodiscard]] bool isError() const;
		[[nodiscard]] bool isMaskedError() const;

		[[nodiscard]] ReaderManagerPlugInType getReaderPlugInType() const;
		void setReaderPlugInType(ReaderManagerPlugInType pReaderPlugInType);

		[[nodiscard]] bool isBasicReader() const;
		[[nodiscard]] bool isRemoteReader() const;
		[[nodiscard]] bool hasCard() const;

		[[nodiscard]] bool isCurrentSmartCardAllowed() const;

		[[nodiscard]] bool isSmartSupported() const;
		[[nodiscard]] virtual QList<ReaderManagerPlugInType> getSupportedReaderPlugInTypes() const;

		[[nodiscard]] bool getNextWorkflowPending() const;

		[[nodiscard]] GlobalStatus::Code getStatusCode() const;
		[[nodiscard]] QString getStatusCodeImage() const;


		[[nodiscard]] QString getStatusHintText() const;
		[[nodiscard]] QString getStatusHintActionText() const;
		[[nodiscard]] Q_INVOKABLE bool invokeStatusHintAction();

		[[nodiscard]] bool showRemoveCardFeedback() const;
		void setRemoveCardFeedback(bool pEnabled);

		Q_INVOKABLE void insertSmartCard();
		Q_INVOKABLE void insertSimulator();
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

	Q_SIGNALS:
		void fireWorkflowStarted();
		void fireCurrentStateChanged(const QString& pState);
		void fireStateEntered(const QString& pState);
		void fireResultChanged();
		void fireReaderPlugInTypeChanged(bool pExplicitStart = false);
		void fireSelectedReaderChanged();
		void fireIsCurrentSmartCardAllowedChanged();
		void fireReaderImageChanged();
		void fireNextWorkflowPendingChanged();
		void fireSupportedPlugInTypesChanged();
		void fireRemoveCardFeedbackChanged();
		void fireHasCardChanged();
		void fireEidTypeMismatchErrorChanged();
		void fireShowUiRequest(UiModule pModule);
		void fireWorkflowFinished();
};


} // namespace governikus
