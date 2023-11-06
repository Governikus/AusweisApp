/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

RadioButton {
	id: root

	readonly property int indicatorHeight: Math.ceil((textStyle.textSize + horizontalPadding) / 2) * 2
	property var textStyle: Style.text.normal
	property bool tintIcon: false

	Accessible.name: text
	Layout.maximumWidth: contentItem ? contentItem.implicitWidth + leftPadding + rightPadding : -1
	indicator: null
	spacing: Constants.groupbox_spacing

	contentItem: RowLayout {
		spacing: root.spacing

		Rectangle {
			Layout.preferredHeight: root.indicatorHeight
			Layout.preferredWidth: root.indicatorHeight
			border.color: Style.color.control
			border.width: Math.max(plugin.scaleFactor * 3, 1)
			radius: height / 2

			Rectangle {
				anchors.alignWhenCentered: false
				anchors.centerIn: parent
				color: Style.color.control
				height: parent.height / 2
				radius: height / 2
				visible: root.checked
				width: height
			}
		}
		TintableIcon {
			Layout.preferredHeight: sourceSize.height
			source: root.icon.source
			sourceSize.height: root.indicatorHeight
			tintColor: root.textStyle.textColor
			tintEnabled: tintIcon
			visible: source.toString() !== ""
		}
		GText {
			Accessible.ignored: true
			text: root.text
			textStyle: root.textStyle
		}
	}

	FocusFrame {
		scope: root
	}
	MouseArea {
		acceptedButtons: Qt.NoButton
		anchors.fill: parent
		cursorShape: root.checked ? Qt.ArrowCursor : Qt.PointingHandCursor
	}
}
