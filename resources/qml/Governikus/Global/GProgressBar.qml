/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

ProgressBar {
	id: progressBar

	property alias text: progressText.text
	property color backgroundColor: Constants.white

	Accessible.role: Accessible.ProgressBar
	Accessible.name: qsTr("%1 percent done").arg(value)

	from: 0
	to: 100

	background: Rectangle {
		radius: Style.dimens.button_radius
		color: Style.color.border
		border.color: Style.color.border
		border.width: Style.dimens.progress_bar_border
	}

	contentItem: Item {
		implicitHeight: Style.dimens.progress_bar_height

		Item {
			anchors.fill: parent
			anchors.margins: Style.dimens.progress_bar_border

			Rectangle {
				width: parent.width
				height: parent.height

				radius: Style.dimens.button_radius
				color: progressBar.backgroundColor
			}

			Rectangle {
				width: parent.width * visualPosition
				height: parent.height

				radius: Style.dimens.button_radius
				color: Constants.green

				Behavior on width { SmoothedAnimation { } }
			}
		}

		GText {
			id: progressText

			anchors {
				left: parent.left
				right: parent.right
				verticalCenter: parent.verticalCenter
			}
			horizontalAlignment: Text.AlignHCenter

			textStyle: Style.text.normal
			elide: Text.ElideMiddle
			maximumLineCount: 1
		}
	}

	FocusFrame {}
}
