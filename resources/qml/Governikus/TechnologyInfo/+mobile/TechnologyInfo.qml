/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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
		verticalAlignment: Text.AlignTop
		visible: text !== ""
	}
	GText {
		id: enableInfo

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.normal_warning
		verticalAlignment: Text.AlignBottom
		visible: text !== ""
	}
	Column {
		id: additionalContentItem

		Layout.fillWidth: true
		visible: children.length !== 0
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
