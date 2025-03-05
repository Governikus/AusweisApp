/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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

	signal continueClicked

	Keys.onEnterPressed: root.continueClicked()
	Keys.onEscapePressed: root.leaveView()
	Keys.onReturnPressed: root.continueClicked()

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("6-digit card PIN")
		textStyle: Style.text.headline
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		animated: false
		type: AnimationLoader.PIN
	}
	GText {
		//: LABEL ALL_PLATFORMS
		text: qsTr("Have your documents ready")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS
		text: qsTr("You need:")
	}
	BulletPointText {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: LABEL ALL_PLATFORMS The text between %1 and %2 will be emphasized.
		text: qsTr("Your %1ID card%2").arg("<b>").arg("</b>")
	}
	GText {
		Layout.bottomMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS
		text: qsTr("And either:")
	}
	BulletPointText {
		Layout.bottomMargin: Style.dimens.text_spacing

		//: LABEL ALL_PLATFORMS The text between %1 and %2 will be emphasized.
		text: qsTr("Your %1card PIN%2 (self-chosen)").arg("<b>").arg("</b>")
	}
	GText {
		Layout.bottomMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS
		text: qsTr("or:")
	}
	BulletPointText {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: LABEL ALL_PLATFORMS
		text: qsTr("Letter from the PIN Reset Service containing the 6-digit PIN")
	}
	GContinueButton {
		onClicked: root.continueClicked()
	}
}
