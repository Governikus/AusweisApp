/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Item {
	property int iconHeight: ApplicationModel.scaleFactor * 175

	Accessible.name: readerName + ". " + readerHTMLDescription
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	implicitHeight: rowLayout.implicitHeight

	FocusFrame {
	}
	RowLayout {
		id: rowLayout
		spacing: 0
		state: {
			if (readerInstalled) {
				if (readerSupported) {
					return "OK";
				}
				return "WARNING";
			}
			return "ERROR";
		}
		width: parent.width

		states: [
			State {
				name: "OK"

				PropertyChanges {
					source: "qrc:///images/status_ok.svg"
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: Style.color.success
				}
				PropertyChanges {
					font.bold: false
					target: textDescription
				}
			},
			State {
				name: "WARNING"

				PropertyChanges {
					source: "qrc:///images/material_alert.svg"
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: "#e68a00"
				}
				PropertyChanges {
					font.bold: true
					target: textDescription
				}
			},
			State {
				name: "ERROR"

				PropertyChanges {
					source: "qrc:///images/status_error.svg"
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: Style.color.warning_text
				}
				PropertyChanges {
					font.bold: false
					target: textDescription
				}
			}
		]

		Rectangle {
			Layout.preferredHeight: iconHeight
			Layout.preferredWidth: iconHeight

			border {
				color: Style.color.border
				width: Style.dimens.separator_size
			}
			Image {
				id: readerIcon
				anchors.fill: parent
				anchors.margins: iconHeight * 0.05
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				source: readerImagePath
			}
		}
		GSpacer {
			height: Constants.component_spacing
			width: Constants.component_spacing
		}
		ColumnLayout {
			id: textColumn
			Layout.alignment: Qt.AlignLeft
			Layout.fillHeight: true
			spacing: Constants.text_spacing

			GText {
				Layout.fillWidth: true
				text: readerName
				textStyle: Style.text.header
			}
			GText {
				id: textDescription
				Accessible.description: qsTr("Press space to open the link in your browser")
				Layout.fillWidth: true
				activeFocusOnTab: true
				text: readerHTMLDescription
				textStyle: Style.text.normal

				FocusFrame {
				}
			}
		}
		TintableIcon {
			id: statusIcon
			sourceSize.height: iconHeight * 0.33
		}
	}
}
