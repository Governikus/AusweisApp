/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.PinResetInformationModel

SectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	property var infoContent: PasswordInfoContent {
	}
	property alias rootEnabled: titleBarAction.rootEnabled
	readonly property string title: infoContent.title

	signal abortCurrentWorkflow
	signal close

	titleBarAction: TitleBarAction {
		id: titleBarAction

		rootEnabled: ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE
		showHelp: false
		showSettings: false
		text: root.title
	}

	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()
	Keys.onReturnPressed: close()

	GFlickableColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.pane_spacing

		Repeater {
			model: root.contentList

			delegate: RowLayout {
				Layout.alignment: headline ? Qt.AlignHCenter : Qt.AlignLeft
				spacing: 2 * Constants.component_spacing

				PasswordInfoImage {
					Layout.alignment: Qt.AlignTop
					imageType: blockHeaderImageType
					scaleFactorGeneral: plugin.scaleFactor
					visible: blockHeaderImageType !== PasswordInfoImage.Type.NONE
				}
				ColumnLayout {
					Layout.alignment: Qt.AlignTop
					spacing: Constants.groupbox_spacing

					GText {
						Layout.alignment: headline ? Qt.AlignHCenter : Qt.AlignLeft
						activeFocusOnTab: true
						font.bold: true
						horizontalAlignment: headline ? Text.AlignHCenter : Text.AlignLeft
						text: blockTitle
						textStyle: headline ? Style.text.headline : Style.text.subline
						visible: text !== ""

						FocusFrame {
						}
					}
					Repeater {
						model: paragraphList

						delegate: GText {
							Layout.alignment: headline ? Qt.AlignHCenter : Qt.AlignLeft
							activeFocusOnTab: true
							horizontalAlignment: headline ? Text.AlignHCenter : Text.AlignLeft
							text: modelData

							FocusFrame {
							}
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
		GSpacer {
			Layout.fillHeight: true
		}
		Hint {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			buttonText: root.hintButtonText
			buttonTooltip: PinResetInformationModel.pinResetUrl
			text: root.hint
			visible: text !== ""

			onClicked: {
				abortCurrentWorkflow();
				Qt.openUrlExternally(PinResetInformationModel.pinResetUrl);
			}
		}
		NavigationButton {
			Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
			buttonType: NavigationButton.Type.Back

			onClicked: root.close()
		}
	}
}
