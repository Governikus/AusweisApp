/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"
#include "WorkflowModel.h"
#include "context/AuthContext.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>
#include <QtQml/qqmlregistration.h>


class test_UiPluginQml;


namespace governikus
{

class AuthModel
	: public WorkflowModel
	, public SingletonCreator<AuthModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;

	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)
	Q_PROPERTY(int progressValue READ getProgressValue NOTIFY fireProgressChanged)
	Q_PROPERTY(QString progressMessage READ getProgressMessage NOTIFY fireProgressChanged)
	Q_PROPERTY(bool changeTransportPin READ getChangeTransportPin NOTIFY fireChangeTransportPinChanged)
	Q_PROPERTY(QString resultHeader READ getResultHeader NOTIFY fireResultChanged)
	Q_PROPERTY(QString errorHeader READ getErrorHeader NOTIFY fireResultChanged)
	Q_PROPERTY(QString errorText READ getErrorText NOTIFY fireResultChanged)
	Q_PROPERTY(QString resultViewButtonIcon READ getResultViewButtonIcon NOTIFY fireResultChanged)
	Q_PROPERTY(QString resultViewButtonText READ getResultViewButtonText NOTIFY fireResultChanged)
	Q_PROPERTY(QUrl resultViewButtonLink READ getResultViewButtonLink NOTIFY fireResultChanged)

	private:
		QSharedPointer<AuthContext> mContext;
		QString mTransactionInfo;

	protected:
		AuthModel();
		~AuthModel() override = default;

	public:
		void resetAuthContext(const QSharedPointer<AuthContext>& pContext = QSharedPointer<AuthContext>());

		[[nodiscard]] const QString& getTransactionInfo() const;
		[[nodiscard]] int getProgressValue() const;
		[[nodiscard]] QString getProgressMessage() const;
		[[nodiscard]] bool getChangeTransportPin() const;
		[[nodiscard]] QString getResultHeader() const;
		[[nodiscard]] QString getErrorHeader() const;
		[[nodiscard]] QString getErrorText() const;
		[[nodiscard]] QString getResultViewButtonIcon() const;
		[[nodiscard]] QString getResultViewButtonText() const;
		[[nodiscard]] QUrl getResultViewButtonLink() const;

		Q_INVOKABLE void cancelWorkflowToChangeTransportPin();

	private Q_SLOTS:
		void onDidAuthenticateEac1Changed();

	Q_SIGNALS:
		void fireChangeTransportPinChanged();
		void fireTransactionInfoChanged();
		void fireProgressChanged();
};


} // namespace governikus
