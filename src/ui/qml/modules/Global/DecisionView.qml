/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property alias agreeButton: agreeButton
	property alias agreeButtonColor: agreeButton.buttonColor
	property alias agreeButtonIcon: agreeButton.icon.source
	property alias agreeButtonText: agreeButton.text
	property alias customContentSourceComponent: customContentLoader.sourceComponent
	property alias descriptionTextsModel: descriptionTexts.model
	readonly property alias disagreeButton: disagreeButton
	property bool disagreeButtonHighlighted: true
	property alias disagreeButtonText: disagreeButton.text
	property alias headlineText: headline.text
	property alias iconSourceComponent: iconLoader.sourceComponent
	property alias subtitleText: subtitle.text
	property alias tintAgreeButtonIcon: agreeButton.tintIcon

	signal agreeClicked
	signal disagreeClicked

	QtObject {
		id: d

		readonly property real buttonWidth: Math.max(agreeButton.implicitWidth, disagreeButton.implicitWidth)
	}
	GText {
		id: headline

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: headline.text !== ""
		wrapMode: Text.WordWrap
	}
	Loader {
		id: iconLoader

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
	}
	GText {
		id: subtitle

		Layout.topMargin: Style.dimens.pane_spacing
		textStyle: Style.text.subline
	}
	Repeater {
		id: descriptionTexts

		delegate: GText {
			required property string modelData

			Layout.topMargin: Style.dimens.text_spacing
			text: modelData
		}
	}
	Loader {
		id: customContentLoader

		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		visible: sourceComponent !== undefined
	}
	GButton {
		id: agreeButton

		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonWidth
		Layout.preferredWidth: d.buttonWidth
		Layout.topMargin: Style.dimens.pane_spacing
		style: root.disagreeButtonHighlighted ? Style.color.controlOptional : Style.color.control
		visible: text !== ""

		onClicked: root.agreeClicked()
	}
	GButton {
		id: disagreeButton

		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonWidth
		Layout.preferredWidth: d.buttonWidth
		Layout.topMargin: Style.dimens.text_spacing
		style: root.disagreeButtonHighlighted ? Style.color.control : Style.color.controlOptional
		visible: text !== ""

		onClicked: root.disagreeClicked()
	}
}
