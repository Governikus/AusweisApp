/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.0
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property alias title: titleText.text
	property int topPadding: Constants.component_spacing

	spacing: Constants.component_spacing

	GText {
		id: titleText

		Layout.topMargin: baseItem.topPadding
		Layout.preferredWidth: titleText.implicitWidth

		textStyle: Style.text.title
	}

	GSeparator {
		Layout.fillWidth: true
		Layout.topMargin: baseItem.topPadding
	}
}
