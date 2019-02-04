// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/virtual/audio_output_stream_virtual.h"

#include "media/audio/virtual/audio_manager_virtual.h"
#include "media/base/audio_timestamp_helper.h"


namespace media {

AudioOutputStreamVirtual::AudioOutputStreamVirtual(
		AudioManagerVirtual* manager,
		const AudioParameters& params,
		const AudioManager::LogCallback& log_callback)
	:
	// manager_(manager),
	params_(params),
	number_of_frames_(params_.frames_per_buffer()),
	graph_(nullptr)
{
	DCHECK(params_.IsValid());
}

AudioOutputStreamVirtual::~AudioOutputStreamVirtual()
{
  DCHECK(thread_checker_.CalledOnValidThread());
}

bool AudioOutputStreamVirtual::Open()
{
  DCHECK(thread_checker_.CalledOnValidThread());
	output_bus_ = AudioBus::CreateWrapper(params_.channels());
  return true;
}

void AudioOutputStreamVirtual::Close()
{
  DCHECK(thread_checker_.CalledOnValidThread());
  Stop();
}

void AudioOutputStreamVirtual::Start(AudioSourceCallback* callback)
{
  DCHECK(thread_checker_.CalledOnValidThread());
  graph_ = callback;
}

void AudioOutputStreamVirtual::Stop()
{
  DCHECK(thread_checker_.CalledOnValidThread());
	graph_ = nullptr;
}

void AudioOutputStreamVirtual::SetVolume(double volume)
{
  DCHECK(thread_checker_.CalledOnValidThread());
  volume_ = volume;
}

void AudioOutputStreamVirtual::GetVolume(double* volume)
{
  DCHECK(thread_checker_.CalledOnValidThread());
  *volume = volume_;
}

// ----------------------------------------------------------------------------

int AudioOutputStreamVirtual::pull(float** outputs, uint32_t nsamples, base::TimeTicks& now)
{
	// UpdatePlayoutTimestamp(output_time_stamp);
	uint32_t current_lost_frames_ = 0;
	
	// -- WrapBufferList
	AudioBus* outbus = output_bus_.get();
	const int channels = outbus->channels();
	for (int i = 0; i < channels; ++i)
		outbus->SetChannelData(i, outputs[i]);
	outbus->set_frames(nsamples);
	
	// -- current_playout_time_ = GetPlayoutTime(output_time_stamp);
	base::TimeTicks current_playout_time_ = now; // base::TimeTicks::Now();

	// -- fifo
	
	// -- ProvideInput
	int frame_delay = 0;
	const base::TimeTicks playout_time =
			current_playout_time_ +
			AudioTimestampHelper::FramesToTime(frame_delay, params_.sample_rate());
	// const base::TimeTicks now = base::TimeTicks::Now();
	const base::TimeDelta delay = playout_time - now;
	
	if (graph_) {
		graph_->OnMoreData(delay, now, current_lost_frames_, outbus);
		// outbus->Scale(volume_);
		return -2;
	}
	else return -3;
}

}  // namespace media
