/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Column {
	id: baseItem

	property string description: ""

	signal scrollDescriptionDown
	signal scrollDescriptionUp

	spacing: Constants.pane_spacing

	GText {
		//: LABEL ANDROID_TABLET IOS_TABLET
		text: qsTr("Description")
		textStyle: Style.text.header_accent
	}
	GText {
		horizontalAlignment: Text.AlignLeft
		//: LABEL ANDROID_TABLET IOS_TABLET
		text: (!!baseItem.description ? baseItem.description : qsTr("The provider did not provide a description."))
		textFormat: Text.RichText
		textStyle: Style.text.normal_secondary
		width: parent.width

		Accessible.onScrollDownAction: baseItem.scrollDescriptionDown()
		Accessible.onScrollUpAction: baseItem.scrollDescriptionUp()
	}
}
