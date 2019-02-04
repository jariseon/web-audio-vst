// web-audio-vst
// chromium browser process patches
// virtual audio streams
//
// these files are in chromium/src/media/audio/virtual folder
// jari kleimola 2018-2019

// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/virtual/audio_manager_virtual.h"

#include <memory>

#include "base/threading/thread_checker.h"
#include "media/audio/virtual/audio_input_stream_virtual.h"
#include "media/audio/virtual/audio_output_stream_virtual.h"
#include "media/base/audio_parameters.h"
#include "media/base/channel_layout.h"

namespace media {

static const int kMaxOutputStreams = 50;

AudioManagerVirtual::AudioManagerVirtual(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory)
    : AudioManagerBase(std::move(audio_thread), audio_log_factory)
{
	SetMaxOutputStreamsAllowed(kMaxOutputStreams);
	
	// WARNING: This is executed on the UI loop, do not add any code here which
	// loads libraries or attempts to call out into the OS.  Instead add such code
	// to the InitializeOnAudioThread() method below.

	// Task must be posted last to avoid races from handing out "this" to the
	// audio thread. Always PostTask even if we're on the right thread since
	// AudioManager creation is on the startup path and this may be slow.
	
  GetTaskRunner()->PostTask(
      FROM_HERE, base::Bind(&AudioManagerVirtual::InitializeOnAudioThread,
                            base::Unretained(this)));
}

void AudioManagerVirtual::InitializeOnAudioThread()
{
	DCHECK(GetTaskRunner()->BelongsToCurrentThread());
}

AudioManagerVirtual::~AudioManagerVirtual() // = default;
{
	for (auto stream : output_streams_)
		ReleaseOutputStream(stream);
	output_streams_.clear();
}

const char* AudioManagerVirtual::GetName() { return "Virtual"; }


// ---------------------------------------------------------------------------------------------------
// devices
//

bool AudioManagerVirtual::HasAudioOutputDevices() { return true; }
bool AudioManagerVirtual::HasAudioInputDevices()  { return true; }

void AudioManagerVirtual::GetAudioInputDeviceNames(AudioDeviceNames* device_names)
{
  device_names->clear();
	device_names->push_back(AudioDeviceName::CreateDefault());
}

void AudioManagerVirtual::GetAudioOutputDeviceNames(AudioDeviceNames* device_names)
{
	device_names->clear();
	device_names->push_back(AudioDeviceName::CreateDefault());
}


// ---------------------------------------------------------------------------------------------------
// streams
//

AudioParameters AudioManagerVirtual::GetInputStreamParameters(const std::string& device_id)
{
	return getStreamParameters();
}

AudioParameters AudioManagerVirtual::GetPreferredOutputStreamParameters(
    const std::string& output_device_id,
    const AudioParameters& input_params)
{
	return getStreamParameters();
}

AudioParameters AudioManagerVirtual::getStreamParameters()
{
	DCHECK(GetTaskRunner()->BelongsToCurrentThread());
	
	// -- hardcode for now
	// -- todo: should get from VST host
	int nchannels = 2;
	int sampleRate = 44100;
	int bitsPerSample = 32;
	int samplesPerBuffer = 128; // GetUserBufferSize();
	
	AudioParameters params(
		AudioParameters::AUDIO_PCM_LOW_LATENCY,
		getChannelLayout(nchannels),
    sampleRate, bitsPerSample, samplesPerBuffer);
	
	if (nchannels > 2)
		params.set_channels_for_discrete(nchannels);
	
	return params;
}

ChannelLayout AudioManagerVirtual::getChannelLayout(int nchannels)
{
	switch (nchannels) {
		case 0:  return CHANNEL_LAYOUT_NONE;
		case 1:  return CHANNEL_LAYOUT_MONO;
		case 2:	 return CHANNEL_LAYOUT_STEREO;
		default: return CHANNEL_LAYOUT_DISCRETE;
	}
}

AudioOutputStream* AudioManagerVirtual::MakeLinearOutputStream(
    const AudioParameters& params,
    const LogCallback& log_callback)
{
	DCHECK(GetTaskRunner()->BelongsToCurrentThread());
	DCHECK_EQ(AudioParameters::AUDIO_PCM_LINEAR, params.format());
	
	return MakeLowLatencyOutputStream(params, std::string(), log_callback);
}

AudioOutputStream* AudioManagerVirtual::MakeLowLatencyOutputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback)
{
	DCHECK(GetTaskRunner()->BelongsToCurrentThread());
	
	auto* stream = new AudioOutputStreamVirtual(this, params, log_callback);
	output_streams_.push_back(stream);
	return stream;
}

AudioInputStream* AudioManagerVirtual::MakeLinearInputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback)
{
	DCHECK(GetTaskRunner()->BelongsToCurrentThread());
	DCHECK_EQ(AudioParameters::AUDIO_PCM_LINEAR, params.format());
	
	return MakeLowLatencyInputStream(params, device_id, log_callback);
}

AudioInputStream* AudioManagerVirtual::MakeLowLatencyInputStream(
    const AudioParameters& params,
    const std::string& device_id,
    const LogCallback& log_callback)
{
  DCHECK(GetTaskRunner()->BelongsToCurrentThread());

  auto* stream = new AudioInputStreamVirtual(this, params, log_callback);
	input_streams_.push_back(stream);
  return stream;
}


// ---------------------------------------------------------------------------------------------------
// virtual
//

AudioInputStreamVirtual* AudioManagerVirtual::getInputStream(uint32_t index)
{
	if (index < input_streams_.size())
		return input_streams_[index];
	return nullptr;
}

AudioOutputStreamVirtual* AudioManagerVirtual::getOutputStream(uint32_t index)
{
	if (index < output_streams_.size())
		return output_streams_[index];
	return nullptr;
}


// static
std::unique_ptr<AudioManager> CreateAudioManager(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory) {
  return std::make_unique<AudioManagerVirtual>(std::move(audio_thread),
                                               audio_log_factory);
}

}  // namespace media
