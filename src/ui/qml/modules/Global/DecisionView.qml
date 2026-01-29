/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	property alias customContentSourceComponent: customContentLoader.sourceComponent
	property alias descriptionTextsModel: descriptionTexts.model
	property alias headlineText: headline.text
	property alias iconSourceComponent: iconLoader.sourceComponent
	property alias primaryButton: primaryButton
	property alias secondaryButton: secondaryButton
	property alias subtitleText: subtitle.text

	signal primaryButtonClicked
	signal secondaryButtonClicked

	QtObject {
		id: d

		readonly property real buttonWidth: Math.max(primaryButton.implicitWidth, secondaryButton.implicitWidth)
	}
	Heading {
		id: headline

		visible: root.headlineText !== ""
		wrapMode: Text.WordWrap
	}
	Loader {
		id: iconLoader

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
	}
	Subheading {
		id: subtitle

		Layout.topMargin: Style.dimens.pane_spacing
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
		visible: status === Loader.Ready
	}
	GButton {
		id: secondaryButton

		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonWidth
		Layout.preferredWidth: d.buttonWidth
		Layout.topMargin: Style.dimens.pane_spacing
		style: Style.color.controlOptional
		visible: text !== ""

		onClicked: root.secondaryButtonClicked()
	}
	GButton {
		id: primaryButton

		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: d.buttonWidth
		Layout.preferredWidth: d.buttonWidth
		Layout.topMargin: secondaryButton.visible ? Style.dimens.text_spacing : Style.dimens.pane_spacing
		visible: text !== ""

		onClicked: root.primaryButtonClicked()
	}
}
