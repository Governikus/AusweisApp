/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.PinResetInformationModel

FlickableSectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	property PasswordInfoData infoContent: PasswordInfoData {
	}
	property alias rootEnabled: titleBarAction.rootEnabled
	readonly property string title: infoContent.title

	signal abortCurrentWorkflow
	signal close

	spacing: Constants.pane_spacing

	titleBarAction: TitleBarAction {
		id: titleBarAction

		rootEnabled: ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE
		showSettings: false
		text: root.title
	}

	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()
	Keys.onReturnPressed: close()

	Repeater {
		model: root.contentList

		delegate: RowLayout {
			Layout.alignment: headline ? Qt.AlignHCenter : Qt.AlignLeft
			spacing: 2 * Constants.component_spacing

			AnimationLoader {
				Layout.alignment: Qt.AlignTop
				animated: false
				type: blockHeaderAnimation
			}
			ColumnLayout {
				Layout.alignment: Qt.AlignTop
				spacing: Constants.groupbox_spacing

				GText {
					Layout.alignment: headline ? Qt.AlignHCenter : Qt.AlignLeft
					activeFocusOnTab: true
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
}
