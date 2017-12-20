/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class WorkflowContext;

class WorkflowModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(QString resultString READ getResultString NOTIFY fireResultChanged)
	Q_PROPERTY(bool error READ isError NOTIFY fireResultChanged)
	Q_PROPERTY(QString readerPlugInType READ getReaderPlugInType WRITE setReaderPlugInType NOTIFY fireReaderPlugInTypeChanged)
	Q_PROPERTY(bool isBasicReader READ isBasicReader NOTIFY fireIsBasicReaderChanged)

	private:
		QSharedPointer<WorkflowContext> mContext;

	public:
		WorkflowModel(QObject* pParent = nullptr);
		virtual ~WorkflowModel();

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		QString getCurrentState() const;
		virtual QString getResultString() const;
		bool isError() const;

		QString getReaderPlugInType() const;
		void setReaderPlugInType(const QString& pReaderPlugInType);

		bool isBasicReader();

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void cancelWorkflowOnPinBlocked();
		Q_INVOKABLE void continueWorkflow();

	Q_SIGNALS:
		void fireStartWorkflow();
		void fireCurrentStateChanged(const QString& pState);
		void fireResultChanged();
		void fireReaderPlugInTypeChanged();
		void fireIsBasicReaderChanged();
};


} /* namespace governikus */
