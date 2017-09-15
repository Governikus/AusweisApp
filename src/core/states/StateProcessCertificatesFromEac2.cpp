/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateProcessCertificatesFromEac2.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "Result.h"

#include <QDebug>


using namespace governikus;


StateProcessCertificatesFromEac2::StateProcessCertificatesFromEac2(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateProcessCertificatesFromEac2::run()
{
	if (getContext()->hasChainForCertificationAuthority(*getContext()->getPaceOutputData()))
	{
		qDebug() << "CVC chain already determined, skip further processing";
		Q_EMIT fireSuccess();
		return;
	}

	QVector<QSharedPointer<CVCertificate> > cvcs;
	for (const auto& cvc : getContext()->getDidAuthenticateEac2()->getCvCertificates())
	{
		// according to TR-03112-7, paragraph 3.6.4.2, AT certs must be ignored
		if (cvc->getBody().getCHAT().getAccessRole() == AccessRole::AT)
		{
			qWarning() << "Ignoring AT CVC from EAC2InputType" << *cvc;
			continue;
		}
		cvcs += cvc;
	}
	getContext()->initCvcChainBuilder(cvcs);

	if (!getContext()->hasChainForCertificationAuthority(*getContext()->getPaceOutputData()))
	{
		qCritical() << "No cvc chain determined, abort authentication";
		setStatus(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity);
		Q_EMIT fireError();
	}
	else
	{
		Q_EMIT fireSuccess();
	}
}
