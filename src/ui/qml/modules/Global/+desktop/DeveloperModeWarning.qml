/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	Crossed {
		anchors.fill: parent
	}
	Rectangle {
		color: Style.color.paneSublevel.background.basic
		height: childrenRect.height
		opacity: 0.7
		radius: Style.dimens.pane_radius
		width: childrenRect.width

		anchors {
			bottom: parent.bottom
			bottomMargin: 4
			horizontalCenter: parent.horizontalCenter
		}
		Row {
			padding: Style.dimens.pane_padding / 2
			spacing: Style.dimens.groupbox_spacing

			GText {
				anchors.verticalCenter: parent.verticalCenter

				//: LABEL DESKTOP
				text: qsTr("Developer Mode: Enabled!")
			}
			GButton {
				//: LABEL DESKTOP Global button to disable developer mode.
				text: qsTr("Disable")

				onClicked: SettingsModel.developerMode = false
			}
		}
	}
}
