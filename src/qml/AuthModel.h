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

	QSharedPointer<AuthContext> mContext;

	public:
		AuthModel(QObject* pParent = nullptr);
		virtual ~AuthModel();

		void resetContext(QSharedPointer<AuthContext> pContext = QSharedPointer<AuthContext>());

		QString getCurrentState() const;
		QString getResultString() const;
		bool isError() const;

		Q_INVOKABLE void continueWorkflow();
		Q_INVOKABLE void cancelWorkflow();
		Q_INVOKABLE void setReaderType(const QString& pReaderType);
		Q_INVOKABLE bool isBasicReader();
		Q_INVOKABLE void abortCardSelection();

	Q_SIGNALS:
		void fireCurrentStateChanged(const QString& pState);
		void fireResultChanged();
};


} /* namespace governikus */
