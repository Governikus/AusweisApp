/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "ReaderManagerPlugInInfo.h"

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
	Q_PROPERTY(bool isBasicReader READ isBasicReader NOTIFY fireIsBasicReaderChanged)
	Q_PROPERTY(QString readerImage READ getReaderImage NOTIFY fireReaderImageChanged)
	Q_PROPERTY(bool hasNextWorkflowPending READ getNextWorkflowPending NOTIFY fireNextWorkflowPendingChanged)

	private:
		friend class ::test_WorkflowModel;

		QSharedPointer<WorkflowContext> mContext;
		QString mReaderImage;

	public:
		explicit WorkflowModel(QObject* pParent = nullptr);
		~WorkflowModel() override;

		void resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		QString getCurrentState() const;
		virtual QString getResultString() const;
		bool isError() const;
		bool isMaskedError() const;

		ReaderManagerPlugInType getReaderPlugInType() const;
		void setReaderPlugInType(ReaderManagerPlugInType pReaderPlugInType);

		bool isBasicReader() const;

		bool getNextWorkflowPending() const;

		QString getReaderImage() const;

		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void startScanIfNecessary();
		Q_INVOKABLE void cancelWorkflowToChangePin();
		Q_INVOKABLE void continueWorkflow();
		Q_INVOKABLE void setInitialPluginType();
		Q_INVOKABLE bool selectedReaderHasCard() const;
		Q_INVOKABLE bool shouldSkipResultView() const;
		Q_INVOKABLE bool isCancellationByUser() const;
		Q_INVOKABLE QString getEmailHeader() const;
		Q_INVOKABLE QString getEmailBody(bool pPercentEncoding = false, bool pAddLogNotice = false) const;
		Q_INVOKABLE void sendResultMail() const;

	public Q_SLOTS:
		void onReaderManagerSignal();

	Q_SIGNALS:
		void fireCurrentStateChanged(const QString& pState);
		void fireResultChanged();
		void fireReaderPlugInTypeChanged();
		void fireIsBasicReaderChanged();
		void fireReaderImageChanged();
		void fireNextWorkflowPendingChanged();
};


} // namespace governikus
