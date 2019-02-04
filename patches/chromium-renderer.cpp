// web-audio-vst
// chromium renderer process patches
// audio: enable autoplay
// midi: grant MIDI access
//
// jari kleimola 2018-2019


// ----------------------------------------------------------------------------
// BaseAudioContext.cpp
// enable autoplay
//
bool BaseAudioContext::IsAllowedToStart() const {
	// patch -->
	return true;
	// <-- patch


// ----------------------------------------------------------------------------
// MIDIAccessInitializer.cpp
// grants MIDI access without user action
//
void MIDIAccessInitializer::OnPermissionsUpdated(PermissionStatus status) {
	// original:
  /* permission_service_.reset();
  if (status == PermissionStatus::GRANTED)
    accessor_->StartSession();
  else
    Reject(DOMException::Create(kSecurityError)); */

	// patch:
	accessor_->StartSession();
}

void MIDIAccessInitializer::OnPermissionUpdated(PermissionStatus status) {
  permission_service_.reset();
	
  // original:
  /* if (status == PermissionStatus::GRANTED)
    accessor_->StartSession();
  else
    Reject(DOMException::Create(kSecurityError)); */
	
	// patch:
	accessor_->StartSession();
}
