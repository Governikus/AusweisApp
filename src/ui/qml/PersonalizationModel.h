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


namespace governikus
{

class PersonalizationModel
	: public AuthModel
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString blockingCode READ getBlockingCode NOTIFY fireBlockingCodeChanged)
	Q_PROPERTY(int remainingAttempts READ getRemainingAttempts NOTIFY fireRemainingAttemptsChanged)
	Q_PROPERTY(QString restrictionDate READ getRestrictionDate NOTIFY fireRestrictionDateChanged)

	private:
		PersonalizationModel();

#if __has_include("context/PersonalizationContext.h")

	private:
		QSharedPointer<PersonalizationContext> mContext;

	public:
		void resetPersonalizationContext(const QSharedPointer<PersonalizationContext>& pContext = QSharedPointer<PersonalizationContext>());
#endif

	public:
		Q_INVOKABLE void startWorkflow();
		[[nodiscard]] QString getBlockingCode() const;
		[[nodiscard]] int getRemainingAttempts() const;
		[[nodiscard]] QString getRestrictionDate() const;
		[[nodiscard]] QVector<ReaderManagerPlugInType> getSupportedReaderPlugInTypes() const override;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireBlockingCodeChanged();
		void fireRemainingAttemptsChanged();
		void fireRestrictionDateChanged();
};

} // namespace governikus
