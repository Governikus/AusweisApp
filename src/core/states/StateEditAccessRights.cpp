/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateEditAccessRights.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;

StateEditAccessRights::StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateEditAccessRights::run()
{
	const auto& effectiveRights = getContext()->getEffectiveAccessRights();
	qCDebug(statemachine) << effectiveRights;

	const auto& requiredRights = getContext()->getRequiredAccessRights();
	printRights(QStringLiteral("Required rights"), requiredRights);

	const auto& optionalRights = getContext()->getOptionalAccessRights();
	printRights(QStringLiteral("Optional selected rights"), effectiveRights & optionalRights);

	printRights(QStringLiteral("Optional not selected rights"), optionalRights - effectiveRights);

	Q_EMIT fireContinue();
}


void StateEditAccessRights::printRights(const QString& pTitle, const QSet<AccessRight>& pRights) const
{
	QStringList accessRightsToDisplay;
	if (pRights.isEmpty())
	{
		accessRightsToDisplay += QStringLiteral("None");
	}
	else
	{
		for (const auto& accessRight : pRights)
		{
			accessRightsToDisplay += AccessRoleAndRightsUtil::toDisplayText(accessRight);
		}
	}
	qCDebug(statemachine) << pTitle << ":" << accessRightsToDisplay.join(QLatin1String(", "));
}
