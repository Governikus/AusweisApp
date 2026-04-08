/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property list<MultiInfoContentBlock> contentList: infoContent.contentList
	property string continueButtonText
	readonly property string hintBoxesTitle: infoContent.hintBoxesTitle
	property MultiInfoData infoContent: MultiInfoData {
	}

	signal abortCurrentWorkflow
	signal continueClicked

	spacing: Style.dimens.pane_spacing
	title: infoContent.title

	GRepeater {
		id: repeater

		autoConsiderItems: false
		model: root.contentList

		delegate: RowLayout {
			id: dataRow

			required property MultiInfoContentBlock modelData

			Layout.alignment: modelData.blockAlignment
			spacing: 2 * Style.dimens.pane_spacing

			AnimationLoader {
				Layout.alignment: Qt.AlignTop
				animated: false
				type: dataRow.modelData.blockHeaderAnimation

				Component.onCompleted: repeater.addConsideredItem(this)
				Component.onDestruction: repeater.removeConsideredItem(this)
			}
			TintableIcon {
				Layout.alignment: Qt.AlignTop
				source: dataRow.modelData.blockImage
				sourceSize.width: Math.max(repeater.maxItemWidth, Style.dimens.header_icon_size)
				tintColor: Style.color.image
				visible: source.toString() !== ""
			}
			ColumnLayout {
				Layout.alignment: Qt.AlignTop
				spacing: Style.dimens.groupbox_spacing

				Subheading {
					text: dataRow.modelData.blockTitle
					visible: text !== ""
				}
				Repeater {
					model: dataRow.modelData.paragraphList

					delegate: GText {
						required property string modelData

						Accessible.role: (textStyle === Style.text.headline || textStyle === Style.text.subline) ? Accessible.Heading : Accessible.StaticText
						Layout.alignment: Qt.AlignLeft
						horizontalAlignment: Text.AlignLeft
						text: modelData
						textStyle: dataRow.modelData.paragraphTextStyle
						visible: text !== ""
					}
				}
			}
		}
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		buttonText: root.infoContent.infoContent.multiInfoHintContent.buttonText
		linkToOpen: root.infoContent.infoContent.multiInfoHintContent.linkToOpen
		text: root.infoContent.infoContent.multiInfoHintContent.text
		title: root.infoContent.infoContent.multiInfoHintContent.title
		visible: text !== ""

		onLinkAboutToOpen: root.abortCurrentWorkflow()
	}
	PinResetHints {
		Layout.fillWidth: true
		title: root.hintBoxesTitle

		onLinkAboutToOpen: root.abortCurrentWorkflow()
	}
	GContinueButton {
		id: continueButton

		text: root.continueButtonText

		onClicked: root.continueClicked()
	}
}
