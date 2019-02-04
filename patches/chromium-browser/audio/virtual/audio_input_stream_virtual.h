// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_VIRTUAL_AUDIO_INPUT_STREAM_VIRTUAL_H_
#define MEDIA_AUDIO_VIRTUAL_AUDIO_INPUT_STREAM_VIRTUAL_H_

#include "base/threading/thread_checker.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_manager.h"
#include "media/base/audio_parameters.h"

namespace media {
class AudioManagerVirtual;

class MEDIA_EXPORT AudioInputStreamVirtual : public AudioInputStream
{
 public:
	AudioInputStreamVirtual(AudioManagerVirtual* manager,
													 const AudioParameters& params,
                           const AudioManager::LogCallback& log_callback);

  ~AudioInputStreamVirtual() override;

  // AudioInputStream:
  bool Open() override;
  void Close() override;
  void Start(AudioInputCallback* callback) override;
  void Stop() override;
  double GetMaxVolume() override;
  void SetVolume(double volume) override;
  double GetVolume() override;
  bool SetAutomaticGainControl(bool enabled) override;
  bool GetAutomaticGainControl() override;
  bool IsMuted() override;

	// -- virtual audio stuff, cef calls this
	void push(float** inputs, uint32_t nsamples, base::TimeTicks& now);

 private:
  AudioInputCallback* sink_;
  AudioParameters params_;
	std::unique_ptr<AudioBus> input_bus_;
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(AudioInputStreamVirtual);
};

}  // namespace media

#endif  // MEDIA_AUDIO_VIRTUAL_AUDIO_INPUT_STREAM_VIRTUAL_H_
