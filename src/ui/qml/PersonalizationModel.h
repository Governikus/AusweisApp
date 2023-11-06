/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for Smart-eID workflow.
 */

#pragma once

#include "AuthModel.h"
#include "WorkflowRequest.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif


class test_UIPlugInQml;


namespace governikus
{

class PersonalizationModel
	: public AuthModel
{
	Q_OBJECT
	friend class Env;
	friend class ::test_UIPlugInQml;

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
		Q_INVOKABLE void startWorkflow() const;
		[[nodiscard]] QString getBlockingCode() const;
		[[nodiscard]] int getRemainingAttempts() const;
		[[nodiscard]] QString getRestrictionDate() const;
		[[nodiscard]] QString getBlockingPeriodMessage() const;
		[[nodiscard]] bool isApplet() const;
		[[nodiscard]] QVector<ReaderManagerPlugInType> getSupportedReaderPlugInTypes() const override;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest) const;
		void fireBlockingCodeChanged();
		void fireRemainingAttemptsChanged();
		void fireRestrictionDateChanged();
		void fireIsAppletChanged();
};

} // namespace governikus
