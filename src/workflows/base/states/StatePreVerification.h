/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Performs the pre-verification of cvcs. If the pre-verification succeeds the cvc chain is set to the authentication model.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

#include <QDateTime>


namespace governikus
{

class StatePreVerification
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StatePreVerification;

	private:
		const QVector<QSharedPointer<const CVCertificate>> mTrustedCvcas;
		const QDateTime mValidationDateTime;

		explicit StatePreVerification(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

		bool isValid(const QVector<QSharedPointer<const CVCertificate>>& pCertificates) const;
		void saveCvcaLinkCertificates(const QVector<QSharedPointer<const CVCertificate>>& pCertificates) const;
};

} // namespace governikus
