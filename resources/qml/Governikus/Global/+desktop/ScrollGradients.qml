/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	id: baseItem

	property color color

	Rectangle {
		height: Constants.pane_padding
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			leftMargin: Constants.pane_padding
			rightMargin: Constants.pane_padding
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
			leftMargin: Constants.pane_padding
			rightMargin: Constants.pane_padding
		}

		gradient: Gradient {
			GradientStop { position: 0.0; color: Style.color.transparent }
			GradientStop { position: 1.0; color: baseItem.color }
		}
	}
}
