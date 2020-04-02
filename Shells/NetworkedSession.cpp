
#include "NetworkedSession.h"

ANetworkedSession::ANetworkedSession() {
	// Is voice enabled always or via a push to talk keybinding
	bRequiresPushToTalk = true;

	// Restrictions on the largest party that can join together
	MaxPartySize = 4;

	// Maximum number of players allowed by this server.
	MaxPlayers = 4;

	// Maximum number of spectators allowed by this server.
	MaxSpectators = 4;

	// Maximum number of splitscreen players to allow from one connection
	MaxSplitscreensPerConnection = 2;

	// SessionName local copy from PlayerState class.
	SessionName = "NetworkedSession";
}