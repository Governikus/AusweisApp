/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Initializer.h"

#include "SingletonHelper.h"

#include <QCoreApplication>

using namespace governikus;

defineSingleton(Initializer)

Initializer & Initializer::getInstance()
{
	return *Instance;
}

void Initializer::init()
{
	for (const auto& entry : getInstance().mRegisteredFunctions)
	{
		entry();
	}
}


void Initializer::add(const std::function<void()>& pRegister)
{
	mRegisteredFunctions.push_back(pRegister);

	if (QCoreApplication::instance())
	{
		pRegister();
	}
}


namespace
{
static void initialize()
{
	Initializer::getInstance().init();
}


} // namespace

Q_COREAPP_STARTUP_FUNCTION(initialize)
