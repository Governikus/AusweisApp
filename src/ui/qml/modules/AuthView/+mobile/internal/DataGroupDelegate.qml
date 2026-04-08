/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.View

Loader {
	id: root

	required property int count
	required property int index
	readonly property bool isLast: index === count - 1
	required property var model
	required property string name
	required property bool optional
	required property bool selected

	sourceComponent: optional ? optionalDelegate : requiredDelegate

	onFocusChanged: item.focus = focus // QTBUG-122734

	GSeparator {
		visible: !root.isLast
		z: 1

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
	Component {
		id: optionalDelegate

		GCheckBox {
			id: checkBox

			//: MOBILE
			Accessible.name: qsTr("%1, optional right, element %2 of %3").arg(text).arg(root.index + 1).arg(root.count)
			checked: root.selected
			focusFrameVisible: false
			horizontalPadding: 0
			text: root.name
			verticalPadding: Style.dimens.text_spacing

			background: RoundedRectangle {
				bottomLeftCorner: root.isLast
				bottomRightCorner: root.isLast
				color: checkBox.preferredBackgroundColor
				topLeftCorner: false
				topRightCorner: false
			}

			onCheckedChanged: root.model.selected = checked

			FocusFrame {
				anchors {
					bottomMargin: Style.dimens.border_width * 2
					leftMargin: -Style.dimens.text_spacing
					rightMargin: -Style.dimens.text_spacing
					topMargin: Style.dimens.border_width
				}
			}
		}
	}
	Component {
		id: requiredDelegate

		GText {
			//: MOBILE
			Accessible.name: qsTr("%1, required right, element %2 of %3").arg(text).arg(root.index + 1).arg(root.count)
			bottomPadding: Style.dimens.text_spacing
			text: root.name
			topPadding: Style.dimens.text_spacing

			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
}
