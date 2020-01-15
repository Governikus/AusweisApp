/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

QtObject {
	readonly property var tutorial_title: TextStyle {
		textSize: Style.dimens.tutorial_title_font_size
	}
	readonly property var tutorial_header: TextStyle {
		textSize: Style.dimens.tutorial_header_font_size
		textFamily: "Charter"
	}
	readonly property var tutorial_header_secondary: TextStyle {
		textSize: Style.dimens.header_font_size
		textFamily: "Charter"
	}
	readonly property var tutorial_content: TextStyle {
		textSize: Style.dimens.normal_font_size
		textFamily: "Charter"
	}
}
