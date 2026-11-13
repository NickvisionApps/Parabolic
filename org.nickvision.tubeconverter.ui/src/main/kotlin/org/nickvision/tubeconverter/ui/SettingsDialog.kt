package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.DpSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.DialogWindow
import androidx.compose.ui.window.rememberDialogState
import org.jetbrains.jewel.foundation.theme.JewelTheme

@Composable
fun SettingsDialog(onClose: () -> Unit) {
    val dark = JewelTheme.isDark

    DialogWindow(onCloseRequest = onClose, state = rememberDialogState(size = DpSize(720.dp, 520.dp)), title = "Settings") {
        MatchWindowBackground(window)
        LaunchedEffect(dark) {
            window.rootPane.apply {
                putClientProperty("apple.awt.windowAppearance", if (dark) "NSAppearanceNameDarkAqua" else "NSAppearanceNameAqua")
                putClientProperty("apple.awt.fullWindowContent", true)
                putClientProperty("apple.awt.transparentTitleBar", true)
                putClientProperty("apple.awt.windowTitleVisible", false)
            }
        }

        Box(Modifier.fillMaxSize().background(JewelTheme.globalColors.panelBackground))
    }
}
