/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.TitleBar

FlickableSectionPage {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property var contentList: infoContent.contentList
	property string continueButtonText
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	readonly property string hintTitle: infoContent.hintTitle
	property MultiInfoData infoContent: MultiInfoData {
	}
	readonly property string infoContentTitle: infoContent.title

	signal abortCurrentWorkflow
	signal close
	signal continueClicked

	spacing: Style.dimens.pane_spacing
	title: infoContentTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.close()
	}

	Column {
		Layout.fillWidth: true
		Layout.margins: Style.dimens.pane_padding
		spacing: Style.dimens.pane_spacing

		GRepeater {
			id: repeater

			autoConsiderItems: false
			model: root.contentList
			width: parent.width

			delegate: Column {
				id: blockDelegate

				required property int blockHeaderAnimation
				required property url blockImage
				required property string blockTitle
				required property var paragraphList
				required property var paragraphTextStyle

				spacing: Style.dimens.pane_spacing
				width: parent.width

				Column {
					spacing: Style.dimens.groupbox_spacing
					width: parent.width

					GText {
						text: blockDelegate.blockTitle
						textStyle: Style.text.subline
						visible: text !== ""
						width: parent.width
						wrapMode: Text.WordWrap
					}
					Repeater {
						model: blockDelegate.paragraphList
						width: parent.width

						delegate: GText {
							required property string modelData

							text: modelData
							textStyle: blockDelegate.paragraphTextStyle
							visible: text !== ""
							width: parent.width
						}
					}
				}
				AnimationLoader {
					anchors.horizontalCenter: parent.horizontalCenter
					animated: false
					type: blockDelegate.blockHeaderAnimation

					Component.onCompleted: repeater.addConsideredItem(this)
					Component.onDestruction: repeater.removeConsideredItem(this)
				}
				TintableIcon {
					anchors.horizontalCenter: parent.horizontalCenter
					source: blockDelegate.blockImage
					sourceSize.width: Math.max(repeater.maxItemWidth, Style.dimens.header_icon_size)
					tintColor: Style.color.image
				}
			}
		}
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		buttonText: root.hintButtonText
		text: root.hint
		//: LABEL ANDROID IOS
		title: root.hintTitle !== "" ? root.hintTitle : qsTr("Hint")
		visible: text !== ""

		onClicked: {
			root.abortCurrentWorkflow();
			Qt.openUrlExternally(root.buttonLink);
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GContinueButton {
		text: root.continueButtonText

		onClicked: root.continueClicked()
	}
}
