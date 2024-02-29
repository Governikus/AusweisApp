/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	required property string name

	//: LABEL DESKTOP
	Accessible.description: Constants.is_desktop ? qsTr("Show more information about the service provider") : ""
	Accessible.name: label.label + ". " + label.text + (Constants.is_desktop ? "" : (". " + link.text))
	Accessible.role: Accessible.Button
	padding: Constants.pane_padding

	background: GPaneBackground {
		id: background

		Accessible.ignored: true

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignTop
			source: "qrc:///images/info.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: label.labelColor
		}
		ColumnLayout {
			spacing: Constants.component_spacing

			LabeledText {
				id: label

				Accessible.ignored: true
				Layout.fillWidth: true
				activeFocusOnTab: false
				bodyElide: Text.ElideRight
				label: Constants.is_desktop ?
				//: LABEL DESKTOP
				qsTr("You are about to identify yourself towards the following provider:") :
				//: LABEL ANDROID IOS
				qsTr("Provider")
				maximumBodyLineCount: 1

				//: LABEL ALL_PLATFORMS
				text: name.length > 0 ? name : qsTr("See details under \"more...\"")
			}
			GText {
				id: link

				Accessible.ignored: true
				color: label.labelColor
				font.pixelSize: plugin.scaleFactor * 24
				text: Constants.is_desktop ?
				//: LABEL DESKTOP
				qsTr("Details about the provider") :
				//: LABEL ANDROID IOS
				qsTr("Touch for more details")
			}
		}
		TintableIcon {
			Layout.alignment: Qt.AlignVCenter
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: label.textColor
		}
	}

	Item {
		id: d

		states: [
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					background.border.color: Style.color.pane_border_pressed
					background.color: Style.color.pane_pressed
					label.labelColor: Style.color.text_subline_pressed
					label.textColor: Style.color.text_pressed
				}
			},
			State {
				name: "hovered"
				when: root.hovered

				PropertyChanges {
					background.border.color: Style.color.pane_border_hovered
					background.color: Style.color.pane_hovered
					label.labelColor: Style.color.text_subline_hovered
					label.textColor: Style.color.text_hovered
				}
			}
		]
		transitions: [
			EaseInPressedTransition {
				targets: [background, label]
			}
		]
	}
}
