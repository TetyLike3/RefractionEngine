
#include <steam/isteamnetworkingutils.h>

#include "EditorNet.h"

namespace Refraction::Editor {
	ISteamNetworkingSockets* EditorNet::Interface = nullptr;
	bool EditorNet::IsServer = false;
	bool EditorNet::ShouldClose = false;

	HSteamListenSocket EditorNet::ListenSocket = {};
	HSteamNetPollGroup EditorNet::PollGroup = {};
	HSteamNetConnection EditorNet::Connection = {};

	std::map<HSteamNetConnection, EditorNet::ClientProfile> EditorNet::ClientMap = {};


	bool EditorNet::InitServer(uint16_t port) {
		Log::Editor.Info("Initialising live collaboration server...");
		Interface = SteamNetworkingSockets();

		SteamNetworkingIPAddr localAddress;
		localAddress.Clear();
		localAddress.m_port = port;
		SteamNetworkingConfigValue_t opt;
		opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnectionStatusChangedCallback);

		ListenSocket = Interface->CreateListenSocketIP(localAddress, 1, &opt);
		if (ListenSocket == k_HSteamListenSocket_Invalid) {
			Log::Editor.Error("Failed to establish server listening socket at port " + port);
			return false;
		}

		PollGroup = Interface->CreatePollGroup();
		if (PollGroup == k_HSteamNetPollGroup_Invalid) {
			Log::Editor.Error("Failed to establish server poll group at port " + port);
			return false;
		}

		Log::Editor.Info("Successfully initialised live collaboration server with port " + port);
		return true;
	}

	bool EditorNet::InitClient() {
		Log::Editor.Info("Initialising live collaboration client...");
		Interface = SteamNetworkingSockets();
		return true;
	}

	bool EditorNet::ConnectToServer(const SteamNetworkingIPAddr& serverAddress) {
		char serverAddrCStr[SteamNetworkingIPAddr::k_cchMaxString]{};
		serverAddress.ToString(serverAddrCStr, sizeof(serverAddrCStr), true);
		Log::Editor.Info("Connecting to live collaboration server at " + std::string(serverAddrCStr) + "...");

		SteamNetworkingConfigValue_t opt{};
		opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnectionStatusChangedCallback);
		Connection = Interface->ConnectByIPAddress(serverAddress, 1, &opt);

		return true;
	}

	void EditorNet::BroadcastMessage(std::string message) {
		if (!IsServer) {
			Log::Editor.Warn("Attempt to broadcast message as client");
			return;
		}


	}

	std::vector<std::string> EditorNet::PollIncomingMessages() {
		std::vector<std::string> msgs = {};
		while (!ShouldClose) {
			ISteamNetworkingMessage* incomingMsg = nullptr;
			int msgCount = Interface->ReceiveMessagesOnConnection(Connection, &incomingMsg, 1);
			if (msgCount == 0) break;
			if (msgCount < 0) {
				Log::Editor.Error("Error while retrieving messages from connection");
				break;
			}

			msgs.push_back(std::string((char*)incomingMsg->GetData(), incomingMsg->GetSize()));
			incomingMsg->Release();
		}
		Interface->RunCallbacks();
		return msgs;
	}

	void EditorNet::SendChangeRequest(std::string serialisedChange) {
		Interface->SendMessageToConnection(Connection, serialisedChange.c_str(), (uint32)serialisedChange.size(), k_nSteamNetworkingSend_Reliable, nullptr);
	}

	void EditorNet::Shutdown() {
		if (IsServer) {

		} else {
			Log::Editor.Info("Disconnecting from live collaboration server...");
			ShouldClose = true;
			Interface->CloseConnection(Connection, k_ESteamNetConnectionEnd_App_Generic, "User Disconnect", true);
		}
	}

	void EditorNet::ConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* info) {
		if (IsServer) {

		} else {
			if (!(info->m_hConn == Connection || Connection == k_HSteamNetConnection_Invalid)) {
				Log::Editor.Error("Invalid connection");
				return;
			}

			switch (info->m_info.m_eState) {
			case k_ESteamNetworkingConnectionState_None:
				break;
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				ShouldClose = true;

				if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
					Log::Editor.Error("Failed to establish connection with live collaboration server: " + std::string(info->m_info.m_szEndDebug));
				} else if (info->m_eOldState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
					Log::Editor.Error("Lost connection from live collaboration server: " + std::string(info->m_info.m_szEndDebug));
				} else {
					Log::Editor.Warn("Live collaboration server closed the connection: " + std::string(info->m_info.m_szEndDebug));
				}

				Interface->CloseConnection(info->m_hConn, 0, nullptr, false);
				Connection = k_HSteamNetConnection_Invalid;
				break;
			case k_ESteamNetworkingConnectionState_Connecting:
				Log::Editor.Info("Establishing connection to live collaboration server...");
				break;
			case k_ESteamNetworkingConnectionState_Connected:
				Log::Editor.Info("Successfully connected to live collaboration server");
				break;
			default:
				break;
			}
		}
	}


}