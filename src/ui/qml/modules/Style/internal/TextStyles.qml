/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global

QtObject {
	readonly property var button: TextStyle {
		lineHeight: Style.dimens.lineHeight_button
		textColor: Style.color.control.content.basic
	}
	readonly property var headline: TextStyle {
		fontWeight: Font.Medium
		lineHeight: Style.dimens.lineHeight_headline
		textColor: Style.color.textHeadline.basic
		textSize: Style.dimens.textHeadline
	}
	readonly property var link: TextStyle {
		textColor: Style.color.linkBasic.basic
	}
	readonly property var navigation: TextStyle {
		fontWeight: Constants.is_desktop ? Font.Medium : Font.Normal
		lineHeight: Style.dimens.lineHeight_navigation
		textColor: Style.color.linkNavigation.basic
		textSize: Style.dimens.text_navigation
	}
	readonly property var normal: TextStyle {
	}
	readonly property var subline: TextStyle {
		fontWeight: Font.Medium
		lineHeight: Style.dimens.lineHeight_subline
		textColor: Style.color.textSubline.basic
		textSize: Style.dimens.textSubline
	}
	readonly property var tile: TextStyle {
		fontWeight: Font.Medium
		lineHeight: Style.dimens.lineHeight_tile
		textColor: Style.color.textTitle.basic
		textSize: Style.dimens.text_tile
	}
	readonly property var title: TextStyle {
		fontWeight: Font.Bold
		lineHeight: Style.dimens.lineHeight_title
		textColor: Style.color.textTitle.basic
		textSize: Style.dimens.textTitle
	}
}
