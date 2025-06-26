/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
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

	readonly property url buttonLink: infoContent.buttonLink
	readonly property list<MultiInfoContentBlock> contentList: infoContent.contentList
	property string continueButtonText
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	readonly property string hintTitle: infoContent.hintTitle
	property MultiInfoData infoContent: MultiInfoData {
	}

	signal abortCurrentWorkflow
	signal continueClicked

	function continueOrLeave() {
		if (continueButton.visible) {
			root.continueClicked();
		} else {
			root.leaveView();
		}
	}

	spacing: Style.dimens.pane_spacing
	title: infoContent.title

	Keys.onEnterPressed: root.continueOrLeave()
	Keys.onEscapePressed: root.leaveView()
	Keys.onReturnPressed: root.continueOrLeave()

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
				visible: source != ""
			}
			ColumnLayout {
				Layout.alignment: Qt.AlignTop
				spacing: Style.dimens.groupbox_spacing

				GText {
					Layout.alignment: Qt.AlignLeft
					horizontalAlignment: Text.AlignLeft
					text: dataRow.modelData.blockTitle
					textStyle: Style.text.subline
					visible: text !== ""
				}
				Repeater {
					model: dataRow.modelData.paragraphList

					delegate: GText {
						required property string modelData

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
	GSpacer {
		Layout.fillHeight: true
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		buttonText: root.hintButtonText
		buttonTooltip: root.buttonLink
		text: root.hint
		//: LABEL DESKTOP
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
		id: continueButton

		text: root.continueButtonText

		onClicked: root.continueClicked()
	}
}
