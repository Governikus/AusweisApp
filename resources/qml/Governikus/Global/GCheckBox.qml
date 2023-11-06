/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

CheckBox {
	id: control

	property alias descriptionItem: descriptionItemLoader.sourceComponent
	property alias maximumLineCount: description.maximumLineCount
	property alias textStyle: description.textStyle

	padding: 0

	contentItem: ColumnLayout {
		spacing: Constants.text_spacing

		GText {
			id: description

			Accessible.ignored: true
			elide: Text.ElideRight
			leftPadding: control.indicator.implicitWidth + control.spacing
			maximumLineCount: 1
			text: control.text
			textStyle: enabled ? Style.text.normal : Style.text.normal
		}
		Loader {
			id: descriptionItemLoader

			Layout.fillWidth: true
			Layout.leftMargin: description.leftPadding
			visible: sourceComponent
		}
	}
	indicator: Rectangle {
		border.color: enabled ? Style.color.control : Style.color.control_disabled
		border.width: Math.max(plugin.scaleFactor * 4, 1)
		color: enabled ? (control.checked ? Style.color.control : Style.color.transparent) : Style.color.control_disabled
		implicitHeight: plugin.scaleFactor * 33
		implicitWidth: implicitHeight
		radius: Math.max(plugin.scaleFactor * 4, 1)

		TintableIcon {
			anchors.fill: parent
			anchors.margins: Math.max(plugin.scaleFactor * 4, 1)
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/checkbox_indicator.svg"
			tintColor: Style.color.control_content
			visible: control.checked
		}
	}

	Accessible.onPressAction: {
		toggle();
		toggled();
	}

	MouseArea {
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onClicked: control.toggle()
	}
	FocusFrame {
		anchors.fill: null
		height: indicator.height - 2 * anchors.margins
		width: description.x + description.width - 2 * anchors.margins
		x: indicator.x + anchors.margins
		y: indicator.y + anchors.margins
	}
}
