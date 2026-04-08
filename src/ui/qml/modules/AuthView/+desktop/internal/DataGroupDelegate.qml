/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.View

Item {
	id: root

	required property int count
	required property int index
	required property var model
	required property string name
	required property bool optional
	required property bool selected

	implicitHeight: fontMetrics.height * 2

	GSeparator {
		anchors.bottom: parent.bottom
		visible: root.index < root.count - 2
		width: parent.width
	}
	FontMetrics {
		id: fontMetrics

		font.pixelSize: Style.dimens.text
	}
	GText {
		//: DESKTOP
		Accessible.name: qsTr("%1, required right, element %2 of %3").arg(text).arg(root.index + 1).arg(root.count)
		anchors.fill: parent
		focusFrameVisible: false
		text: root.name
		visible: !root.optional

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
	GCheckBox {
		//: DESKTOP
		Accessible.name: qsTr("%1, optional right, element %2 of %3").arg(text).arg(root.index + 1).arg(root.count)
		anchors.fill: parent
		checked: root.selected
		focusFrameVisible: false
		horizontalPadding: 0
		text: root.name
		verticalPadding: 0
		visible: root.optional

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
