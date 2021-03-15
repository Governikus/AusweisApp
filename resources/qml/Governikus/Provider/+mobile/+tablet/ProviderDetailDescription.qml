/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Column {
	id: baseItem
	spacing: Constants.pane_spacing

	signal scrollDescriptionUp()
	signal scrollDescriptionDown()

	property string description: ""

	GText {
		Accessible.role: Accessible.StaticText
		Accessible.name: text

		//: LABEL ANDROID_TABLET IOS_TABLET
		text: qsTr("Description")
		textStyle: Style.text.header_accent
	}

	GText {
		width: parent.width

		Accessible.role: Accessible.StaticText
		Accessible.name: text
		Accessible.onScrollDownAction: baseItem.scrollDescriptionDown()
		Accessible.onScrollUpAction: baseItem.scrollDescriptionUp()

		horizontalAlignment: Text.AlignLeft
		text: baseItem.description
		textStyle: Style.text.normal_secondary
	}
}
