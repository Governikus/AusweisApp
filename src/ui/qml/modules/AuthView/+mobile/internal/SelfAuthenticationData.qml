/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	property alias okButtonText: okButton.text

	spacing: Style.dimens.pane_spacing
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	GridLayout {
		id: grid

		columnSpacing: Style.dimens.text_spacing
		columns: Math.max(1, (width + columnSpacing) / (repeater.maxItemWidth + columnSpacing))
		rowSpacing: Style.dimens.text_spacing
		width: parent.width

		GRepeater {
			id: repeater

			model: SelfAuthModel

			LabeledText {
				required property string name
				required property string value

				Layout.fillWidth: true
				label: name
				text: value === "" ? "---" : value

				Accessible.onScrollDownAction: root.scrollPageDown()
				Accessible.onScrollUpAction: root.scrollPageUp()
			}
		}
	}
	GButton {
		id: okButton

		Layout.alignment: Qt.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Back to start page")

		onClicked: root.leaveView()
	}
}
