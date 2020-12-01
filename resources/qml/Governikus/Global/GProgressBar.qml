/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

ProgressBar {
	id: progressBar

	property alias text: progressText.text

	Accessible.role: Accessible.ProgressBar
	Accessible.name: qsTr("%1 percent done").arg(value)

	from: 0
	to: 100

	background: Rectangle {
		radius: Style.dimens.button_radius
		color: Constants.lightgrey
	}

	contentItem: Item {
		Rectangle {
			width: progressBar.visualPosition * parent.width
			height: parent.height
			radius: Style.dimens.button_radius
			color: Constants.green
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
