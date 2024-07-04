/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View
import Governikus.Type

RoundedRectangle {
	property int iconHeight: UiPluginModel.scaleFactor * 175

	//: INFO DESKTOP Text read by screen reader if the text contains a web link to a card reader driver which may be opened.
	Accessible.name: readerName + ". " + ApplicationModel.stripHtmlTags(readerHTMLDescription) + ". " + (textDescription.hasLink ? qsTr("Press space to open link.") : "")
	Accessible.role: textDescription.hasLink ? Accessible.Button : Accessible.ListItem
	activeFocusOnTab: true
	color: Style.color.paneSublevel.background.basic
	implicitHeight: rowLayout.implicitHeight
	implicitWidth: rowLayout.implicitWidth

	Keys.onSpacePressed: textDescription.tryActivateLink()

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
					statusIcon.source: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
					statusIcon.tintColor: Style.color.success
				}
			},
			State {
				name: "WARNING"

				PropertyChanges {
					statusIcon.source: "qrc:///images/status_warning.svg"
					statusIcon.tintColor: Style.color.fail
				}
			},
			State {
				name: "ERROR"

				PropertyChanges {
					statusIcon.source: "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name)
					statusIcon.tintColor: Style.color.warning
				}
			}
		]

		RoundedRectangle {
			Layout.fillHeight: true
			Layout.preferredHeight: iconHeight + 2 * Constants.pane_padding
			Layout.preferredWidth: iconHeight + Constants.pane_padding
			bottomRightCorner: false
			color: Style.color.paneSublevel.background.basic
			gradientColor: Style.color.pane.background.basic
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
				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				clip: true
				text: readerName
				textStyle: Style.text.headline
			}
			GText {
				id: textDescription

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
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
