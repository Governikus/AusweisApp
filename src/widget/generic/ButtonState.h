/*!
 * \brief Defines the ButtonState enum.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace governikus
{

enum class ButtonState
{
	/*! Button is visible, enabled, and focussed. */
	FOCUSSED,

	/*! Button is visible and enabled. */
	ENABLED,

	/*! Button is visible and disabled. */
	DISABLED,

	/*! Button is not visible. */
	HIDDEN
};

} /* namespace governikus */
