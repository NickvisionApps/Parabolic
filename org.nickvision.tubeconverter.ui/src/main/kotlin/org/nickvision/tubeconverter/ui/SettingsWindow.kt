@file:Suppress("ktlint:standard:max-line-length")

package org.nickvision.tubeconverter.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.input.rememberTextFieldState
import androidx.compose.foundation.text.input.setTextAndPlaceCursorAtEnd
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.key
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.runtime.setValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.DpSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.WindowPosition
import androidx.compose.ui.window.rememberWindowState
import kotlinx.coroutines.flow.drop
import org.jetbrains.jewel.foundation.theme.JewelTheme
import org.jetbrains.jewel.ui.Orientation
import org.jetbrains.jewel.ui.component.CheckboxRow
import org.jetbrains.jewel.ui.component.Divider
import org.jetbrains.jewel.ui.component.ExternalLink
import org.jetbrains.jewel.ui.component.InfoText
import org.jetbrains.jewel.ui.component.ListComboBox
import org.jetbrains.jewel.ui.component.OutlinedButton
import org.jetbrains.jewel.ui.component.SimpleListItem
import org.jetbrains.jewel.ui.component.Text
import org.jetbrains.jewel.ui.component.TextField
import org.jetbrains.jewel.ui.typography
import org.jetbrains.jewel.window.DecoratedWindow
import org.jetbrains.jewel.window.TitleBar
import org.jetbrains.jewel.window.newFullscreenControls
import java.awt.FileDialog
import java.io.File
import java.text.Collator
import java.util.Locale

private enum class Section(val title: () -> String) {
    General({ Controller.trp("Settings", "General") }),
    Downloads({ Controller.trp("Settings", "Downloads") }),
    Discovery({ Controller.trp("Settings", "Discovery") }),
    Extractor({ Controller.trp("Settings", "Extractor") }),
    Aria2({ Controller.trp("Settings", "aria2") }),
    History({ Controller.trp("Settings", "History") }),
    Postprocessing({ Controller.trp("Settings", "Postprocessing") }),
    Arguments({ Controller.tr("Arguments") }),
}

private fun labeled(label: String) = Controller.tr("{}:").replace("{}", label)

@Composable
private fun SettingCheckbox(
    text: String,
    checked: Boolean,
    onCheckedChange: (Boolean) -> Unit,
    description: String? = null,
    enabled: Boolean = true,
) {
    Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
        CheckboxRow(text, checked, onCheckedChange, enabled = enabled)
        if (description != null) {
            InfoText(description, Modifier.padding(start = 24.dp))
        }
    }
}

@Composable
private fun SettingCombo(
    label: String,
    items: List<String>,
    selectedIndex: Int,
    onSelectedItemChange: (Int) -> Unit,
    description: String? = null,
) {
    Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Text(labeled(label))
            ListComboBox(items, selectedIndex.coerceIn(items.indices), onSelectedItemChange, Modifier.width(220.dp))
        }
        if (description != null) {
            InfoText(description)
        }
    }
}

@Composable
private fun SettingNumber(
    label: String,
    value: Int,
    range: IntRange,
    onValueChange: (Int) -> Unit,
    enabled: Boolean = true,
    description: String? = null,
) {
    val state = rememberTextFieldState(value.toString())
    val latest by rememberUpdatedState(onValueChange)
    LaunchedEffect(state) {
        snapshotFlow { state.text.toString() }.drop(1).collect { text ->
            val digits = text.filter { it.isDigit() }
            if (digits != text) {
                state.setTextAndPlaceCursorAtEnd(digits)
            } else {
                digits.toIntOrNull()?.let { latest(it.coerceIn(range)) }
            }
        }
    }
    Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Text(labeled(label))
            TextField(state, Modifier.width(100.dp), enabled = enabled)
        }
        if (description != null) {
            InfoText(description)
        }
    }
}

@Composable
private fun SettingText(label: String, value: String, onValueChange: (String) -> Unit, enabled: Boolean = true) {
    val state = rememberTextFieldState(value)
    val latest by rememberUpdatedState(onValueChange)
    LaunchedEffect(state) {
        snapshotFlow { state.text.toString() }.drop(1).collect { latest(it) }
    }
    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
        Text(labeled(label))
        TextField(state, Modifier.width(320.dp), enabled = enabled)
    }
}

@Composable
fun SettingsWindow(onClose: () -> Unit) {
    var configuration by remember { mutableStateOf(Controller.getConfiguration()) }
    var section by remember { mutableStateOf(Section.General) }
    val update = { transform: (Configuration) -> Configuration ->
        configuration = transform(configuration)
        Controller.setConfiguration(configuration)
    }
    val languages = remember {
        Controller.getAvailableLanguages()
            .map { id ->
                id to Locale.forLanguageTag(id.replace('_', '-')).getDisplayName(Locale.getDefault()).ifBlank { id }
            }
            .sortedWith(compareBy(Collator.getInstance()) { it.second })
    }
    val languageIds = listOf("C") + languages.map { it.first }
    val languageNames = listOf(Controller.trp("Language", "System")) + languages.map { it.second }
    val historyLengths = listOf(0, 1, 7, 30, 90, 180, 365, -1)
    val steps = listOf(
        Controller.tr("None"),
        Controller.tr("Merger"),
        Controller.tr("Modify Chapters"),
        Controller.tr("Split Chapters"),
        Controller.tr("Extract Audio"),
        Controller.tr("Video Remuxer"),
        Controller.tr("Video Converter"),
        Controller.tr("Metadata"),
        Controller.tr("Embed Subtitle"),
        Controller.tr("Embed Thumbnail"),
        Controller.tr("Subtitles Converter"),
        Controller.tr("Thumbnails Converter"),
        Controller.tr("Fixup Stretched"),
        Controller.tr("Fixup M4a"),
        Controller.tr("Fixup M3u8"),
        Controller.tr("Fixup Timestamp"),
        Controller.tr("Fixup Duration"),
    )

    DecoratedWindow(
        onCloseRequest = onClose,
        state = rememberWindowState(size = DpSize(860.dp, 600.dp), position = WindowPosition(Alignment.Center)),
        title = Controller.tr("Settings"),
    ) {
        MatchWindowBackground(window)
        TitleBar(Modifier.newFullscreenControls()) {}

        Row(Modifier.fillMaxSize().background(JewelTheme.globalColors.panelBackground)) {
            Column(
                Modifier.fillMaxHeight().width(190.dp).padding(8.dp),
                verticalArrangement = Arrangement.spacedBy(2.dp),
            ) {
                Section.entries.forEach { entry ->
                    SimpleListItem(
                        entry.title(),
                        section == entry,
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(start = if (entry == Section.Arguments) 16.dp else 0.dp)
                            .clickable { section = entry },
                    )
                }
            }
            Divider(Orientation.Vertical, Modifier.fillMaxHeight())
            Column(
                Modifier.fillMaxSize().verticalScroll(rememberScrollState()).padding(24.dp),
                verticalArrangement = Arrangement.spacedBy(16.dp),
            ) {
                Text(section.title(), style = JewelTheme.typography.h3TextStyle)
                when (section) {
                    Section.General -> {
                        SettingCombo(
                            Controller.tr("Theme"),
                            listOf(
                                Controller.trp("Theme", "Light"),
                                Controller.trp("Theme", "Dark"),
                                Controller.trp("Theme", "System"),
                            ),
                            configuration.theme,
                            { index ->
                                themeState.intValue = index
                                update { it.copy(theme = index) }
                            },
                        )
                        SettingCombo(
                            Controller.tr("Translation Language"),
                            languageNames,
                            languageIds.indexOf(configuration.translationLanguage).coerceAtLeast(0),
                            { index ->
                                update { it.copy(translationLanguage = languageIds[index]) }
                                Controller.refreshLanguage()
                            },
                        )
                        SettingCheckbox(
                            Controller.tr("Automatically Check for Updates"),
                            configuration.automaticallyCheckForUpdates,
                            { checked -> update { it.copy(automaticallyCheckForUpdates = checked) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Allow Preview Updates"),
                            configuration.allowPreviewUpdates,
                            { checked -> update { it.copy(allowPreviewUpdates = checked) } },
                            Controller.tr("Update the app and dependencies, such as yt-dlp, to beta versions."),
                        )
                    }

                    Section.Downloads -> {
                        SettingNumber(
                            Controller.tr("Maximum Active Downloads"),
                            configuration.maxNumberOfActiveDownloads,
                            1..10,
                            { value -> update { it.copy(maxNumberOfActiveDownloads = value) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Overwrite Existing Files"),
                            configuration.overwriteExistingFiles,
                            { checked -> update { it.copy(overwriteExistingFiles = checked) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Prevent Sleep During Downloads"),
                            configuration.preventSuspend,
                            { checked -> update { it.copy(preventSuspend = checked) } },
                        )
                        SettingText(
                            Controller.tr("Extra yt-dlp Download Arguments"),
                            configuration.ytdlpDownloadArgs,
                            { text -> update { it.copy(ytdlpDownloadArgs = text) } },
                        )
                    }

                    Section.Discovery -> {
                        SettingCheckbox(
                            Controller.tr("Show Auto-Generated Subtitles"),
                            configuration.includeAutoGeneratedSubtitles,
                            { checked -> update { it.copy(includeAutoGeneratedSubtitles = checked) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Include Media Id in Title"),
                            configuration.includeMediaIdInTitle,
                            { checked -> update { it.copy(includeMediaIdInTitle = checked) } },
                            Controller.tr("Add the media entry's yt-dlp id to its default title."),
                        )
                        SettingCheckbox(
                            Controller.tr("Show Super Resolution Formats"),
                            configuration.includeSuperResolutions,
                            { checked -> update { it.copy(includeSuperResolutions = checked) } },
                            Controller.tr(
                                "Super resolutions are resolutions that have been enhanced with artificial intelligence.",
                            ),
                        )
                        SettingCheckbox(
                            Controller.tr("Limit Filename Characters"),
                            configuration.limitCharacters,
                            { checked -> update { it.copy(limitCharacters = checked) } },
                            Controller.tr("Restrict filename characters to those supported only by Windows."),
                        )
                        SettingCombo(
                            Controller.tr("Preferred Audio Codec"),
                            listOf(
                                Controller.tr("Any"),
                                Controller.tr("FLAC"),
                                Controller.tr("WAV"),
                                Controller.tr("Opus"),
                                Controller.tr("AAC"),
                                Controller.tr("MP4A"),
                                Controller.tr("MP3"),
                            ),
                            configuration.preferredAudioCodec,
                            { index -> update { it.copy(preferredAudioCodec = index) } },
                            Controller.tr("Prefer this codec when parsing audio formats to show for download."),
                        )
                        SettingCombo(
                            Controller.tr("Preferred Video Codec"),
                            listOf(
                                Controller.tr("Any"),
                                Controller.tr("H.265"),
                                Controller.tr("VP9"),
                                Controller.tr("AV1"),
                                Controller.tr("H.264"),
                            ),
                            configuration.preferredVideoCodec,
                            { index -> update { it.copy(preferredVideoCodec = index) } },
                            Controller.tr("Prefer this codec when parsing video formats to show for download."),
                        )
                        SettingCombo(
                            Controller.tr("Preferred Subtitle Format"),
                            listOf(
                                Controller.tr("Any"),
                                Controller.tr("VTT"),
                                Controller.tr("SRT"),
                                Controller.tr("ASS"),
                                Controller.tr("LRC"),
                            ),
                            configuration.preferredSubtitleFormat,
                            { index -> update { it.copy(preferredSubtitleFormat = index) } },
                            Controller.tr("Prefer this subtitle file format for download."),
                        )
                        SettingCombo(
                            Controller.tr("Preferred Frame Rate"),
                            listOf(
                                Controller.tr("Any"),
                                Controller.tr("24 FPS"),
                                Controller.tr("30 FPS"),
                                Controller.tr("60 FPS"),
                            ),
                            configuration.preferredFrameRate,
                            { index -> update { it.copy(preferredFrameRate = index) } },
                            Controller.tr("Prefer this frame rate when parsing video formats to show for download."),
                        )
                        SettingText(
                            Controller.tr("Extra yt-dlp Discovery Arguments"),
                            configuration.ytdlpDiscoveryArgs,
                            { text -> update { it.copy(ytdlpDiscoveryArgs = text) } },
                        )
                    }

                    Section.Extractor -> {
                        SettingCheckbox(
                            Controller.tr("Use Part Files"),
                            configuration.usePartFiles,
                            { checked -> update { it.copy(usePartFiles = checked) } },
                            Controller.tr(
                                "Download media in separate .part files instead of directly into the output file.",
                            ),
                        )
                        SettingCheckbox(
                            Controller.tr("Use SponsorBlock for YouTube"),
                            configuration.youTubeSponsorBlock,
                            { checked -> update { it.copy(youTubeSponsorBlock = checked) } },
                            Controller.tr("Try to remove sponsored segments from videos."),
                        )
                        SettingCheckbox(
                            Controller.tr("Translate Metadata and Chapters"),
                            configuration.translateMetadataAndChapters,
                            { checked -> update { it.copy(translateMetadataAndChapters = checked) } },
                            Controller.tr(
                                "Automatically translate embedded metadata and chapters using the app's set language on supported sites and media.",
                            ),
                        )
                        SettingCheckbox(
                            Controller.tr("Limit Download Speed"),
                            configuration.speedLimit != null,
                            { checked -> update { it.copy(speedLimit = if (checked) 1024 else null) } },
                        )
                        SettingNumber(
                            Controller.tr("Speed Limit (KiB/s)"),
                            configuration.speedLimit ?: 1024,
                            512..10240,
                            { value -> update { it.copy(speedLimit = value) } },
                            enabled = configuration.speedLimit != null,
                        )
                        SettingText(
                            Controller.tr("Proxy URL"),
                            configuration.proxyUrl,
                            { text -> update { it.copy(proxyUrl = text) } },
                        )
                        SettingCombo(
                            Controller.tr("Cookies Browser"),
                            listOf(
                                Controller.tr("None"),
                                Controller.tr("Brave"),
                                Controller.tr("Chrome"),
                                Controller.tr("Chromium"),
                                Controller.tr("Edge"),
                                Controller.tr("Firefox"),
                                Controller.tr("Opera"),
                                Controller.tr("Vivaldi"),
                                Controller.tr("Whale"),
                            ),
                            configuration.cookiesBrowser,
                            { index -> update { it.copy(cookiesBrowser = index) } },
                        )
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Cookies File")))
                            Text(
                                configuration.cookiesPath,
                                Modifier.width(240.dp),
                                overflow = TextOverflow.Ellipsis,
                                maxLines = 1,
                            )
                            OutlinedButton({
                                val dialog = FileDialog(window, Controller.tr("Cookies File"), FileDialog.LOAD)
                                dialog.isVisible = true
                                val file = dialog.file
                                if (file != null) {
                                    update { it.copy(cookiesPath = File(dialog.directory, file).path) }
                                }
                            }) {
                                Text(Controller.tr("Browse…"))
                            }
                            OutlinedButton(
                                { update { it.copy(cookiesPath = "") } },
                                enabled = configuration.cookiesPath.isNotEmpty(),
                            ) {
                                Text(Controller.tr("Clear"))
                            }
                        }
                    }

                    Section.Aria2 -> {
                        SettingCheckbox(
                            Controller.tr("Use aria2"),
                            configuration.useAria,
                            { checked -> update { it.copy(useAria = checked) } },
                            Controller.tr(
                                "An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader.",
                            ),
                        )
                        ExternalLink(Controller.tr("Learn More…"), "https://aria2.github.io")
                        SettingNumber(
                            Controller.tr("Maximum Connections Per Server"),
                            configuration.ariaMaxConnectionsPerServer,
                            1..16,
                            { value -> update { it.copy(ariaMaxConnectionsPerServer = value) } },
                        )
                        SettingNumber(
                            Controller.tr("Minimum Split Size (MiB)"),
                            configuration.ariaMinSplitSize,
                            1..1024,
                            { value -> update { it.copy(ariaMinSplitSize = value) } },
                        )
                    }

                    Section.History -> {
                        SettingCombo(
                            Controller.tr("Keep Download History For"),
                            listOf(
                                Controller.tr("Never"),
                                Controller.tr("1 Day"),
                                Controller.tr("1 Week"),
                                Controller.tr("1 Month"),
                                Controller.tr("3 Months"),
                                Controller.tr("6 Months"),
                                Controller.tr("1 Year"),
                                Controller.tr("Forever"),
                            ),
                            historyLengths.indexOf(configuration.historyLength).coerceAtLeast(0),
                            { index -> update { it.copy(historyLength = historyLengths[index]) } },
                        )
                    }

                    Section.Postprocessing -> {
                        SettingCheckbox(
                            Controller.tr("Embed Metadata"),
                            configuration.embedMetadata,
                            { checked -> update { it.copy(embedMetadata = checked) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Remove Source Data"),
                            configuration.removeSourceData,
                            { checked -> update { it.copy(removeSourceData = checked) } },
                            Controller.tr("Clear metadata fields containing identifying download information."),
                        )
                        SettingCheckbox(
                            Controller.tr("Embed Thumbnails"),
                            configuration.embedThumbnails,
                            { checked -> update { it.copy(embedThumbnails = checked) } },
                            Controller.tr(
                                "If the file type does not support embedding, the thumbnail will be written to a separate image file.",
                            ),
                        )
                        SettingCheckbox(
                            Controller.tr("Crop Audio Thumbnails"),
                            configuration.cropAudioThumbnails,
                            { checked -> update { it.copy(cropAudioThumbnails = checked) } },
                            Controller.tr("Crop thumbnails of audio files to squares."),
                        )
                        SettingCheckbox(
                            Controller.tr("Embed Chapters"),
                            configuration.embedChapters,
                            { checked -> update { it.copy(embedChapters = checked) } },
                        )
                        SettingCheckbox(
                            Controller.tr("Embed Subtitles"),
                            configuration.embedSubtitles,
                            { checked -> update { it.copy(embedSubtitles = checked) } },
                            Controller.tr(
                                "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files.",
                            ),
                        )
                        SettingNumber(
                            Controller.tr("FFmpeg Threads"),
                            configuration.postprocessingThreads,
                            1..Runtime.getRuntime().availableProcessors(),
                            { value -> update { it.copy(postprocessingThreads = value) } },
                            description = Controller.tr("Limit the number of threads used by ffmpeg."),
                        )
                        OutlinedButton({ section = Section.Arguments }) {
                            Text(Controller.tr("Manage Arguments…"))
                        }
                    }

                    Section.Arguments -> {
                        var selected by remember { mutableIntStateOf(-1) }
                        InfoText(
                            Controller.tr("Arguments will be shown for selection in the download discovery dialog."),
                        )
                        Column(Modifier.fillMaxWidth().height(160.dp).verticalScroll(rememberScrollState())) {
                            configuration.postprocessingArguments.forEachIndexed { index, argument ->
                                SimpleListItem(
                                    argument.name,
                                    selected == index,
                                    modifier = Modifier.fillMaxWidth().clickable { selected = index },
                                )
                            }
                        }
                        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                            OutlinedButton({
                                val taken = configuration.postprocessingArguments.map { it.name }.toSet()
                                val name = generateSequence(1) { it + 1 }.map { it.toString() }.first { it !in taken }
                                update {
                                    it.copy(
                                        postprocessingArguments = it.postprocessingArguments +
                                            PostprocessingArgument(name),
                                    )
                                }
                                selected = configuration.postprocessingArguments.lastIndex
                            }) {
                                Text(Controller.tr("Add"))
                            }
                            OutlinedButton(
                                {
                                    update {
                                        it.copy(
                                            postprocessingArguments = it.postprocessingArguments
                                                .filterIndexed { index, _ -> index != selected },
                                        )
                                    }
                                    selected = -1
                                },
                                enabled = selected in configuration.postprocessingArguments.indices,
                            ) {
                                Text(Controller.tr("Remove"))
                            }
                        }
                        if (selected in configuration.postprocessingArguments.indices) {
                            key(selected) {
                                var nameTaken by remember { mutableStateOf(false) }
                                val argument = configuration.postprocessingArguments[selected]
                                val edit = { change: (PostprocessingArgument) -> PostprocessingArgument ->
                                    update { current ->
                                        current.copy(
                                            postprocessingArguments = current.postprocessingArguments.mapIndexed {
                                                    index,
                                                    item,
                                                ->
                                                if (index == selected) change(item) else item
                                            },
                                        )
                                    }
                                }
                                SettingText(
                                    Controller.tr("Name"),
                                    argument.name,
                                    { name ->
                                        nameTaken = configuration.postprocessingArguments.withIndex()
                                            .any { (index, item) -> index != selected && item.name == name }
                                        if (!nameTaken && name.isNotBlank()) {
                                            edit { it.copy(name = name) }
                                        }
                                    },
                                )
                                if (nameTaken) {
                                    InfoText(Controller.tr("An argument with this name already exists."))
                                }
                                SettingCombo(
                                    Controller.tr("Post Processor"),
                                    steps,
                                    argument.step,
                                    { index -> edit { it.copy(step = index) } },
                                )
                                SettingCombo(
                                    Controller.tr("Executable"),
                                    listOf(
                                        Controller.tr("None"),
                                        Controller.tr("AtomicParsley"),
                                        Controller.tr("FFmpeg"),
                                        Controller.tr("FFprobe"),
                                    ),
                                    argument.executable,
                                    { index -> edit { it.copy(executable = index) } },
                                )
                                SettingText(
                                    Controller.tr("Args"),
                                    argument.args,
                                    { text -> edit { it.copy(args = text) } },
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}
