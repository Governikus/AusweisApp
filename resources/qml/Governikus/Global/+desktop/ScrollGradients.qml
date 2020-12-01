/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	id: baseItem

	property real leftMargin: Constants.pane_padding
	property real rightMargin: Constants.pane_padding
	property color color

	visible: !Style.currentTheme.highContrast

	Rectangle {
		height: Constants.pane_padding
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			leftMargin: leftMargin
			rightMargin: rightMargin
		}

		gradient: Gradient {
			GradientStop { position: 0.0; color: baseItem.color }
			GradientStop { position: 1.0; color: Style.color.transparent }
		}
	}

	Rectangle {
		height: Constants.pane_padding
		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			leftMargin: leftMargin
			rightMargin: rightMargin
		}

		gradient: Gradient {
			GradientStop { position: 0.0; color: Style.color.transparent }
			GradientStop { position: 1.0; color: baseItem.color }
		}
	}
}
