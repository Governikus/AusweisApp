/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	property alias moreInformationText: moreInformationLink.text

	signal cancel
	signal pinKnown
	signal pinUnknown
	signal showInfoView

	margins: Constants.pane_padding * 2
	spacing: Constants.component_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancel()
	}

	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/info.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
	}
	PaneTitle {
		Layout.alignment: Qt.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Do you know your 6-digit ID card PIN?")
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Online identification with Transport PIN is not possible. The self-selected, 6-digit ID card PIN is mandatory to use the eID function.")
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("To set up a Smart-eID you also need to have assigned a 6-digit PIN beforehand.")
		visible: ApplicationModel.isSmartSupported
	}
	MoreInformationLink {
		id: moreInformationLink

		Layout.alignment: Qt.AlignCenter
		Layout.topMargin: Constants.component_spacing

		onClicked: root.showInfoView()
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GButton {
			Layout.alignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width / 2
			Layout.preferredWidth: Style.dimens.max_text_width / 2

			//: LABEL ANDROID IOS
			text: qsTr("No")

			onClicked: pinUnknown()
			onFocusChanged: if (focus)
				root.positionViewAtItem(this)
		}
		GButton {
			Layout.alignment: Qt.AlignVCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width / 2
			Layout.preferredWidth: Style.dimens.max_text_width / 2

			//: LABEL ANDROID IOS
			text: qsTr("Yes")

			onClicked: pinKnown()
			onFocusChanged: if (focus)
				root.positionViewAtItem(this)
		}
	}
}
