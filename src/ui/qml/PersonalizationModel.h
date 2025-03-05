/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AuthModel.h"
#include "SingletonCreator.h"
#include "WorkflowRequest.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include <QtQml/qqmlregistration.h>

class test_UiPluginQml;


namespace governikus
{

class PersonalizationModel
	: public AuthModel
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;

	Q_PROPERTY(QString blockingCode READ getBlockingCode NOTIFY fireBlockingCodeChanged)
	Q_PROPERTY(int remainingAttempts READ getRemainingAttempts NOTIFY fireRemainingAttemptsChanged)
	Q_PROPERTY(QString restrictionDate READ getRestrictionDate NOTIFY fireRestrictionDateChanged)
	Q_PROPERTY(QString blockingPeriodMessage READ getBlockingPeriodMessage NOTIFY fireRestrictionDateChanged)
	Q_PROPERTY(bool applet READ isApplet NOTIFY fireIsAppletChanged)

	private:
		PersonalizationModel();

#if __has_include("context/PersonalizationContext.h")

	private:
		QSharedPointer<PersonalizationContext> mContext;

	public:
		void resetPersonalizationContext(const QSharedPointer<PersonalizationContext>& pContext = QSharedPointer<PersonalizationContext>());
#endif

	public:
		static PersonalizationModel* create(const QQmlEngine* pQmlEngine, const QJSEngine* pJSEngine)
		{
			return SingletonCreator<PersonalizationModel>::create(pQmlEngine, pJSEngine);
		}


#if __has_include("controller/PersonalizationController.h")
		Q_INVOKABLE void startWorkflow();
#else
		Q_INVOKABLE void startWorkflow() const
		{
		}


#endif
		[[nodiscard]] QString getBlockingCode() const;
		[[nodiscard]] int getRemainingAttempts() const;
		[[nodiscard]] QString getRestrictionDate() const;
		[[nodiscard]] QString getBlockingPeriodMessage() const;
		[[nodiscard]] bool isApplet() const;
		[[nodiscard]] QList<ReaderManagerPluginType> getSupportedReaderPluginTypes() const override;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireBlockingCodeChanged();
		void fireRemainingAttemptsChanged();
		void fireRestrictionDateChanged();
		void fireIsAppletChanged();
};

} // namespace governikus
