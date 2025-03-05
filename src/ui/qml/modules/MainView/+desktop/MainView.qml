/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.View
import Governikus.Type
import Governikus.Style

SectionPage {
	id: root

	signal requestUiModule(var pModule)

	title: ""

	GridLayout {
		columnSpacing: Style.dimens.pane_spacing
		columns: 2
		rowSpacing: Style.dimens.pane_spacing
		rows: 2

		anchors {
			bottomMargin: anchors.topMargin
			fill: parent
			leftMargin: anchors.rightMargin
			rightMargin: Style.dimens.pane_spacing + Math.max(0, (root.width - root.height) / 2)
			topMargin: Style.dimens.pane_spacing + Math.max(0, (root.height - root.width) / 2)
		}
		Tile {
			icon.source: "qrc:/images/lock.svg"

			//: LABEL DESKTOP
			text: qsTr("Change PIN")

			onClicked: root.requestUiModule(UiModule.PINMANAGEMENT)
		}
		Tile {
			icon.source: "qrc:/images/mydata_tile.svg"

			//: LABEL DESKTOP
			text: qsTr("See my<br>personal data")

			onClicked: root.requestUiModule(UiModule.SELF_AUTHENTICATION)
		}
		Tile {
			icon.source: "qrc:/images/desktop/settings.svg"

			//: LABEL DESKTOP
			text: qsTr("Settings")

			onClicked: root.requestUiModule(UiModule.SETTINGS)
		}
		Tile {
			icon.source: "qrc:/images/desktop/help.svg"

			//: LABEL DESKTOP
			text: qsTr("Help")

			onClicked: root.requestUiModule(UiModule.HELP)
		}
	}
}
