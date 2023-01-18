/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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

	contentItem: RowLayout {
		spacing: root.spacing

		Rectangle {
			Layout.preferredHeight: root.indicatorHeight
			Layout.preferredWidth: root.indicatorHeight
			border.color: Style.color.accent
			border.width: Math.max(ApplicationModel.scaleFactor * 3, 1)
			radius: height / 2

			Rectangle {
				anchors.alignWhenCentered: false
				anchors.centerIn: parent
				color: Style.color.accent
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
			visible: source != ""
		}
		GText {
			Accessible.ignored: true
			Layout.fillWidth: true
			text: root.text
			textStyle: root.textStyle
		}
	}
	indicator: Item {
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
