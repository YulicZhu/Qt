// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_PROXY_AUDIO_INPUT_RESOURCE_H_
#define PPAPI_PROXY_AUDIO_INPUT_RESOURCE_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "base/sync_socket.h"
#include "base/threading/simple_thread.h"
#include "ppapi/proxy/device_enumeration_resource_helper.h"
#include "ppapi/proxy/plugin_resource.h"
#include "ppapi/shared_impl/scoped_pp_resource.h"
#include "ppapi/thunk/ppb_audio_input_api.h"

namespace media {
class AudioBus;
}

namespace ppapi {
namespace proxy {

class ResourceMessageReplyParams;

class AudioInputResource : public PluginResource,
                           public thunk::PPB_AudioInput_API,
                           public base::DelegateSimpleThread::Delegate {
 public:
  AudioInputResource(Connection connection, PP_Instance instance);
  virtual ~AudioInputResource();

  // Resource overrides.
  virtual thunk::PPB_AudioInput_API* AsPPB_AudioInput_API() override;
  virtual void OnReplyReceived(const ResourceMessageReplyParams& params,
                               const IPC::Message& msg) override;

  // PPB_AudioInput_API implementation.
  virtual int32_t EnumerateDevices(
      const PP_ArrayOutput& output,
      scoped_refptr<TrackedCallback> callback) override;
  virtual int32_t MonitorDeviceChange(
      PP_MonitorDeviceChangeCallback callback,
      void* user_data) override;
  virtual int32_t Open0_3(PP_Resource device_ref,
                          PP_Resource config,
                          PPB_AudioInput_Callback_0_3 audio_input_callback_0_3,
                          void* user_data,
                          scoped_refptr<TrackedCallback> callback) override;
  virtual int32_t Open(PP_Resource device_ref,
                       PP_Resource config,
                       PPB_AudioInput_Callback audio_input_callback,
                       void* user_data,
                       scoped_refptr<TrackedCallback> callback) override;
  virtual PP_Resource GetCurrentConfig() override;
  virtual PP_Bool StartCapture() override;
  virtual PP_Bool StopCapture() override;
  virtual void Close() override;

 protected:
  // Resource override.
  virtual void LastPluginRefWasDeleted() override;

 private:
  enum OpenState {
    BEFORE_OPEN,
    OPENED,
    CLOSED
  };

  void OnPluginMsgOpenReply(const ResourceMessageReplyParams& params);

  // Sets the shared memory and socket handles. This will automatically start
  // capture if we're currently set to capture.
  void SetStreamInfo(base::SharedMemoryHandle shared_memory_handle,
                     size_t shared_memory_size,
                     base::SyncSocket::Handle socket_handle);

  // Starts execution of the audio input thread.
  void StartThread();

  // Stops execution of the audio input thread.
  void StopThread();

  // DelegateSimpleThread::Delegate implementation.
  // Run on the audio input thread.
  virtual void Run() override;

  int32_t CommonOpen(PP_Resource device_ref,
                     PP_Resource config,
                     PPB_AudioInput_Callback_0_3 audio_input_callback_0_3,
                     PPB_AudioInput_Callback audio_input_callback,
                     void* user_data,
                     scoped_refptr<TrackedCallback> callback);

  OpenState open_state_;

  // True if capturing the stream.
  bool capturing_;

  // Socket used to notify us when new samples are available. This pointer is
  // created in SetStreamInfo().
  scoped_ptr<base::CancelableSyncSocket> socket_;

  // Sample buffer in shared memory. This pointer is created in
  // SetStreamInfo(). The memory is only mapped when the audio thread is
  // created.
  scoped_ptr<base::SharedMemory> shared_memory_;

  // The size of the sample buffer in bytes.
  size_t shared_memory_size_;

  // When the callback is set, this thread is spawned for calling it.
  scoped_ptr<base::DelegateSimpleThread> audio_input_thread_;

  // Callback to call when new samples are available.
  PPB_AudioInput_Callback_0_3 audio_input_callback_0_3_;
  PPB_AudioInput_Callback audio_input_callback_;

  // User data pointer passed verbatim to the callback function.
  void* user_data_;

  // The callback is not directly passed to OnPluginMsgOpenReply() because we
  // would like to be able to cancel it early in Close().
  scoped_refptr<TrackedCallback> open_callback_;

  // Owning reference to the current config object. This isn't actually used,
  // we just dish it out as requested by the plugin.
  ScopedPPResource config_;

  DeviceEnumerationResourceHelper enumeration_helper_;

  // The data size (in bytes) of one second of audio input. Used to calculate
  // latency.
  size_t bytes_per_second_;

  // AudioBus for shuttling data across the shared memory.
  scoped_ptr<media::AudioBus> audio_bus_;
  int sample_frame_count_;

  // Internal buffer for client's integer audio data.
  int client_buffer_size_bytes_;
  scoped_ptr<uint8_t[]> client_buffer_;

  DISALLOW_COPY_AND_ASSIGN(AudioInputResource);
};

}  // namespace proxy
}  // namespace ppapi

#endif  // PPAPI_PROXY_AUDIO_INPUT_RESOURCE_H_