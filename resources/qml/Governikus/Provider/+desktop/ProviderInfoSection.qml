/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	property alias image: icon.source
	property alias title: text.label
	property string name: ""

	Accessible.role: Accessible.StaticText
	Accessible.name: text.Accessible.name

	height: contentRow.height
	width: contentRow.width

	Row {
		id: contentRow
		height: text.height
		spacing: Constants.groupbox_spacing

		Image {
			id: icon
			sourceSize.height: ApplicationModel.scaleFactor * 40
			anchors.verticalCenter: text.verticalCenter
		}

		LabeledText {
			id: text
			width: baseItem.width - icon.width - Constants.groupbox_spacing
			//: LABEL DESKTOP_QML
			text: name.length > 0 ? name : qsTr("See details under \"more...\"") + SettingsModel.translationTrigger
			maximumBodyLineCount: 1
			bodyElide: Text.ElideRight
		}
	}

	FocusFrame {
		borderColor: Style.color.focus_indicator
	}
}
