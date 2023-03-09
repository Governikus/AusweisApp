/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

ProgressBar {
	id: progressBar

	property color backgroundColor: Constants.white
	property alias text: progressText.text

	Accessible.name: qsTr("%1 percent done").arg(value)
	Accessible.role: Accessible.ProgressBar
	from: 0
	to: 100

	background: Rectangle {
		border.color: Style.color.border
		border.width: Style.dimens.progress_bar_border
		color: Style.color.border
		radius: Style.dimens.button_radius
	}
	contentItem: Item {
		implicitHeight: Style.dimens.progress_bar_height

		Item {
			anchors.fill: parent
			anchors.margins: Style.dimens.progress_bar_border

			Rectangle {
				color: progressBar.backgroundColor
				height: parent.height
				radius: Style.dimens.button_radius
				width: parent.width
			}
			Rectangle {
				color: Constants.green
				height: parent.height
				radius: Style.dimens.button_radius
				width: parent.width * visualPosition

				Behavior on width  {
					SmoothedAnimation {
					}
				}
			}
		}
		GText {
			id: progressText
			elide: Text.ElideMiddle
			horizontalAlignment: Text.AlignHCenter
			maximumLineCount: 1
			textStyle: Style.text.normal

			anchors {
				left: parent.left
				right: parent.right
				verticalCenter: parent.verticalCenter
			}
		}
	}

	FocusFrame {
	}
}
