/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	id: root

	Accessible.role: Accessible.Heading
	Accessible.name: titleText.text

	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	property alias checkBox: titleText.checkBox
	property alias content: paneContent
	default property alias children: paneContent.children

	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	Keys.onPressed: if (event.key === Qt.Key_Space) checkBox.toggle()

	Item {
		id: shadowLayer

		anchors.fill: parent

		layer.enabled: true
		layer.effect: DropShadow {
			radius: 8
			samples: 8
			source: background
			color: Qt.darker(Constants.grey, 1.2)
			verticalOffset: 2
		}
	}

	Rectangle {
		id: background

		anchors.fill: parent

		color: Style.color.background_pane
		radius: Style.dimens.corner_radius
	}

	Column {
		id: containerCol

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding
		topPadding: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing

		PaneTitle {
			id: titleText

			width: Math.max(implicitWidth + 2 * Constants.pane_padding, parent.width)

			FocusFrame {
				scope: root
				dynamic: false
				border.color: Constants.black
			}
		}

		Column {
			id: paneContent

			width: parent.width
			spacing: Constants.pane_spacing
		}
	}

}
