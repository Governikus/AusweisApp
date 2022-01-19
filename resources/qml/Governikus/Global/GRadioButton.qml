/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

RadioButton {
	id: root

	readonly property int indicatorHeight: Math.ceil((textStyle.textSize + horizontalPadding) / 2) * 2
	property var textStyle: Style.text.normal
	property bool tintIcon: false

	Accessible.name: text

	spacing: Constants.groupbox_spacing
	indicator: Item {}
	contentItem: RowLayout {
		spacing: root.spacing

		Rectangle {
			Layout.preferredHeight: root.indicatorHeight
			Layout.preferredWidth: root.indicatorHeight

			radius: height / 2
			border.color: Style.color.accent
			border.width: Math.max(ApplicationModel.scaleFactor * 3, 1)

			Rectangle {
				visible: root.checked
				anchors.centerIn: parent
				anchors.alignWhenCentered: false
				height: parent.height / 2
				width: height

				radius: height / 2
				color: Style.color.accent
			}
		}

		TintableIcon {
			visible: source != ""
			Layout.preferredHeight: sourceSize.height

			source: root.icon.source
			sourceSize.height: root.indicatorHeight

			tintEnabled: tintIcon
			tintColor: root.textStyle.textColor
		}

		GText {
			Layout.fillWidth: true

			Accessible.ignored: true

			text: root.text
			textStyle: root.textStyle

		}
	}

	FocusFrame {
		borderColor: Style.color.focus_indicator
		scope: root
	}

	MouseArea {
		anchors.fill: parent

		cursorShape: root.checked ? Qt.ArrowCursor : Qt.PointingHandCursor
		acceptedButtons: Qt.NoButton
	}
}
