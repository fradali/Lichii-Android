// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "audio_core/audio_core.h"
#include "audio_core/sink/sink_details.h"
#include "common/settings.h"
#include "core/core.h"

namespace AudioCore {

AudioCore::AudioCore(Core::System& system) {
    audio_manager.emplace();
    CreateSinks();
    // Must be created after the sinks
    adsp.emplace(system, *output_sink);
}

AudioCore ::~AudioCore() {
    Shutdown();
}

void AudioCore::CreateSinks() {
    const auto& sink_id{Settings::values.sink_id};
    const auto& audio_output_device_id{Settings::values.audio_output_device_id};
    const auto& audio_input_device_id{Settings::values.audio_input_device_id};

    output_sink = Sink::CreateSinkFromID(sink_id.GetValue(), audio_output_device_id.GetValue());
#ifdef __ANDROID__
    // SDL3 opens an Android recording device only after RECORD_AUDIO is granted and blocks until
    // SDLActivity reports the result. The app neither declares nor forwards that permission, so a
    // capture stream would hang the audio service (Ocarina of Time froze on a black screen when
    // it opened its microphone stream). Capture stays silent on Android.
    input_sink =
        Sink::CreateSinkFromID(Settings::AudioEngine::Null, audio_input_device_id.GetValue());
#else
    input_sink = Sink::CreateSinkFromID(sink_id.GetValue(), audio_input_device_id.GetValue());
#endif
    input_sink->SetDeviceVolume(Settings::values.microphone_volume.GetValue() / 100.0f);
}

void AudioCore::Shutdown() {
    audio_manager->Shutdown();
}

AudioManager& AudioCore::GetAudioManager() {
    return *audio_manager;
}

Sink::Sink& AudioCore::GetOutputSink() {
    return *output_sink;
}

Sink::Sink& AudioCore::GetInputSink() {
    return *input_sink;
}

ADSP::ADSP& AudioCore::ADSP() {
    return *adsp;
}

} // namespace AudioCore
