/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QtQmlCompiler/QtQmlCompiler>

namespace governikus
{
static constexpr QQmlSA::LoggerWarningId LogIdSource {"Plugin.AusweisApp.source"};

class ElementPass
	: public QQmlSA::ElementPass
{
	public:
		explicit ElementPass(QQmlSA::PassManager* pManager);
		void run(const QQmlSA::Element& pElement) override;
};

} // namespace governikus
