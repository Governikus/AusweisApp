/*
 * \brief Performs the pre-verification of cvcs. If the pre-verification succeeds the cvc chain is set to the authentication model.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

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

	const QVector<QSharedPointer<const CVCertificate> > mTrustedCvcas;
	const QDateTime mValidationDateTime;

	explicit StatePreVerification(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	bool isValid(const QVector<QSharedPointer<const CVCertificate> >& pCertificates);
	void saveCvcaLinkCertificates(const QVector<QSharedPointer<const CVCertificate> >& pCertificates);
};

} // namespace governikus
