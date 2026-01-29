/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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

	readonly property var contentList: infoContent.contentList
	property string continueButtonText
	readonly property string firstHint: infoContent.firstHint
	readonly property url firstHintButtonLink: infoContent.firstHintButtonLink
	readonly property string firstHintButtonText: infoContent.firstHintButtonText
	readonly property string firstHintTitle: infoContent.firstHintTitle
	readonly property string hintBoxesTitle: infoContent.hintBoxesTitle
	property MultiInfoData infoContent: MultiInfoData {
	}
	readonly property string infoContentTitle: infoContent.title
	readonly property url secondHintButtonLink: infoContent.secondHintButtonLink
	readonly property string secondHintButtonText: infoContent.secondHintButtonText
	readonly property string secondHintText: infoContent.secondHintText
	readonly property string secondHintTitle: infoContent.secondHintTitle

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
		Layout.leftMargin: Style.dimens.pane_padding
		Layout.rightMargin: Style.dimens.pane_padding
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

					Subheading {
						text: blockDelegate.blockTitle
						visible: text !== ""
						width: parent.width
						wrapMode: Text.WordWrap
					}
					Repeater {
						model: blockDelegate.paragraphList
						width: parent.width

						delegate: GText {
							required property string modelData

							Accessible.role: (textStyle === Style.text.headline || textStyle === Style.text.subline) ? Accessible.Heading : Accessible.StaticText
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
	GText {
		Layout.topMargin: Style.dimens.pane_spacing
		text: root.hintBoxesTitle
		textStyle: Style.text.subline
		visible: text !== ""
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		buttonText: root.firstHintButtonText
		linkToOpen: root.firstHintButtonLink
		text: root.firstHint
		//: MOBILE
		title: root.firstHintTitle !== "" ? root.firstHintTitle : qsTr("Hint")
		visible: text !== ""

		onLinkAboutToOpen: root.abortCurrentWorkflow()
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		buttonText: root.secondHintButtonText
		linkToOpen: root.secondHintButtonLink
		text: root.secondHintText
		//: MOBILE
		title: root.secondHintTitle !== "" ? root.secondHintTitle : qsTr("Hint")
		visible: text !== ""

		onLinkAboutToOpen: root.abortCurrentWorkflow()
	}
	GContinueButton {
		id: continueButton

		text: root.continueButtonText

		onClicked: root.continueClicked()
	}
}
