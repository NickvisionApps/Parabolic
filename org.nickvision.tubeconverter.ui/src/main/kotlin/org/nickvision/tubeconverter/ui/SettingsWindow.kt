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
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.key
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
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
import org.jetbrains.jewel.ui.theme.colorPalette
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
fun SettingsWindow(onClose: () -> Unit) {
    var configuration by remember { mutableStateOf(Controller.getConfiguration()) }
    var section by remember { mutableStateOf(Section.General) }
    val update = { transform: (Configuration) -> Configuration ->
        configuration = transform(configuration)
        Controller.setConfiguration(configuration)
    }
    val displayLocale = when (configuration.translationLanguage) {
        "C" -> Locale.getDefault()
        else -> Locale.forLanguageTag(configuration.translationLanguage.replace('_', '-'))
    }
    val languages = remember(displayLocale) {
        Controller.getAvailableLanguages()
            .map { id ->
                id to Locale.forLanguageTag(id.replace('_', '-')).getDisplayName(displayLocale).ifBlank { id }
            }
            .sortedWith(compareBy(Collator.getInstance(displayLocale)) { it.second })
    }
    val languageIds = listOf("C") + languages.map { it.first }
    val historyLengths = listOf(0, 1, 7, 30, 90, 180, 365, -1)

    DecoratedWindow(
        onCloseRequest = onClose,
        state = rememberWindowState(size = DpSize(860.dp, 600.dp), position = WindowPosition(Alignment.Center)),
        title = Controller.tr("Settings"),
    ) {
        MatchWindowBackground(window, JewelTheme.colorPalette.rawMap.getValue("dialog-bg"))
        TitleBar(Modifier.newFullscreenControls()) {}

        Row(Modifier.fillMaxSize().background(JewelTheme.colorPalette.rawMap.getValue("dialog-bg"))) {
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
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Theme")))
                            ListComboBox(
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
                                Modifier.width(220.dp),
                            )
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Translation Language")))
                            ListComboBox(
                                listOf(Controller.trp("Language", "System")) + languages.map { it.second },
                                languageIds.indexOf(configuration.translationLanguage).coerceAtLeast(0),
                                { index ->
                                    update { it.copy(translationLanguage = languageIds[index]) }
                                    Controller.refreshLanguage()
                                },
                                Modifier.width(220.dp),
                            )
                        }
                        CheckboxRow(
                            Controller.tr("Automatically Check for Updates"),
                            configuration.automaticallyCheckForUpdates,
                            { checked -> update { it.copy(automaticallyCheckForUpdates = checked) } },
                        )
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Allow Preview Updates"),
                                configuration.allowPreviewUpdates,
                                { checked -> update { it.copy(allowPreviewUpdates = checked) } },
                            )
                            InfoText(
                                Controller.tr("Update the app and dependencies, such as yt-dlp, to beta versions."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                    }

                    Section.Downloads -> {
                        val maxActiveDownloads =
                            rememberTextFieldState(configuration.maxNumberOfActiveDownloads.toString())
                        val downloadArguments = rememberTextFieldState(configuration.ytdlpDownloadArgs)
                        LaunchedEffect(maxActiveDownloads) {
                            snapshotFlow { maxActiveDownloads.text.toString() }.drop(1).collect { text ->
                                text.toIntOrNull()?.let { value ->
                                    update { it.copy(maxNumberOfActiveDownloads = value.coerceIn(1..10)) }
                                }
                            }
                        }
                        LaunchedEffect(downloadArguments) {
                            snapshotFlow { downloadArguments.text.toString() }.drop(1).collect { text ->
                                update { it.copy(ytdlpDownloadArgs = text) }
                            }
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Maximum Active Downloads")))
                            TextField(maxActiveDownloads, Modifier.width(100.dp))
                        }
                        CheckboxRow(
                            Controller.tr("Overwrite Existing Files"),
                            configuration.overwriteExistingFiles,
                            { checked -> update { it.copy(overwriteExistingFiles = checked) } },
                        )
                        CheckboxRow(
                            Controller.tr("Prevent Sleep During Downloads"),
                            configuration.preventSuspend,
                            { checked -> update { it.copy(preventSuspend = checked) } },
                        )
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Extra yt-dlp Download Arguments")))
                            TextField(downloadArguments, Modifier.width(320.dp))
                        }
                    }

                    Section.Discovery -> {
                        val discoveryArguments = rememberTextFieldState(configuration.ytdlpDiscoveryArgs)
                        LaunchedEffect(discoveryArguments) {
                            snapshotFlow { discoveryArguments.text.toString() }.drop(1).collect { text ->
                                update { it.copy(ytdlpDiscoveryArgs = text) }
                            }
                        }
                        CheckboxRow(
                            Controller.tr("Show Auto-Generated Subtitles"),
                            configuration.includeAutoGeneratedSubtitles,
                            { checked -> update { it.copy(includeAutoGeneratedSubtitles = checked) } },
                        )
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Include Media Id in Title"),
                                configuration.includeMediaIdInTitle,
                                { checked -> update { it.copy(includeMediaIdInTitle = checked) } },
                            )
                            InfoText(
                                Controller.tr("Add the media entry's yt-dlp id to its default title."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Show Super Resolution Formats"),
                                configuration.includeSuperResolutions,
                                { checked -> update { it.copy(includeSuperResolutions = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "Super resolutions are resolutions that have been enhanced with artificial intelligence.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Limit Filename Characters"),
                                configuration.limitCharacters,
                                { checked -> update { it.copy(limitCharacters = checked) } },
                            )
                            InfoText(
                                Controller.tr("Restrict filename characters to those supported only by Windows."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(8.dp),
                            ) {
                                Text(labeled(Controller.tr("Preferred Audio Codec")))
                                ListComboBox(
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
                                    Modifier.width(220.dp),
                                )
                            }
                            InfoText(
                                Controller.tr("Prefer this codec when parsing audio formats to show for download."),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(8.dp),
                            ) {
                                Text(labeled(Controller.tr("Preferred Video Codec")))
                                ListComboBox(
                                    listOf(
                                        Controller.tr("Any"),
                                        Controller.tr("H.265"),
                                        Controller.tr("VP9"),
                                        Controller.tr("AV1"),
                                        Controller.tr("H.264"),
                                    ),
                                    configuration.preferredVideoCodec,
                                    { index -> update { it.copy(preferredVideoCodec = index) } },
                                    Modifier.width(220.dp),
                                )
                            }
                            InfoText(
                                Controller.tr("Prefer this codec when parsing video formats to show for download."),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(8.dp),
                            ) {
                                Text(labeled(Controller.tr("Preferred Subtitle Format")))
                                ListComboBox(
                                    listOf(
                                        Controller.tr("Any"),
                                        Controller.tr("VTT"),
                                        Controller.tr("SRT"),
                                        Controller.tr("ASS"),
                                        Controller.tr("LRC"),
                                    ),
                                    configuration.preferredSubtitleFormat,
                                    { index -> update { it.copy(preferredSubtitleFormat = index) } },
                                    Modifier.width(220.dp),
                                )
                            }
                            InfoText(Controller.tr("Prefer this subtitle file format for download."))
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(8.dp),
                            ) {
                                Text(labeled(Controller.tr("Preferred Frame Rate")))
                                ListComboBox(
                                    listOf(
                                        Controller.tr("Any"),
                                        Controller.tr("24 FPS"),
                                        Controller.tr("30 FPS"),
                                        Controller.tr("60 FPS"),
                                    ),
                                    configuration.preferredFrameRate,
                                    { index -> update { it.copy(preferredFrameRate = index) } },
                                    Modifier.width(220.dp),
                                )
                            }
                            InfoText(
                                Controller.tr(
                                    "Prefer this frame rate when parsing video formats to show for download.",
                                ),
                            )
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Extra yt-dlp Discovery Arguments")))
                            TextField(discoveryArguments, Modifier.width(320.dp))
                        }
                    }

                    Section.Extractor -> {
                        val speedLimit = rememberTextFieldState((configuration.speedLimit ?: 1024).toString())
                        val proxyUrl = rememberTextFieldState(configuration.proxyUrl)
                        LaunchedEffect(speedLimit) {
                            snapshotFlow { speedLimit.text.toString() }.drop(1).collect { text ->
                                text.toIntOrNull()?.let { value ->
                                    update { it.copy(speedLimit = value.coerceIn(512..10240)) }
                                }
                            }
                        }
                        LaunchedEffect(proxyUrl) {
                            snapshotFlow { proxyUrl.text.toString() }.drop(1).collect { text ->
                                update { it.copy(proxyUrl = text) }
                            }
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Use Part Files"),
                                configuration.usePartFiles,
                                { checked -> update { it.copy(usePartFiles = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "Download media in separate .part files instead of directly into the output file.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Use SponsorBlock for YouTube"),
                                configuration.youTubeSponsorBlock,
                                { checked -> update { it.copy(youTubeSponsorBlock = checked) } },
                            )
                            InfoText(
                                Controller.tr("Try to remove sponsored segments from videos."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Translate Metadata and Chapters"),
                                configuration.translateMetadataAndChapters,
                                { checked -> update { it.copy(translateMetadataAndChapters = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "Automatically translate embedded metadata and chapters using the app's set language on supported sites and media.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        CheckboxRow(
                            Controller.tr("Limit Download Speed"),
                            configuration.speedLimit != null,
                            { checked -> update { it.copy(speedLimit = if (checked) 1024 else null) } },
                        )
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Speed Limit (KiB/s)")))
                            TextField(speedLimit, Modifier.width(100.dp), enabled = configuration.speedLimit != null)
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Proxy URL")))
                            TextField(proxyUrl, Modifier.width(320.dp))
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Cookies Browser")))
                            ListComboBox(
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
                                Modifier.width(220.dp),
                            )
                        }
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
                        val maxConnections =
                            rememberTextFieldState(configuration.ariaMaxConnectionsPerServer.toString())
                        val minSplitSize = rememberTextFieldState(configuration.ariaMinSplitSize.toString())
                        LaunchedEffect(maxConnections) {
                            snapshotFlow { maxConnections.text.toString() }.drop(1).collect { text ->
                                text.toIntOrNull()?.let { value ->
                                    update { it.copy(ariaMaxConnectionsPerServer = value.coerceIn(1..16)) }
                                }
                            }
                        }
                        LaunchedEffect(minSplitSize) {
                            snapshotFlow { minSplitSize.text.toString() }.drop(1).collect { text ->
                                text.toIntOrNull()?.let { value ->
                                    update { it.copy(ariaMinSplitSize = value.coerceIn(1..1024)) }
                                }
                            }
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Use aria2"),
                                configuration.useAria,
                                { checked -> update { it.copy(useAria = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        ExternalLink(Controller.tr("Learn More…"), "https://aria2.github.io")
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Maximum Connections Per Server")))
                            TextField(maxConnections, Modifier.width(100.dp))
                        }
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Minimum Split Size (MiB)")))
                            TextField(minSplitSize, Modifier.width(100.dp))
                        }
                    }

                    Section.History -> {
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                        ) {
                            Text(labeled(Controller.tr("Keep Download History For")))
                            ListComboBox(
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
                                Modifier.width(220.dp),
                            )
                        }
                    }

                    Section.Postprocessing -> {
                        val threads = rememberTextFieldState(configuration.postprocessingThreads.toString())
                        LaunchedEffect(threads) {
                            snapshotFlow { threads.text.toString() }.drop(1).collect { text ->
                                text.toIntOrNull()?.let { value ->
                                    update {
                                        it.copy(
                                            postprocessingThreads = value.coerceIn(
                                                1..Runtime.getRuntime().availableProcessors(),
                                            ),
                                        )
                                    }
                                }
                            }
                        }
                        CheckboxRow(
                            Controller.tr("Embed Metadata"),
                            configuration.embedMetadata,
                            { checked -> update { it.copy(embedMetadata = checked) } },
                        )
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Remove Source Data"),
                                configuration.removeSourceData,
                                { checked -> update { it.copy(removeSourceData = checked) } },
                            )
                            InfoText(
                                Controller.tr("Clear metadata fields containing identifying download information."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Embed Thumbnails"),
                                configuration.embedThumbnails,
                                { checked -> update { it.copy(embedThumbnails = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "If the file type does not support embedding, the thumbnail will be written to a separate image file.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Crop Audio Thumbnails"),
                                configuration.cropAudioThumbnails,
                                { checked -> update { it.copy(cropAudioThumbnails = checked) } },
                            )
                            InfoText(
                                Controller.tr("Crop thumbnails of audio files to squares."),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        CheckboxRow(
                            Controller.tr("Embed Chapters"),
                            configuration.embedChapters,
                            { checked -> update { it.copy(embedChapters = checked) } },
                        )
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            CheckboxRow(
                                Controller.tr("Embed Subtitles"),
                                configuration.embedSubtitles,
                                { checked -> update { it.copy(embedSubtitles = checked) } },
                            )
                            InfoText(
                                Controller.tr(
                                    "If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files.",
                                ),
                                Modifier.padding(start = 24.dp),
                            )
                        }
                        Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                            Row(
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.spacedBy(8.dp),
                            ) {
                                Text(labeled(Controller.tr("FFmpeg Threads")))
                                TextField(threads, Modifier.width(100.dp))
                            }
                            InfoText(Controller.tr("Limit the number of threads used by ffmpeg."))
                        }
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
                                update { current ->
                                    current.copy(
                                        postprocessingArguments = current.postprocessingArguments +
                                            PostprocessingArgument(
                                                generateSequence(1) { it + 1 }
                                                    .map { it.toString() }
                                                    .first { name ->
                                                        current.postprocessingArguments.none { it.name == name }
                                                    },
                                            ),
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
                                val argumentName = rememberTextFieldState(argument.name)
                                val argumentArgs = rememberTextFieldState(argument.args)
                                val edit = { change: (PostprocessingArgument) -> PostprocessingArgument ->
                                    update { current ->
                                        current.copy(
                                            postprocessingArguments = current.postprocessingArguments
                                                .mapIndexed { index, item ->
                                                    if (index == selected) change(item) else item
                                                },
                                        )
                                    }
                                }
                                LaunchedEffect(argumentName) {
                                    snapshotFlow { argumentName.text.toString() }.drop(1).collect { text ->
                                        nameTaken = configuration.postprocessingArguments.withIndex()
                                            .any { (index, item) -> index != selected && item.name == text }
                                        if (!nameTaken && text.isNotBlank()) {
                                            edit { it.copy(name = text) }
                                        }
                                    }
                                }
                                LaunchedEffect(argumentArgs) {
                                    snapshotFlow { argumentArgs.text.toString() }.drop(1).collect { text ->
                                        edit { it.copy(args = text) }
                                    }
                                }
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                                ) {
                                    Text(labeled(Controller.tr("Name")))
                                    TextField(argumentName, Modifier.width(320.dp))
                                }
                                if (nameTaken) {
                                    InfoText(Controller.tr("An argument with this name already exists."))
                                }
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                                ) {
                                    Text(labeled(Controller.tr("Post Processor")))
                                    ListComboBox(
                                        listOf(
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
                                        ),
                                        argument.step,
                                        { index -> edit { it.copy(step = index) } },
                                        Modifier.width(220.dp),
                                    )
                                }
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                                ) {
                                    Text(labeled(Controller.tr("Executable")))
                                    ListComboBox(
                                        listOf(
                                            Controller.tr("None"),
                                            Controller.tr("AtomicParsley"),
                                            Controller.tr("FFmpeg"),
                                            Controller.tr("FFprobe"),
                                        ),
                                        argument.executable,
                                        { index -> edit { it.copy(executable = index) } },
                                        Modifier.width(220.dp),
                                    )
                                }
                                Row(
                                    verticalAlignment = Alignment.CenterVertically,
                                    horizontalArrangement = Arrangement.spacedBy(8.dp),
                                ) {
                                    Text(labeled(Controller.tr("Args")))
                                    TextField(argumentArgs, Modifier.width(320.dp))
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
