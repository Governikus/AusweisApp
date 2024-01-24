/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View

ProgressBar {
	id: progressBar

	property color backgroundColor: Style.color.background
	property alias text: progressText.text

	Accessible.name: qsTr("%1 percent done").arg(value)
	Accessible.role: Accessible.ProgressBar
	from: 0
	to: 100

	background: Rectangle {
		border.color: Style.color.control_border
		border.width: Style.dimens.progress_bar_border
		color: Style.color.transparent
		radius: height / 2
	}
	contentItem: Item {
		implicitHeight: Style.dimens.progress_bar_height

		Item {
			anchors.fill: parent
			anchors.margins: Style.dimens.progress_bar_border * 3

			Rectangle {
				color: progressBar.backgroundColor
				height: parent.height
				radius: height / 2
				width: parent.width
			}
			Rectangle {
				property real mutableVisualPosition: visualPosition

				color: Style.color.control
				height: parent.height
				radius: height / 2
				width: parent.width * mutableVisualPosition

				Behavior on mutableVisualPosition {
					SmoothedAnimation {
						velocity: 0.5
					}
				}
			}
		}
		GText {
			id: progressText

			elide: Text.ElideMiddle
			horizontalAlignment: Text.AlignHCenter
			maximumLineCount: 1

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
