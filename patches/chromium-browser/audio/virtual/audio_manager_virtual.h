// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_VIRTUAL_AUDIO_MANAGER_VIRTUAL_H_
#define MEDIA_AUDIO_VIRTUAL_AUDIO_MANAGER_VIRTUAL_H_

#include <stdint.h>
#include <vector>

#include "media/audio/audio_io.h"
#include "media/audio/audio_manager_base.h"


namespace media {

class AudioInputStreamVirtual;
class AudioOutputStreamVirtual;

class AudioManagerVirtual : public AudioManagerBase {
 public:
  AudioManagerVirtual(std::unique_ptr<AudioThread> audio_thread,
                      AudioLogFactory* audio_log_factory);
  ~AudioManagerVirtual() override;

  // Implementation of AudioManager.
  bool HasAudioOutputDevices() override;
  bool HasAudioInputDevices() override;
  void GetAudioInputDeviceNames(AudioDeviceNames* device_names) override;
  void GetAudioOutputDeviceNames(AudioDeviceNames* device_names) override;
  AudioParameters GetInputStreamParameters(
      const std::string& device_id) override;
  const char* GetName() override;

  // Implementation of AudioManagerBase.
  AudioOutputStream* MakeLinearOutputStream(
      const AudioParameters& params,
      const LogCallback& log_callback) override;
  AudioOutputStream* MakeLowLatencyOutputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;
  AudioInputStream* MakeLinearInputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;
  AudioInputStream* MakeLowLatencyInputStream(
      const AudioParameters& params,
      const std::string& device_id,
      const LogCallback& log_callback) override;
 protected:
  AudioParameters GetPreferredOutputStreamParameters(
      const std::string& output_device_id,
      const AudioParameters& input_params) override;

 // -- expose streams to CEF
 public:
	AudioInputStreamVirtual* getInputStream(uint32_t index = 0);
	AudioOutputStreamVirtual* getOutputStream(uint32_t index = 0);
	int getNumOutStreams() { return input_streams_.size(); }
	
 private:
	std::vector<AudioInputStreamVirtual*>  input_streams_;
	std::vector<AudioOutputStreamVirtual*> output_streams_;

	void InitializeOnAudioThread();
	ChannelLayout getChannelLayout(int nchannels);
	AudioParameters getStreamParameters();

  DISALLOW_COPY_AND_ASSIGN(AudioManagerVirtual);
};

} // namespace media

#endif  // MEDIA_AUDIO_VIRTUAL_AUDIO_MANAGER_VIRTUAL_H_
