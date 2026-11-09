#import "settings_model.h"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "application.h"
#include "aria2.h"
#include "audio.h"
#include "configuration.h"
#include "cookies.h"
#include "discovery.h"
#include "download.h"
#include "extractor.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "update.h"
#include "video.h"

@implementation SettingsModel {
    NSMutableArray<NSString*>* _languageIdentifiers;
    NSMutableArray<NSString*>* _languageNames;
    NSInteger _speedLimit;
}

- (instancetype)init
{
    self = [super init];
    if (self != nil)
    {
        const parabolic::translation::Translator& translator{ parabolic::application::State::instance().translator() };
        NSLocale* currentLocale{ [NSLocale currentLocale] };
        _languageIdentifiers = [NSMutableArray
                arrayWithObject:@(parabolic::translation::Translator::follow_system_language().c_str())];
        _languageNames = [NSMutableArray arrayWithObject:@(translator._p("Language", "System"))];
        for (const std::string& language : translator.available_languages())
        {
            NSString* identifier
            {
                @(language.c_str()),
            };
            NSString* displayName{ [currentLocale localizedStringForLocaleIdentifier:identifier] };
            [_languageIdentifiers addObject:identifier];
            [_languageNames addObject:displayName.length > 0 ? displayName : identifier];
        }
        _speedLimit =
                parabolic::application::State::instance().configuration().extractor_options().speed_limit().value_or(
                        1024);
    }
    return self;
}

+ (SettingsModel*)sharedModel
{
    static SettingsModel* model{ [[SettingsModel alloc] init] };
    return model;
}

- (BOOL)allowPreviewUpdates
{
    return parabolic::application::State::instance().configuration().update_options().allow_preview_updates();
}

- (void)setAllowPreviewUpdates:(BOOL)allowPreviewUpdates
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::update::Options options{ configuration.update_options() };
    options.set_allow_preview_updates(allowPreviewUpdates);
    configuration.set_update_options(options);
}

- (BOOL)automaticallyCheckForUpdates
{
    return parabolic::application::State::instance().configuration().automatically_check_for_updates();
}

- (void)setAutomaticallyCheckForUpdates:(BOOL)automaticallyCheckForUpdates
{
    parabolic::application::State::instance().configuration().set_automatically_check_for_updates(
            automaticallyCheckForUpdates);
}

- (NSInteger)cookiesBrowser
{
    return static_cast<NSInteger>(
            parabolic::application::State::instance().configuration().extractor_options().cookies_browser());
}

- (void)setCookiesBrowser:(NSInteger)cookiesBrowser
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_cookies_browser(static_cast<parabolic::cookies::Browser>(cookiesBrowser));
    configuration.set_extractor_options(options);
}

- (NSString*)cookiesPath
{
    return @(parabolic::application::State::instance().configuration().extractor_options().cookies_path().c_str());
}

- (void)setCookiesPath:(NSString*)cookiesPath
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_cookies_path(cookiesPath != nil ? cookiesPath.UTF8String : "");
    configuration.set_extractor_options(options);
}

- (BOOL)cropAudioThumbnails
{
    return parabolic::application::State::instance().configuration().postprocessing_options().crop_audio_thumbnails();
}

- (void)setCropAudioThumbnails:(BOOL)cropAudioThumbnails
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_crop_audio_thumbnails(cropAudioThumbnails);
    configuration.set_postprocessing_options(options);
}

- (NSString*)discoveryYtdlpArguments
{
    return @(parabolic::application::State::instance().configuration().discovery_options().ytdlp_arguments().c_str());
}

- (void)setDiscoveryYtdlpArguments:(NSString*)discoveryYtdlpArguments
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_ytdlp_arguments(discoveryYtdlpArguments != nil ? discoveryYtdlpArguments.UTF8String : "");
    configuration.set_discovery_options(options);
}

- (NSString*)downloadYtdlpArguments
{
    return @(parabolic::application::State::instance().configuration().download_options().ytdlp_arguments().c_str());
}

- (void)setDownloadYtdlpArguments:(NSString*)downloadYtdlpArguments
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_ytdlp_arguments(downloadYtdlpArguments != nil ? downloadYtdlpArguments.UTF8String : "");
    configuration.set_download_options(options);
}

- (BOOL)embedChapters
{
    return parabolic::application::State::instance().configuration().postprocessing_options().embed_chapters();
}

- (void)setEmbedChapters:(BOOL)embedChapters
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_chapters(embedChapters);
    configuration.set_postprocessing_options(options);
}

- (BOOL)embedMetadata
{
    return parabolic::application::State::instance().configuration().postprocessing_options().embed_metadata();
}

- (void)setEmbedMetadata:(BOOL)embedMetadata
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_metadata(embedMetadata);
    configuration.set_postprocessing_options(options);
}

- (BOOL)embedSubtitles
{
    return parabolic::application::State::instance().configuration().postprocessing_options().embed_subtitles();
}

- (void)setEmbedSubtitles:(BOOL)embedSubtitles
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_subtitles(embedSubtitles);
    configuration.set_postprocessing_options(options);
}

- (BOOL)embedThumbnails
{
    return parabolic::application::State::instance().configuration().postprocessing_options().embed_thumbnails();
}

- (void)setEmbedThumbnails:(BOOL)embedThumbnails
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_embed_thumbnails(embedThumbnails);
    configuration.set_postprocessing_options(options);
}

- (NSInteger)historyLength
{
    return static_cast<NSInteger>(parabolic::application::State::instance().configuration().history_options().length());
}

- (void)setHistoryLength:(NSInteger)historyLength
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::history::Options options{ configuration.history_options() };
    options.set_length(static_cast<parabolic::history::Length>(historyLength));
    configuration.set_history_options(options);
}

- (BOOL)includeAutoGeneratedSubtitles
{
    return parabolic::application::State::instance()
            .configuration()
            .discovery_options()
            .include_auto_generated_subtitles();
}

- (void)setIncludeAutoGeneratedSubtitles:(BOOL)includeAutoGeneratedSubtitles
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_auto_generated_subtitles(includeAutoGeneratedSubtitles);
    configuration.set_discovery_options(options);
}

- (BOOL)includeMediaIdInTitle
{
    return parabolic::application::State::instance().configuration().discovery_options().include_media_id_in_title();
}

- (void)setIncludeMediaIdInTitle:(BOOL)includeMediaIdInTitle
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_media_id_in_title(includeMediaIdInTitle);
    configuration.set_discovery_options(options);
}

- (BOOL)includeSuperResolutions
{
    return parabolic::application::State::instance().configuration().discovery_options().include_super_resolutions();
}

- (void)setIncludeSuperResolutions:(BOOL)includeSuperResolutions
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_include_super_resolutions(includeSuperResolutions);
    configuration.set_discovery_options(options);
}

- (NSInteger)languageIndex
{
    NSUInteger index
    {
        [_languageIdentifiers indexOfObject:@(parabolic::application::State::instance()
                                                      .configuration()
                                                      .translation_options()
                                                      .language()
                                                      .c_str())],
    };
    return index == NSNotFound ? 0 : static_cast<NSInteger>(index);
}

- (void)setLanguageIndex:(NSInteger)languageIndex
{
    if (languageIndex < 0 || std::cmp_greater_equal(languageIndex, _languageIdentifiers.count))
    {
        return;
    }
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::translation::Options options{ configuration.translation_options() };
    options.set_language(_languageIdentifiers[static_cast<NSUInteger>(languageIndex)].UTF8String);
    configuration.set_translation_options(options);
}

- (NSArray<NSString*>*)languageNames
{
    return _languageNames;
}

- (BOOL)limitCharacters
{
    return parabolic::application::State::instance().configuration().discovery_options().limit_characters();
}

- (void)setLimitCharacters:(BOOL)limitCharacters
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_limit_characters(limitCharacters);
    configuration.set_discovery_options(options);
}

- (BOOL)limitSpeed
{
    return parabolic::application::State::instance().configuration().extractor_options().speed_limit().has_value();
}

- (void)setLimitSpeed:(BOOL)limitSpeed
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_speed_limit(limitSpeed ? std::optional<int>{ static_cast<int>(_speedLimit) } : std::nullopt);
    configuration.set_extractor_options(options);
}

- (NSInteger)maxActiveDownloads
{
    return parabolic::application::State::instance()
            .configuration()
            .download_options()
            .max_number_of_active_downloads();
}

- (void)setMaxActiveDownloads:(NSInteger)maxActiveDownloads
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_max_number_of_active_downloads(static_cast<int>(maxActiveDownloads));
    configuration.set_download_options(options);
}

- (NSInteger)maxConnectionsPerServer
{
    return parabolic::application::State::instance().configuration().aria2_options().max_connections_per_server();
}

- (void)setMaxConnectionsPerServer:(NSInteger)maxConnectionsPerServer
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::aria2::Options options{ configuration.aria2_options() };
    options.set_max_connections_per_server(static_cast<int>(maxConnectionsPerServer));
    configuration.set_aria2_options(options);
}

- (NSInteger)minSplitSize
{
    return parabolic::application::State::instance().configuration().aria2_options().min_split_size();
}

- (void)setMinSplitSize:(NSInteger)minSplitSize
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::aria2::Options options{ configuration.aria2_options() };
    options.set_min_split_size(static_cast<int>(minSplitSize));
    configuration.set_aria2_options(options);
}

- (BOOL)overwriteExistingFiles
{
    return parabolic::application::State::instance().configuration().download_options().overwrite_existing_files();
}

- (void)setOverwriteExistingFiles:(BOOL)overwriteExistingFiles
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_overwrite_existing_files(overwriteExistingFiles);
    configuration.set_download_options(options);
}

- (NSInteger)postprocessingThreads
{
    return parabolic::application::State::instance().configuration().postprocessing_options().threads();
}

- (void)setPostprocessingThreads:(NSInteger)postprocessingThreads
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_threads(static_cast<int>(postprocessingThreads));
    configuration.set_postprocessing_options(options);
}

- (NSInteger)preferredAudioCodec
{
    return static_cast<NSInteger>(
            parabolic::application::State::instance().configuration().discovery_options().preferred_audio_codec());
}

- (void)setPreferredAudioCodec:(NSInteger)preferredAudioCodec
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_audio_codec(static_cast<parabolic::audio::Codec>(preferredAudioCodec));
    configuration.set_discovery_options(options);
}

- (NSInteger)preferredFrameRate
{
    return static_cast<NSInteger>(
            parabolic::application::State::instance().configuration().discovery_options().preferred_frame_rate());
}

- (void)setPreferredFrameRate:(NSInteger)preferredFrameRate
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_frame_rate(static_cast<parabolic::video::FrameRate>(preferredFrameRate));
    configuration.set_discovery_options(options);
}

- (NSInteger)preferredSubtitleFormat
{
    return static_cast<NSInteger>(
            parabolic::application::State::instance().configuration().discovery_options().preferred_subtitle_format());
}

- (void)setPreferredSubtitleFormat:(NSInteger)preferredSubtitleFormat
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_subtitle_format(static_cast<parabolic::subtitle::Type>(preferredSubtitleFormat));
    configuration.set_discovery_options(options);
}

- (NSInteger)preferredVideoCodec
{
    return static_cast<NSInteger>(
            parabolic::application::State::instance().configuration().discovery_options().preferred_video_codec());
}

- (void)setPreferredVideoCodec:(NSInteger)preferredVideoCodec
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::discovery::Options options{ configuration.discovery_options() };
    options.set_preferred_video_codec(static_cast<parabolic::video::Codec>(preferredVideoCodec));
    configuration.set_discovery_options(options);
}

- (BOOL)preventSuspend
{
    return parabolic::application::State::instance().configuration().download_options().prevent_suspend();
}

- (void)setPreventSuspend:(BOOL)preventSuspend
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::download::Options options{ configuration.download_options() };
    options.set_prevent_suspend(preventSuspend);
    configuration.set_download_options(options);
}

- (NSString*)proxyUrl
{
    return @(parabolic::application::State::instance().configuration().extractor_options().proxy_url().c_str());
}

- (void)setProxyUrl:(NSString*)proxyUrl
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_proxy_url(proxyUrl != nil ? proxyUrl.UTF8String : "");
    configuration.set_extractor_options(options);
}

- (BOOL)removeSourceData
{
    return parabolic::application::State::instance().configuration().postprocessing_options().remove_source_data();
}

- (void)setRemoveSourceData:(BOOL)removeSourceData
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    options.set_remove_source_data(removeSourceData);
    configuration.set_postprocessing_options(options);
}

- (NSInteger)speedLimit
{
    return parabolic::application::State::instance().configuration().extractor_options().speed_limit().value_or(
            static_cast<int>(_speedLimit));
}

- (void)setSpeedLimit:(NSInteger)speedLimit
{
    _speedLimit = speedLimit;
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    if (options.speed_limit().has_value())
    {
        options.set_speed_limit(static_cast<int>(speedLimit));
        configuration.set_extractor_options(options);
    }
}

- (NSInteger)theme
{
    return static_cast<NSInteger>(parabolic::application::State::instance().configuration().theme());
}

- (void)setTheme:(NSInteger)theme
{
    parabolic::configuration::Theme value{ static_cast<parabolic::configuration::Theme>(theme) };
    parabolic::application::State::instance().configuration().set_theme(value);
    switch (value)
    {
    case parabolic::configuration::Theme::Light:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];
        break;
    case parabolic::configuration::Theme::Dark:
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
        break;
    case parabolic::configuration::Theme::System:
    default:
        NSApp.appearance = nil;
        break;
    }
}

- (BOOL)translateMetadataAndChapters
{
    return parabolic::application::State::instance()
            .configuration()
            .extractor_options()
            .translate_metadata_and_chapters();
}

- (void)setTranslateMetadataAndChapters:(BOOL)translateMetadataAndChapters
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_translate_metadata_and_chapters(translateMetadataAndChapters);
    configuration.set_extractor_options(options);
}

- (BOOL)useAria
{
    return parabolic::application::State::instance().configuration().aria2_options().use_aria();
}

- (void)setUseAria:(BOOL)useAria
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::aria2::Options options{ configuration.aria2_options() };
    options.set_use_aria(useAria);
    configuration.set_aria2_options(options);
}

- (BOOL)usePartFiles
{
    return parabolic::application::State::instance().configuration().extractor_options().use_part_files();
}

- (void)setUsePartFiles:(BOOL)usePartFiles
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_use_part_files(usePartFiles);
    configuration.set_extractor_options(options);
}

- (BOOL)youtubeSponsorBlock
{
    return parabolic::application::State::instance().configuration().extractor_options().youtube_sponsor_block();
}

- (void)setYoutubeSponsorBlock:(BOOL)youtubeSponsorBlock
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::extractor::Options options{ configuration.extractor_options() };
    options.set_youtube_sponsor_block(youtubeSponsorBlock);
    configuration.set_extractor_options(options);
}

- (void)addArgumentWithName:(NSString*)name step:(NSInteger)step executable:(NSInteger)executable args:(NSString*)args
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    std::vector<parabolic::postprocessing::Argument> arguments{ options.arguments() };
    arguments.emplace_back(name.UTF8String, static_cast<parabolic::postprocessing::Step>(step),
                           static_cast<parabolic::postprocessing::Executable>(executable), args.UTF8String);
    options.set_arguments(arguments);
    configuration.set_postprocessing_options(options);
}

- (NSString*)argumentArgsAtIndex:(NSUInteger)index
{
    return @(parabolic::application::State::instance()
                     .configuration()
                     .postprocessing_options()
                     .arguments()[index]
                     .args()
                     .c_str());
}

- (NSUInteger)argumentCount
{
    return parabolic::application::State::instance().configuration().postprocessing_options().arguments().size();
}

- (NSInteger)argumentExecutableAtIndex:(NSUInteger)index
{
    return static_cast<NSInteger>(parabolic::application::State::instance()
                                          .configuration()
                                          .postprocessing_options()
                                          .arguments()[index]
                                          .executable());
}

- (NSString*)argumentNameAtIndex:(NSUInteger)index
{
    return @(parabolic::application::State::instance()
                     .configuration()
                     .postprocessing_options()
                     .arguments()[index]
                     .name()
                     .c_str());
}

- (NSInteger)argumentStepAtIndex:(NSUInteger)index
{
    return static_cast<NSInteger>(parabolic::application::State::instance()
                                          .configuration()
                                          .postprocessing_options()
                                          .arguments()[index]
                                          .step());
}

- (void)removeArgumentAtIndex:(NSUInteger)index
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    std::vector<parabolic::postprocessing::Argument> arguments{ options.arguments() };
    arguments.erase(arguments.begin() + static_cast<std::ptrdiff_t>(index));
    options.set_arguments(arguments);
    configuration.set_postprocessing_options(options);
}

- (void)replaceArgumentAtIndex:(NSUInteger)index
                      withName:(NSString*)name
                          step:(NSInteger)step
                    executable:(NSInteger)executable
                          args:(NSString*)args
{
    parabolic::configuration::Configuration& configuration{ parabolic::application::State::instance().configuration() };
    parabolic::postprocessing::Options options{ configuration.postprocessing_options() };
    std::vector<parabolic::postprocessing::Argument> arguments{ options.arguments() };
    arguments[index].set_name(name.UTF8String);
    arguments[index].set_step(static_cast<parabolic::postprocessing::Step>(step));
    arguments[index].set_executable(static_cast<parabolic::postprocessing::Executable>(executable));
    arguments[index].set_args(args.UTF8String);
    options.set_arguments(arguments);
    configuration.set_postprocessing_options(options);
}

- (BOOL)validateMaxActiveDownloads:(id*)ioValue error:(NSError**)outError
{
    if (*ioValue == nil)
    {
        *ioValue = @(self.maxActiveDownloads);
        return YES;
    }
    *ioValue = @(std::clamp<NSInteger>([*ioValue integerValue], 1, 10));
    return YES;
}

- (BOOL)validateMaxConnectionsPerServer:(id*)ioValue error:(NSError**)outError
{
    if (*ioValue == nil)
    {
        *ioValue = @(self.maxConnectionsPerServer);
        return YES;
    }
    *ioValue = @(std::clamp<NSInteger>([*ioValue integerValue], 1, 16));
    return YES;
}

- (BOOL)validateMinSplitSize:(id*)ioValue error:(NSError**)outError
{
    if (*ioValue == nil)
    {
        *ioValue = @(self.minSplitSize);
        return YES;
    }
    *ioValue = @(std::clamp<NSInteger>([*ioValue integerValue], 1, 1024));
    return YES;
}

- (BOOL)validatePostprocessingThreads:(id*)ioValue error:(NSError**)outError
{
    if (*ioValue == nil)
    {
        *ioValue = @(self.postprocessingThreads);
        return YES;
    }
    *ioValue = @(std::clamp<NSInteger>([*ioValue integerValue], 1,
                                       static_cast<NSInteger>(std::max(1U, std::thread::hardware_concurrency()))));
    return YES;
}

- (BOOL)validateSpeedLimit:(id*)ioValue error:(NSError**)outError
{
    if (*ioValue == nil)
    {
        *ioValue = @(self.speedLimit);
        return YES;
    }
    *ioValue = @(std::clamp<NSInteger>([*ioValue integerValue], 512, 10240));
    return YES;
}

- (void)setNilValueForKey:(NSString*)key
{
}

@end
