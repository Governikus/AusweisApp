/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.ResultView

RowLayout {
	id: root

	enum Type {
		IsSuccess,
		IsError,
		IsInfo,
		IsWait
	}

	property int resultType: ResultEntry.Type.IsSuccess
	property alias text: textItem.text

	Accessible.focusable: true
	Accessible.name: text
	Accessible.role: Accessible.ListItem
	Layout.alignment: Qt.AlignLeft
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	spacing: Constants.groupbox_spacing

	TintableIcon {
		Layout.alignment: Qt.AlignTop
		source: {
			switch (resultType) {
			case ResultEntry.Type.IsSuccess:
				return "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name);
			case ResultEntry.Type.IsInfo:
				return "qrc:///images/info.svg";
			case ResultEntry.Type.IsError:
				return "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name);
			}
		}
		sourceSize.height: Style.dimens.small_icon_size
		tintColor: {
			switch (resultType) {
			case ResultEntry.Type.IsSuccess:
				return Style.color.success;
			case ResultEntry.Type.IsInfo:
				return Style.color.text;
			case ResultEntry.Type.IsError:
				return Style.color.warning;
			}
		}
	}
	GText {
		id: textItem

		Accessible.ignored: true
		elide: Text.ElideRight
		textStyle: {
			switch (resultType) {
			case ResultEntry.Type.IsSuccess:
				return Style.text.normal;
			case ResultEntry.Type.IsInfo:
				return Style.text.normal;
			case ResultEntry.Type.IsError:
				return Style.text.normal_warning;
			}
		}
	}
}
