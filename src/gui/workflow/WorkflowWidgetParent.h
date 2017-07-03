/*!
 * WorkflowWidgetParent.h
 *
 * \brief Enum identifying the containers in the application GUI which can be
 * parent to a workflow widget.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

namespace governikus
{

enum class WorkflowWidgetParent
{
	Authentication,
	SettingsChangePin,
	SelfAuthentication
};


} /* namespace governikus */
