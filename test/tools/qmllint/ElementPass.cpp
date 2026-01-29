/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ElementPass.h"

#include <QRegularExpression>

using namespace governikus;

ElementPass::ElementPass(QQmlSA::PassManager* pManager)
	: QQmlSA::ElementPass(pManager)
{
}


void ElementPass::run(const QQmlSA::Element& pElement)
{
	const QRegularExpression regex(QStringLiteral(R"(source (===|!==) ["'])"));

	const auto& bindings = pElement.ownPropertyBindings();
	for (const auto& binding : bindings)
	{
		if (binding.bindingType() == QQmlSA::BindingType::Script &&
				regex.match(sourceCode(binding.sourceLocation())).hasMatch())
		{
			emitWarning("Use source.toString() as it is an URL", LogIdSource, binding.sourceLocation());
		}
	}
}
