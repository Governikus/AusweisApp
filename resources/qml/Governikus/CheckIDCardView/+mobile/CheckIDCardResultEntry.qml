/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.ResultView 1.0

RowLayout {
	id: root

	property int resultType: ResultView.Type.IsSuccess
	property alias text: textItem.text

	Layout.fillWidth: true
	Layout.maximumWidth: Style.dimens.max_text_width
	Layout.alignment: Qt.AlignHCenter

	Accessible.role: Accessible.ListItem
	Accessible.name: text

	spacing: Constants.groupbox_spacing

	TintableIcon {
		sourceSize.height: Style.dimens.small_icon_size

		source: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
					return "qrc:///images/material_check.svg"
				case ResultView.Type.IsInfo:
					return "qrc:///images/info.svg"
				case ResultView.Type.IsError:
					return "qrc:///images/material_clear.svg"
			}
		}

		tintColor: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
					return Style.color.success
				case ResultView.Type.IsInfo:
					return Style.color.info_text
				case ResultView.Type.IsError:
					return Style.color.warning_text
			}
		}
	}

	GText {
		id: textItem

		Layout.fillWidth: true

		Accessible.ignored: true

		elide: Text.ElideRight
		maximumLineCount: 1
		textStyle: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
					return Style.text.normal
				case ResultView.Type.IsInfo:
					return Style.text.normal_info
				case ResultView.Type.IsError:
					return Style.text.normal_warning
			}
		}
	}
}
