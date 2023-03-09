/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.PinResetInformationModel 1.0

SectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property int imageType: infoContent.imageType
	property var infoContent: PasswordInfoContent {
	}
	readonly property string infoContentTitle: infoContent.title

	signal close

	sectionPageFlickable: contentItem
	title: infoContentTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: close()
	}

	GFlickableColumnLayout {
		id: contentItem
		anchors.fill: parent
		fillHeight: false
		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.component_spacing

		PasswordInfoImage {
			Layout.fillWidth: true
			Layout.topMargin: Constants.pane_padding
			imageType: root.imageType
			visible: root.imageType !== PasswordInfoImage.Type.NONE
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width

			Column {
				spacing: Constants.component_spacing
				width: parent.width

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
								text: blockTitle
								textStyle: Style.text.header_secondary_highlight
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
					icon.source: "qrc:///images/material_open_in_new.svg"
					text: root.buttonText
					visible: text !== ""

					onClicked: Qt.openUrlExternally(root.buttonLink)
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		Hint {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			buttonText: PinResetInformationModel.pinResetActionText
			text: root.hint
			visible: text !== ""

			onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		}
	}
}
