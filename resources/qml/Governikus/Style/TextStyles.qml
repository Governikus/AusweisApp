/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

PlatformTextStyles {
	readonly property var button: TextStyle {
		textColor: Style.color.button_text
	}
	readonly property var button_disabled: TextStyle {
		textColor: Style.color.button_text_disabled
	}
	readonly property var header: TextStyle {
		textSize: Style.dimens.header_font_size
	}
	readonly property var header_accent: TextStyle {
		textColor: Style.color.accent_text
		textSize: Style.dimens.header_font_size
	}
	readonly property var header_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.primary_text_inverse
		textSize: Style.dimens.header_font_size
	}
	readonly property var header_secondary: TextStyle {
		textColor: Style.color.secondary_text
		textSize: Style.dimens.header_font_size
	}
	readonly property var header_secondary_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.secondary_text_inverse
		textSize: Style.dimens.header_font_size
	}
	readonly property var header_warning: TextStyle {
		textColor: Style.color.warning_text
		textSize: Style.dimens.header_font_size
	}
	readonly property var hint: TextStyle {
		textSize: Style.dimens.hint_font_size
	}
	readonly property var hint_accent: TextStyle {
		textColor: Style.color.accent_text
		textSize: Style.dimens.hint_font_size
	}
	readonly property var hint_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.primary_text_inverse
		textSize: Style.dimens.hint_font_size
	}
	readonly property var hint_secondary: TextStyle {
		textColor: Style.color.secondary_text
		textSize: Style.dimens.hint_font_size
	}
	readonly property var hint_secondary_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.secondary_text_inverse
		textSize: Style.dimens.hint_font_size
	}
	readonly property var hint_warning: TextStyle {
		textColor: Style.color.warning_text
		textSize: Style.dimens.hint_font_size
	}
	readonly property var navigation: TextStyle {
		textSize: Style.dimens.navigation_font_size
	}
	readonly property var normal: TextStyle {
	}
	readonly property var normal_accent: TextStyle {
		textColor: Style.color.accent_text
	}
	readonly property var normal_info: TextStyle {
		textColor: Style.color.info_text
	}
	readonly property var normal_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.primary_text_inverse
	}
	readonly property var normal_secondary: TextStyle {
		textColor: Style.color.secondary_text
	}
	readonly property var normal_secondary_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.secondary_text_inverse
	}
	readonly property var normal_warning: TextStyle {
		textColor: Style.color.warning_text
	}
	readonly property var title: TextStyle {
		textSize: Style.dimens.title_font_size
	}
	readonly property var title_accent: TextStyle {
		textColor: Style.color.accent_text
		textSize: Style.dimens.title_font_size
	}
	readonly property var title_inverse: TextStyle {
		linkColor: Style.color.accent_text_inverse
		textColor: Style.color.primary_text_inverse
		textSize: Style.dimens.title_font_size
	}
	readonly property var title_secondary: TextStyle {
		textColor: Style.color.secondary_text
		textSize: Style.dimens.title_font_size
	}
	readonly property var title_warning: TextStyle {
		textColor: Style.color.warning_text
		textSize: Style.dimens.title_font_size
	}
}
