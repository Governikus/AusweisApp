/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	property int iconHeight: ApplicationModel.scaleFactor * 175

	implicitHeight: rowLayout.implicitHeight
	activeFocusOnTab: true

	Accessible.role: Accessible.Button
	Accessible.name: readerName + ". " + readerHTMLDescription

	FocusFrame {}

	RowLayout {
		id: rowLayout

		width: parent.width

		spacing: 0

		states: [
			State {
				name: "OK"
				PropertyChanges {target: statusIcon; source: "qrc:///images/status_ok.svg"}
				PropertyChanges {target: statusIcon; tintColor: Style.color.success}
				PropertyChanges {target: textDescription; font.bold: false}
			},
			State {
				name: "WARNING"
				PropertyChanges {target: statusIcon; source: "qrc:///images/material_alert.svg"}
				PropertyChanges {target: statusIcon; tintColor: "#e68a00"}
				PropertyChanges {target: textDescription; font.bold: true}
			},
			State {
				name: "ERROR"
				PropertyChanges {target: statusIcon; source: "qrc:///images/status_error.svg"}
				PropertyChanges {target: statusIcon; tintColor: Style.color.warning_text}
				PropertyChanges {target: textDescription; font.bold: false}
			}
		]

		state: {
			if (readerInstalled) {
				if (readerSupported) {
					return "OK"
				}
				return "WARNING"
			}
			return "ERROR"
		}

		Rectangle {
			Layout.preferredWidth: iconHeight
			Layout.preferredHeight: iconHeight

			border {
				color: Style.color.border
				width: Style.dimens.separator_size
			}

			Image {
				id: readerIcon

				anchors.fill: parent
				anchors.margins: iconHeight * 0.05

				asynchronous: true
				source: readerImagePath
				fillMode: Image.PreserveAspectFit
			}
		}

		GSpacer {
			width: Constants.component_spacing
			height: Constants.component_spacing
		}

		ColumnLayout {
			id: textColumn

			Layout.fillHeight: true
			Layout.alignment: Qt.AlignLeft
			spacing: Constants.text_spacing

			GText {
				Layout.fillWidth: true

				textStyle: Style.text.header
				text: readerName
			}

			GText {
				id: textDescription

				Layout.fillWidth: true

				activeFocusOnTab: true
				Accessible.description: qsTr("Press space to open the link in your browser")

				textStyle: Style.text.normal
				text: readerHTMLDescription

				FocusFrame {}
			}
		}

		TintableIcon {
			id: statusIcon

			sourceSize.height: iconHeight * 0.33
		}
	}
}
