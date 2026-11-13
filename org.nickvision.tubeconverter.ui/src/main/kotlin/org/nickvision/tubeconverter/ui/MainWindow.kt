@file:OptIn(ExperimentalFoundationApi::class)

package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalWindowInfo
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.ui.component.Icon
import org.jetbrains.jewel.ui.component.IconButton
import org.jetbrains.jewel.ui.component.SelectableIconButton
import org.jetbrains.jewel.ui.component.Text
import org.jetbrains.jewel.ui.component.Tooltip
import org.jetbrains.jewel.ui.icon.IconKey
import org.jetbrains.jewel.ui.icons.AllIconsKeys
import org.jetbrains.jewel.ui.theme.colorPalette
import org.jetbrains.jewel.window.DecoratedWindow
import org.jetbrains.jewel.window.TitleBar
import org.jetbrains.jewel.window.defaultTitleBarStyle
import org.jetbrains.jewel.window.newFullscreenControls
import java.awt.Window
import javax.swing.RootPaneContainer

private val GlowCenterX = 140.dp
private val GlowRadius = 720.dp
private val TitleBarBorder = 1.dp

@Composable
private fun Modifier.frameBackground(top: Dp = 0.dp): Modifier {
    val frame = JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-bg")
    val glow = JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-a2")
    return drawBehind {
        val area = size.copy(height = size.height + TitleBarBorder.toPx())
        drawRect(frame, size = area)
        drawRect(
            Brush.radialGradient(
                listOf(glow, Color.Transparent),
                Offset(GlowCenterX.toPx(), -top.toPx()),
                GlowRadius.toPx(),
            ),
            size = area,
        )
    }
}

private enum class Page(val label: () -> String, val icon: IconKey) {
    Downloads({ Controller.tr("Downloads") }, AllIconsKeys.Actions.Download),
    Keyring({ Controller.tr("Keyring") }, AllIconsKeys.Ide.Readonly),
    History({ Controller.tr("History") }, AllIconsKeys.General.History),
}

@Composable
fun MainWindow(onClose: () -> Unit) {
    var settingsOpen by remember { mutableStateOf(false) }
    var page by remember { mutableStateOf(Page.Downloads) }

    DecoratedWindow(onCloseRequest = onClose, title = "Parabolic", enabled = !settingsOpen) {
        MatchWindowBackground(window, JewelTheme.colorPalette.rawMap.getValue("grad-g1-amber-bg"))
        TitleBar(Modifier.newFullscreenControls().frameBackground()) {
            Row(Modifier.align(Alignment.End).padding(end = 8.dp)) {
                Tooltip({ Text(Controller.tr("Settings")) }) {
                    IconButton({ settingsOpen = true }, Modifier.size(28.dp)) { _ ->
                        Icon(AllIconsKeys.General.Settings, Controller.tr("Settings"), Modifier.size(18.dp))
                    }
                }
            }
        }

        Row(
            Modifier.fillMaxSize()
                .frameBackground(top = JewelTheme.defaultTitleBarStyle.metrics.height + TitleBarBorder),
        ) {
            Column(
                Modifier.fillMaxHeight().width(46.dp).padding(vertical = 6.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(4.dp),
            ) {
                Page.entries.forEach { entry ->
                    Tooltip({ Text(entry.label()) }) {
                        SelectableIconButton(page == entry, { page = entry }, Modifier.size(34.dp)) { _ ->
                            val blue = page == entry && !JewelTheme.isDark &&
                                LocalWindowInfo.current.isWindowFocused
                            Icon(
                                entry.icon,
                                entry.label(),
                                Modifier.size(21.dp),
                                tint = if (blue) Color.White else Color.Unspecified,
                            )
                        }
                    }
                }
            }
            Box(
                Modifier.fillMaxSize().padding(end = 4.dp, bottom = 4.dp)
                    .clip(RoundedCornerShape(10.dp))
                    .background(JewelTheme.colorPalette.rawMap.getValue("tool-window-bg")),
            ) {
                when (page) {
                    Page.Downloads -> DownloadsPage()
                    Page.Keyring -> KeyringPage()
                    Page.History -> HistoryPage()
                }
            }
        }

        if (settingsOpen) {
            SettingsWindow(onClose = { settingsOpen = false })
        }
    }
}

@Composable
fun MatchWindowBackground(window: Window, color: Color) {
    LaunchedEffect(window, color) {
        val awtColor = java.awt.Color(color.toArgb())
        window.background = awtColor
        (window as? RootPaneContainer)?.contentPane?.background = awtColor
    }
}
