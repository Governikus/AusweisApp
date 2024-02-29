/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.NumberModel

ColumnLayout {
	id: root

	property alias additionalContent: additionalContentItem.data
	property alias enableButtonText: enableButton.text
	property alias enableText: enableInfo.text
	property alias subTitleText: subTitle.text
	property alias titleText: title.text

	signal enableClicked

	spacing: Constants.component_spacing

	GText {
		id: title

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""
	}
	GText {
		id: subTitle

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""
	}
	GText {
		id: enableInfo

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.normal_warning
		visible: text !== ""
	}
	ColumnLayout {
		id: additionalContentItem

		visible: visibleChildren.length !== 0
	}
	GSpacer {
		Layout.fillHeight: true
		visible: enableButton.visible
	}
	GButton {
		id: enableButton

		Layout.alignment: Qt.AlignHCenter
		visible: text !== ""

		onClicked: enableClicked()
	}
}
