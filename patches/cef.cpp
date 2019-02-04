// web-audio-vst
// cef patches for audio buffer routing
// steps:
// 1. CefBrowserHost::RenderAudio
// 2. _cef_browser_host_t::render_audio
// 3. CefBrowserHostImpl::RenderAudio
//
// files are in chromium/src/cef folder
// jari kleimola 2018-2019


// ----------------------------------------------------------------------------
// 1: include/cef_browser.h
//
class CefBrowserHost : public virtual CefBaseRefCounted {
 public:
  typedef cef_drag_operations_mask_t DragOperationsMask;
  typedef cef_file_dialog_mode_t FileDialogMode;
  typedef cef_mouse_button_type_t MouseButtonType;
  typedef cef_paint_element_type_t PaintElementType;

  // patch -->
	/*--cef()--*/
	virtual int RenderAudio(float** inputs, float** outputs, int nsamples, double curtime, int plugid) = 0;
	// <-- patch

// ----------------------------------------------------------------------------
// 1: libcef_dll/ctocpp/browser_host_ctocpp.h
//
class CefBrowserHostCToCpp : public CefCToCppRefCounted<CefBrowserHostCToCpp,
                                                        CefBrowserHost,
                                                        cef_browser_host_t> {
 public:
  CefBrowserHostCToCpp();

	// patch -->
	int RenderAudio(float** inputs, float** outputs, int nsamples, double curtime, int plugid) OVERRIDE;
	// <-- patch

// ----------------------------------------------------------------------------
// 1: libcef_dll/ctocpp/browser_host_ctocpp.cc
//
// patch -->
int CefBrowserHostCToCpp::RenderAudio(float** inputs, float** outputs, int nsamples, double curtime, int plugid) {
  cef_browser_host_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, render_audio))
    return 2;

  // Execute
  return _struct->render_audio(_struct, inputs, outputs, nsamples, curtime, plugid);
}
// <-- patch


// ----------------------------------------------------------------------------
// 2: include/capi/cef_browser_capi.h
//
typedef struct _cef_browser_host_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

	// patch -->
  int(CEF_CALLBACK* render_audio)(struct _cef_browser_host_t* self,
		float** inputs, float** outputs, int nsamples, double curtime, int plugid);
	// <-- patch

// ----------------------------------------------------------------------------
// 2: cef/libcef_dll/cpptoc/browser_host_cpptoc.cc
//
// patch -->
int CEF_CALLBACK browser_host_render_audio(struct _cef_browser_host_t* self,
	float** inputs, float** outputs, int nsamples, double curtime, int plugid) {
  DCHECK(self);
  if (!self)
    return 3;

  // Execute
  return CefBrowserHostCppToC::Get(self)->RenderAudio(inputs, outputs, nsamples, curtime, plugid);
}
// <-- patch

}  // namespace

CefBrowserHostCppToC::CefBrowserHostCppToC() {
  // patch -->
	GetStruct()->render_audio = browser_host_render_audio;
  // <-- patch


// ----------------------------------------------------------------------------
// 3: libcef/browser/browser_host_impl.h
//
class CefBrowserHostImpl : public CefBrowserHost,
                           public CefBrowser,
                           public content::WebContentsDelegate,
                           public content::WebContentsObserver,
                           public content::NotificationObserver {
 public:
	// patch -->
	int RenderAudio(float** inputs, float** outputs, int nsamples, double curtime, int plugid) override;
	// <-- patch
	
// ----------------------------------------------------------------------------
// 3: libcef/browser/browser_host_impl.cc
//
// patch -->
#include "../../src/media/audio/virtual/audio_manager_virtual.h"
#include "../../src/media/audio/virtual/audio_input_stream_virtual.h"
#include "../../src/media/audio/virtual/audio_output_stream_virtual.h"
using namespace media;

int CefBrowserHostImpl::RenderAudio(float** inputs, float** outputs, int nsamples, double curtime, int plugid)
{
	AudioManagerVirtual* am = (AudioManagerVirtual*)AudioManager::Get();

	if (am) {
		base::TimeTicks now = base::TimeTicks::FromInternalValue(curtime * 1000000);

		// -- push
		AudioInputStreamVirtual* inStream = (AudioInputStreamVirtual*)am->getInputStream(plugid);
		if (inStream)
			inStream->push(inputs, nsamples, now);
		
		// -- pull
		AudioOutputStreamVirtual* outStream = (AudioOutputStreamVirtual*)am->getOutputStream(plugid);
		if (outStream) {
			outStream->pull(outputs, nsamples, now);
			return am->getNumOutStreams();
		}
	}
	
	return 0;
}
// <-- patch
