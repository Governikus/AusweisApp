/*!
 * \brief Model implementation for the PIN action.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class ChangePinContext;

class ChangePinModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(QString resultString READ getResultString NOTIFY fireResultChanged)
	Q_PROPERTY(bool changedPinSuccessfully READ isResultOk NOTIFY fireResultChanged)

	QSharedPointer<ChangePinContext> mContext;

	public:
		ChangePinModel(QObject* pParent = nullptr);
		virtual ~ChangePinModel();

		void resetContext(const QSharedPointer<ChangePinContext>& pContext = QSharedPointer<ChangePinContext>());

		QString getCurrentState() const;
		QString getResultString() const;
		bool isResultOk() const;

		Q_INVOKABLE void startWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void setReaderType(const QString& pReaderType);
		Q_INVOKABLE bool isBasicReader();
		Q_INVOKABLE void abortCardSelection();

	Q_SIGNALS:
		void fireStartWorkflow();
		void fireCurrentStateChanged(const QString& pState);
		void fireResultChanged();
};


} /* namespace governikus */
