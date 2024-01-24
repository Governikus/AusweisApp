/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel

MouseArea {
	id: baseItem

	property alias image: icon.source
	property string name: ""
	property alias title: text.label

	Accessible.name: text.Accessible.name
	Accessible.role: Accessible.StaticText
	cursorShape: Qt.PointingHandCursor
	implicitHeight: contentRow.height

	GridLayout {
		id: contentRow

		anchors.verticalCenter: parent.verticalCenter
		columnSpacing: Constants.text_spacing
		columns: 3
		rowSpacing: columnSpacing
		rows: 2
		width: parent.width

		TintableIcon {
			id: icon

			Layout.alignment: Qt.AlignVCenter
			Layout.rightMargin: Constants.groupbox_spacing
			sourceSize.height: Style.dimens.icon_size
			tintColor: Style.text.subline.textColor
			tintEnabled: true
		}
		LabeledText {
			id: text

			Layout.fillWidth: true
			activeFocusOnTab: false
			bodyElide: Text.ElideRight
			maximumBodyLineCount: 1

			//: LABEL DESKTOP
			text: name.length > 0 ? name : qsTr("See details under \"more...\"")
		}
		TintableIcon {
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			Layout.rowSpan: 2
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.dimens.icon_size
			tintColor: Style.color.text
		}
		GText {
			//: LABEL DESKTOP
			Accessible.description: qsTr("Show more information about the service provider")
			Layout.column: 1
			Layout.row: 1
			Layout.topMargin: Constants.component_spacing
			activeFocusOnTab: true
			font.pixelSize: plugin.scaleFactor * 24
			//: LABEL DESKTOP
			text: qsTr("Details about the provider")
			textStyle: Style.text.subline
		}
	}
	FocusFrame {
	}
}
