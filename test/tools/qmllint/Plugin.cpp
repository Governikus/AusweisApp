/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "Plugin.h"

#include "ElementPass.h"

using namespace governikus;

void Plugin::registerPasses(QQmlSA::PassManager* pManager, const QQmlSA::Element& pRootElement)
{
	Q_UNUSED(pRootElement)

	if (pManager->isCategoryEnabled(LogIdSource))
	{
		pManager->registerElementPass(std::make_unique<ElementPass>(pManager));
	}
}
