/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.TitleBar
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

		GRepeater {
			id: repeater

			autoConsiderItems: false
			model: root.contentList
			width: parent.width

			delegate: Column {
				spacing: Constants.component_spacing
				width: parent.width

				Column {
					spacing: Constants.groupbox_spacing
					width: parent.width

					GText {
						text: blockTitle
						textStyle: Style.text.subline
						visible: text !== ""
						width: parent.width
						wrapMode: Text.WordWrap
					}
					Repeater {
						model: paragraphList
						width: parent.width

						delegate: GText {
							text: modelData
							visible: text !== ""
							width: parent.width
						}
					}
				}
				AnimationLoader {
					anchors.horizontalCenter: parent.horizontalCenter
					animated: false
					type: blockHeaderAnimation

					Component.onCompleted: repeater.addConsideredItem(this)
					Component.onDestruction: repeater.removeConsideredItem(this)
				}
				TintableIcon {
					anchors.horizontalCenter: parent.horizontalCenter
					source: blockImage
					sourceSize.width: Math.max(repeater.maxItemWidth, Style.dimens.header_icon_size)
					tintColor: Style.color.image
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
			onFocusChanged: if (focus)
				root.positionViewAtItem(this)
		}
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: Constants.component_spacing
		buttonText: root.hintButtonText
		text: root.hint
		//: LABEL ANDROID IOS
		title: root.hintTitle !== "" ? root.hintTitle : qsTr("Hint")
		visible: text !== ""

		onClicked: {
			abortCurrentWorkflow();
			Qt.openUrlExternally(PinResetInformationModel.pinResetUrl);
		}
		onReceivedFocus: pItem => root.positionViewAtItem(pItem)
	}
}
