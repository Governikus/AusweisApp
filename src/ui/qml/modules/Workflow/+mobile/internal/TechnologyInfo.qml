/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	default property alias additionalContent: additionalContentItem.data
	property alias enableButtonText: enableButton.text
	property alias infoText: info.text
	property alias progressIndicator: progressIndicatorLoader.sourceComponent
	property alias showAdditionalContent: additionalContentItem.visible
	property alias subTitleText: subTitle.text
	property alias titleText: title.text

	signal enableClicked

	Layout.leftMargin: Style.dimens.pane_padding
	Layout.rightMargin: Style.dimens.pane_padding
	spacing: Style.dimens.pane_spacing

	GText {
		id: title

		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""
	}
	Loader {
		id: progressIndicatorLoader

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: -root.spacing
		Layout.maximumHeight: item ? (item as Item).Layout.maximumHeight : -1
		Layout.minimumHeight: item ? (item as Item).Layout.minimumHeight : -1
		Layout.preferredHeight: item ? (item as Item).Layout.preferredHeight : -1
	}
	GText {
		id: subTitle

		textStyle: Style.text.subline
		visible: text !== ""
	}
	GText {
		id: info

		visible: text !== ""
	}
	ColumnLayout {
		id: additionalContentItem

		visible: false
	}
	GSpacer {
		Layout.fillHeight: true
		visible: enableButton.visible
	}
	GButton {
		id: enableButton

		Layout.alignment: Qt.AlignHCenter
		visible: text !== ""

		onClicked: root.enableClicked()
	}
}
