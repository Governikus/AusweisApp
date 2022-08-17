/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

QtObject {
	property color textColor: Style.color.primary_text
	property color linkColor: Style.color.accent_text
	property int textSize: Style.dimens.normal_font_size

	/// An empty string means "unspecified"
	property string textFamily
}
