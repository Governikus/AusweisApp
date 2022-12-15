/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property alias image: icon.source
	property string name: ""
	property alias title: text.label

	Accessible.name: text.Accessible.name
	Accessible.role: Accessible.StaticText
	height: contentRow.height
	width: contentRow.width

	Row {
		id: contentRow
		height: text.height
		spacing: Constants.groupbox_spacing

		Image {
			id: icon
			anchors.verticalCenter: text.verticalCenter
			sourceSize.height: Style.dimens.icon_size
		}
		LabeledText {
			id: text
			activeFocusOnTab: false
			bodyElide: Text.ElideRight
			maximumBodyLineCount: 1

			//: LABEL DESKTOP
			text: name.length > 0 ? name : qsTr("See details under \"more...\"")
			width: baseItem.width - icon.width - Constants.groupbox_spacing
		}
	}
	FocusFrame {
	}
}
