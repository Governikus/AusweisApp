/*
 * StatePreVerification.h
 *
 * \brief Performs the pre-verification of cvcs. If the pre-verification succeeds the cvc chain is set to the authentication model.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

#include <QDateTime>

class test_StatePreVerification;

namespace governikus
{

class StatePreVerification
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class::test_StatePreVerification;

	const QVector<QSharedPointer<CVCertificate> > mTrustedCvcas;
	const QDateTime mValidationDateTime;

	StatePreVerification(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	bool isValid(const QVector<QSharedPointer<CVCertificate> >& pCertificates);
	void saveCvcaLinkCertificates(const QVector<QSharedPointer<CVCertificate> >& pCertificates);
};

} /* namespace governikus */
