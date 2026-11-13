package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.window.application
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.intui.standalone.theme.IntUiTheme
import org.jetbrains.jewel.intui.standalone.theme.createDefaultTextStyle
import org.jetbrains.jewel.intui.standalone.theme.createEditorTextStyle
import org.jetbrains.jewel.intui.standalone.theme.darkThemeDefinition
import org.jetbrains.jewel.intui.standalone.theme.default
import org.jetbrains.jewel.intui.standalone.theme.lightThemeDefinition
import org.jetbrains.jewel.intui.window.decoratedWindow
import org.jetbrains.jewel.intui.window.styling.dark
import org.jetbrains.jewel.intui.window.styling.lightWithLightHeader
import org.jetbrains.jewel.ui.ComponentStyling
import org.jetbrains.jewel.window.styling.TitleBarColors
import org.jetbrains.jewel.window.styling.TitleBarStyle

fun main() = application(exitProcessOnExit = false) {
    val dark = isSystemInDarkTheme()
    val textStyle = JewelTheme.createDefaultTextStyle()
    val editorStyle = JewelTheme.createEditorTextStyle()

    IntUiTheme(
        theme = if (dark) {
            JewelTheme.darkThemeDefinition(defaultTextStyle = textStyle, editorTextStyle = editorStyle)
        } else {
            JewelTheme.lightThemeDefinition(defaultTextStyle = textStyle, editorTextStyle = editorStyle)
        },
        styling = ComponentStyling.default().decoratedWindow(
            titleBarStyle = if (dark) {
                TitleBarStyle.dark(colors = TitleBarColors.dark(backgroundColor = Color.Transparent, inactiveBackground = Color.Transparent, borderColor = Color.Transparent))
            } else {
                val clear = Color.White.copy(alpha = 0f)
                TitleBarStyle.lightWithLightHeader(
                    colors = TitleBarColors.lightWithLightHeader(backgroundColor = clear, inactiveBackground = clear, borderColor = clear),
                )
            },
        ),
    ) {
        MainWindow(onClose = ::exitApplication)
    }
}
