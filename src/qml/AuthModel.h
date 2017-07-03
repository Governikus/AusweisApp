/*!
 * \brief Model implementation for the authentication action.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class AuthContext;

class AuthModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(QString resultString READ getResultString NOTIFY fireResultChanged)
	Q_PROPERTY(bool error READ isError NOTIFY fireResultChanged)
	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)

	QSharedPointer<AuthContext> mContext;
	QString mTransactionInfo;

	public:
		AuthModel(QObject* pParent = nullptr);
		virtual ~AuthModel();

		void resetContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		QString getCurrentState() const;
		QString getResultString() const;
		bool isError() const;
		const QString& getTransactionInfo() const;

		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void cancelWorkflowOnPinBlocked();
		Q_INVOKABLE void continueWorkflow();
		Q_INVOKABLE void setReaderType(const QString& pReaderType);
		Q_INVOKABLE bool isBasicReader();
		Q_INVOKABLE void abortCardSelection();

	public Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	Q_SIGNALS:
		void fireCurrentStateChanged(const QString& pState);
		void fireResultChanged();
		void fireTransactionInfoChanged();
};


} /* namespace governikus */
