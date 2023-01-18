/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.ResultView 1.0

RowLayout {
	id: root

	property int resultType: ResultView.Type.IsSuccess
	property alias text: textItem.text

	Accessible.focusable: true
	Accessible.name: text
	Accessible.role: Accessible.ListItem
	Layout.alignment: Qt.AlignHCenter
	Layout.fillWidth: true
	Layout.maximumWidth: Style.dimens.max_text_width
	spacing: Constants.groupbox_spacing

	TintableIcon {
		Layout.alignment: Qt.AlignTop
		source: {
			switch (resultType) {
			case ResultView.Type.IsSuccess:
				return "qrc:///images/material_check.svg";
			case ResultView.Type.IsInfo:
				return "qrc:///images/info.svg";
			case ResultView.Type.IsError:
				return "qrc:///images/material_clear.svg";
			}
		}
		sourceSize.height: Style.dimens.small_icon_size
		tintColor: {
			switch (resultType) {
			case ResultView.Type.IsSuccess:
				return Style.color.success;
			case ResultView.Type.IsInfo:
				return Style.color.info_text;
			case ResultView.Type.IsError:
				return Style.color.warning_text;
			}
		}
	}
	GText {
		id: textItem
		Accessible.ignored: true
		Layout.fillWidth: true
		elide: Text.ElideRight
		textStyle: {
			switch (resultType) {
			case ResultView.Type.IsSuccess:
				return Style.text.normal;
			case ResultView.Type.IsInfo:
				return Style.text.normal_info;
			case ResultView.Type.IsError:
				return Style.text.normal_warning;
			}
		}
	}
}
