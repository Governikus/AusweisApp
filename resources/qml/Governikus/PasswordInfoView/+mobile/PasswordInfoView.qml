/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type.PinResetInformationModel

FlickableSectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	property var infoContent: PasswordInfoContent {
	}
	readonly property string infoContentTitle: infoContent.title

	signal abortCurrentWorkflow
	signal close

	spacing: Constants.component_spacing
	title: infoContentTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: close()
	}

	Column {
		Layout.fillWidth: true
		Layout.margins: Constants.pane_padding
		spacing: Constants.component_spacing

		Repeater {
			model: root.contentList
			width: parent.width

			delegate: Column {
				spacing: Constants.component_spacing
				width: parent.width

				PasswordInfoImage {
					anchors.horizontalCenter: parent.horizontalCenter
					imageType: blockHeaderImageType
					visible: blockHeaderImageType !== PasswordInfoImage.Type.NONE
					width: parent.width * 0.3
				}
				Column {
					spacing: Constants.groupbox_spacing
					width: parent.width

					GText {
						font.bold: true
						text: blockTitle
						textStyle: headline ? Style.text.headline : Style.text.subline
						width: parent.width
						wrapMode: Text.WordWrap
					}
					Repeater {
						model: paragraphList
						width: parent.width

						delegate: GText {
							text: modelData
							width: parent.width
						}
					}
				}
			}
		}
		GButton {
			Layout.alignment: Qt.AlignRight
			Layout.topMargin: Constants.component_spacing
			icon.source: "qrc:///images/open_website.svg"
			text: root.buttonText
			visible: text !== ""

			onClicked: Qt.openUrlExternally(root.buttonLink)
		}
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: Constants.component_spacing
		buttonText: root.hintButtonText
		text: root.hint
		visible: text !== ""

		onClicked: {
			abortCurrentWorkflow();
			Qt.openUrlExternally(PinResetInformationModel.pinResetUrl);
		}
	}
}
