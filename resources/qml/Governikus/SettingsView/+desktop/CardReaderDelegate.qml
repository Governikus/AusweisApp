/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

RoundedRectangle {
	property int iconHeight: plugin.scaleFactor * 175

	Accessible.name: readerName + ". " + readerHTMLDescription
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	color: Style.color.pane_sublevel
	implicitHeight: rowLayout.implicitHeight
	implicitWidth: rowLayout.implicitWidth

	FocusFrame {
	}
	RowLayout {
		id: rowLayout

		anchors.fill: parent
		spacing: Constants.component_spacing
		state: {
			if (readerInstalled) {
				if (readerSupported) {
					return "OK";
				}
				return "WARNING";
			}
			return "ERROR";
		}

		states: [
			State {
				name: "OK"

				PropertyChanges {
					source: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: Style.color.success
				}
			},
			State {
				name: "WARNING"

				PropertyChanges {
					source: "qrc:///images/status_warning.svg"
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: Style.color.fail
				}
			},
			State {
				name: "ERROR"

				PropertyChanges {
					source: "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name)
					target: statusIcon
				}
				PropertyChanges {
					target: statusIcon
					tintColor: Style.color.text_warning
				}
			}
		]

		RoundedRectangle {
			Layout.fillHeight: true
			Layout.preferredHeight: iconHeight + 2 * Constants.pane_padding
			Layout.preferredWidth: iconHeight + Constants.pane_padding
			bottomRightCorner: false
			color: Style.color.pane_sublevel
			gradientColor: Style.color.pane
			topRightCorner: false

			Image {
				id: readerIcon

				anchors.centerIn: parent
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				height: iconHeight * 0.95
				source: readerImagePath
				width: iconHeight * 0.95
			}
		}
		ColumnLayout {
			id: textColumn

			Layout.alignment: Qt.AlignLeft
			Layout.bottomMargin: Constants.pane_padding
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: 0
			Layout.preferredWidth: parent.width
			Layout.topMargin: Constants.pane_padding
			spacing: Constants.text_spacing

			GText {
				Layout.alignment: Qt.AlignLeft
				clip: true
				text: readerName
				textStyle: Style.text.headline
			}
			GText {
				id: textDescription

				Accessible.description: qsTr("Press space to open the link in your browser")
				Layout.alignment: Qt.AlignLeft
				activeFocusOnTab: true
				text: readerHTMLDescription

				FocusFrame {
				}
			}
		}
		TintableIcon {
			id: statusIcon

			Layout.rightMargin: Constants.pane_padding
			sourceSize.height: iconHeight * 0.33
		}
	}
}
