// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_VIRTUAL_AUDIO_OUTPUT_STREAM_VIRTUAL_H_
#define MEDIA_AUDIO_VIRTUAL_AUDIO_OUTPUT_STREAM_VIRTUAL_H_

#include <stdint.h>

#include "base/threading/thread_checker.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_manager.h"
#include "media/base/audio_parameters.h"


namespace media {
class AudioManagerVirtual;

class MEDIA_EXPORT AudioOutputStreamVirtual : public AudioOutputStream
{
 public:
	AudioOutputStreamVirtual(AudioManagerVirtual* manager,
													 const AudioParameters& params,
                           const AudioManager::LogCallback& log_callback);

  ~AudioOutputStreamVirtual() override;

  // AudioOutputStream:
  bool Open() override;
  void Start(AudioSourceCallback* callback) override;
  void Stop() override;
  void SetVolume(double volume) override;
  void GetVolume(double* volume) override;
  void Close() override;

	// virtual audio stuff, cef calls this
	int pull(float** outputs, uint32_t nsamples, base::TimeTicks& now);

 private:
	// AudioManagerVirtual* const manager_;
	const AudioParameters params_;
	const size_t number_of_frames_;
  AudioSourceCallback* graph_;
	std::unique_ptr<AudioBus> output_bus_;
  base::ThreadChecker thread_checker_;
 
  double volume_;

  DISALLOW_COPY_AND_ASSIGN(AudioOutputStreamVirtual);
};

}  // namespace media

#endif  // MEDIA_AUDIO_VIRTUAL_AUDIO_OUTPUT_STREAM_VIRTUAL_H_
