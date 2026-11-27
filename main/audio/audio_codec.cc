#include "audio_codec.h"
#include <esp_timer.h>
#include "board.h"
#include "settings.h"

#include <esp_log.h>
#include <cstring>
#include <driver/i2s_common.h>

#define TAG "AudioCodec"

AudioCodec::AudioCodec() {
}

AudioCodec::~AudioCodec() {
}

void AudioCodec::OutputData(std::vector<int16_t>& data) {
#ifdef CONFIG_USE_TERMINAL_FACE
    unsigned sum = 0;
    unsigned count = 0;
    for (size_t i = 0; i < data.size(); i += 16) {
        const int sample = data[i];
        sum += sample < 0 ? -sample : sample;
        ++count;
    }
    const unsigned level = count ? sum / count : 0;
    const unsigned mouth = level > 3200 ? 3 : level > 1200 ? 2 : level > 350 ? 1 : 0;
    const auto ms = static_cast<uint32_t>(esp_timer_get_time() / 1000);
    playback_mouth_.store((ms & ~3u) | mouth, std::memory_order_relaxed);
#endif
    Write(data.data(), data.size());
}

#ifdef CONFIG_USE_TERMINAL_FACE
unsigned AudioCodec::GetPlaybackMouthLevel() const {
    const auto value = playback_mouth_.load(std::memory_order_relaxed);
    const auto ms = static_cast<uint32_t>(esp_timer_get_time() / 1000);
    return ms - (value & ~3u) < 180 ? value & 3u : 0;
}
#endif

bool AudioCodec::InputData(std::vector<int16_t>& data) {
    int samples = Read(data.data(), data.size());
    if (samples > 0) {
        return true;
    }
    return false;
}

void AudioCodec::Start() {
    Settings settings("audio", false);
    output_volume_ = settings.GetInt("output_volume", output_volume_);
    if (output_volume_ <= 0) {
        ESP_LOGW(TAG, "Output volume value (%d) is too small, setting to default (10)", output_volume_);
        output_volume_ = 10;
    }

    ESP_LOGI(TAG, "Audio codec started");
}

void AudioCodec::SetOutputVolume(int volume) {
    output_volume_ = volume;
    ESP_LOGI(TAG, "Set output volume to %d", output_volume_);
    
    Settings settings("audio", true);
    settings.SetInt("output_volume", output_volume_);
}

void AudioCodec::SetInputGain(float gain) {
    input_gain_ = gain;
    ESP_LOGI(TAG, "Set input gain to %.1f", input_gain_);
}

void AudioCodec::EnableInput(bool enable) {
    if (enable == input_enabled_) {
        return;
    }
    input_enabled_ = enable;
    ESP_LOGI(TAG, "Set input enable to %s", enable ? "true" : "false");
}

void AudioCodec::EnableOutput(bool enable) {
    if (enable == output_enabled_) {
        return;
    }
    output_enabled_ = enable;
    ESP_LOGI(TAG, "Set output enable to %s", enable ? "true" : "false");
}
