// web-audio-vst
// chromium browser process patches
// midi: virtual midi ports (optional, not used in iteration #1)
//
// jari kleimola 2018-2019


// ----------------------------------------------------------------------------
// midi_manager_mac.cc
// create virtual MIDI ports
//
void MidiManagerMac::InitializeCoreMIDI() {
  DCHECK(service()->task_service()->IsOnTaskRunner(kClientTaskRunner));
	
	// patch -->
	destinations_.resize(1);
	sources_.resize(1);
	
	int iportid = 0;
	int oportid = 1;
	std::string iid = IntToString(iportid);
	std::string oid = IntToString(oportid);
	std::string manufacturer = "acme";
	std::string iname = "virtual in #";
	std::string oname = "virtual out #";
	std::string version = "0.1";
  const PortState state = PortState::OPENED;
	
	iname += iid;
	oname += oid;
  MidiPortInfo iinfo = MidiPortInfo(iid, manufacturer, iname, version, state);
  MidiPortInfo oinfo = MidiPortInfo(oid, manufacturer, oname, version, state);
	AddInputPort(iinfo);
	AddOutputPort(oinfo);
	// <-- patch
