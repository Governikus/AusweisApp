/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	signal done
	signal requestBack

	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: {
			root.requestBack();
			root.done();
		}
	}

	GridLayout {
		id: grid

		columnSpacing: Constants.text_spacing
		columns: Math.max(1, (width + columnSpacing) / (repeater.maxItemWidth + columnSpacing))
		rowSpacing: Constants.text_spacing
		width: parent.width

		GRepeater {
			id: repeater

			model: SelfAuthModel

			LabeledText {
				Layout.fillWidth: true
				label: name
				text: value === "" ? "---" : value

				Accessible.onScrollDownAction: root.scrollPageDown()
				Accessible.onScrollUpAction: root.scrollPageUp()
			}
		}
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Back to start page")

		onClicked: root.done()
		onFocusChanged: if (focus)
			root.positionViewAtItem(this)
	}
}
