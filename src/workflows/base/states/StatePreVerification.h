/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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
		const QList<QSharedPointer<const CVCertificate>> mTrustedCvcas;
		const QDateTime mValidationDateTime;

		explicit StatePreVerification(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

		bool isValid(const QList<QSharedPointer<const CVCertificate>>& pCertificates) const;
		void saveCvcaLinkCertificates(const QList<QSharedPointer<const CVCertificate>>& pCertificates) const;
};

} // namespace governikus
