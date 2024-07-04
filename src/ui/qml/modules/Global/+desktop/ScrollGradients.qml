/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: baseItem

	property color color
	property real leftMargin: Constants.pane_padding
	property real rightMargin: Constants.pane_padding

	visible: !Style.currentTheme.highContrast

	Rectangle {
		height: Constants.pane_padding

		gradient: Gradient {
			GradientStop {
				color: baseItem.color
				position: 0.0
			}
			GradientStop {
				color: Style.color.transparent
				position: 1.0
			}
		}

		anchors {
			left: parent.left
			leftMargin: leftMargin
			right: parent.right
			rightMargin: rightMargin
			top: parent.top
		}
	}
	Rectangle {
		height: Constants.pane_padding

		gradient: Gradient {
			GradientStop {
				color: Style.color.transparent
				position: 0.0
			}
			GradientStop {
				color: baseItem.color
				position: 1.0
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: leftMargin
			right: parent.right
			rightMargin: rightMargin
		}
	}
}
