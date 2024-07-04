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
import Governikus.Type

FlickableSectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	readonly property string hintTitle: infoContent.hintTitle
	property PasswordInfoData infoContent: PasswordInfoData {
	}
	property alias rootEnabled: titleBarAction.rootEnabled
	readonly property string title: infoContent.title

	signal abortCurrentWorkflow
	signal close

	spacing: Constants.pane_spacing

	titleBarAction: TitleBarAction {
		id: titleBarAction

		rootEnabled: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE
		showSettings: false
		text: root.title
	}

	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()
	Keys.onReturnPressed: close()

	GRepeater {
		id: repeater

		autoConsiderItems: false
		model: root.contentList

		delegate: RowLayout {
			Layout.alignment: Qt.AlignLeft
			spacing: 2 * Constants.component_spacing

			AnimationLoader {
				Layout.alignment: Qt.AlignTop
				animated: false
				type: blockHeaderAnimation

				Component.onCompleted: repeater.addConsideredItem(this)
				Component.onDestruction: repeater.removeConsideredItem(this)
			}
			TintableIcon {
				Layout.alignment: Qt.AlignTop
				source: blockImage
				sourceSize.width: Math.max(repeater.maxItemWidth, Style.dimens.header_icon_size)
				tintColor: Style.color.image
				visible: source != ""
			}
			ColumnLayout {
				Layout.alignment: Qt.AlignTop
				spacing: Constants.groupbox_spacing

				GText {
					Layout.alignment: Qt.AlignLeft
					activeFocusOnTab: true
					horizontalAlignment: Text.AlignLeft
					text: blockTitle
					textStyle: Style.text.subline
					visible: text !== ""

					FocusFrame {
					}
				}
				Repeater {
					model: paragraphList

					delegate: GText {
						Layout.alignment: Qt.AlignLeft
						activeFocusOnTab: true
						horizontalAlignment: Text.AlignLeft
						text: modelData
						visible: text !== ""

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
		//: LABEL DESKTOP
		title: root.hintTitle !== "" ? root.hintTitle : qsTr("Hint")
		visible: text !== ""

		onClicked: {
			abortCurrentWorkflow();
			Qt.openUrlExternally(PinResetInformationModel.pinResetUrl);
		}
	}
}
