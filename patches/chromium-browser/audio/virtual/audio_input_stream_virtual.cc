// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/virtual/audio_input_stream_virtual.h"
#include "media/audio/virtual/audio_manager_virtual.h"
#include "media/base/audio_timestamp_helper.h"

namespace media {

AudioInputStreamVirtual::AudioInputStreamVirtual(
		AudioManagerVirtual* manager,
		const AudioParameters& params,
		const AudioManager::LogCallback& log_callback)
	: sink_(nullptr),
		params_(params),
		input_bus_(AudioBus::Create(params))
{
  DCHECK(params_.IsValid());
}

AudioInputStreamVirtual::~AudioInputStreamVirtual()
{
  DCHECK(!sink_);
}

bool AudioInputStreamVirtual::Open()
{
	DCHECK(thread_checker_.CalledOnValidThread());
	input_bus_ = AudioBus::CreateWrapper(params_.channels());
	return true;
}

void AudioInputStreamVirtual::Close()
{
  DCHECK(thread_checker_.CalledOnValidThread());
  Stop();
}

void AudioInputStreamVirtual::Start(AudioInputCallback* callback)
{
  DCHECK(thread_checker_.CalledOnValidThread());
  sink_ = callback;
}

void AudioInputStreamVirtual::Stop()
{
  DCHECK(thread_checker_.CalledOnValidThread());
  sink_ = nullptr;
}


// ---------------------------------------------------------------------------------------------------
// volume
//

double AudioInputStreamVirtual::GetMaxVolume() { return 1.0; }
void AudioInputStreamVirtual::SetVolume(double volume) {}
double AudioInputStreamVirtual::GetVolume() { return 1.0; }
bool AudioInputStreamVirtual::SetAutomaticGainControl(bool enabled) { return false; }
bool AudioInputStreamVirtual::GetAutomaticGainControl() { return false; }
bool AudioInputStreamVirtual::IsMuted() { return false; }

// ----------------------------------------------------------------------------

void AudioInputStreamVirtual::push(float** inputs, uint32_t nsamples, base::TimeTicks& now)
{
	base::TimeTicks capture_time = now; // base::TimeTicks::Now();
	
	// no fifo
	AudioBus* inbus = input_bus_.get();
	const int channels = inbus->channels();
	for (int i = 0; i < channels; ++i)
		inbus->SetChannelData(i, inputs[i]);
	inbus->set_frames(nsamples);
	
	if (sink_)
		sink_->OnData(inbus, capture_time, 1.0);
	
	// capture_time += AudioTimestampHelper::FramesToTime(nsamples, params_.sample_rate());
}

}  // namespace media
