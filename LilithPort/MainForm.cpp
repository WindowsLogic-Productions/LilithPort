#include "stdafx.h"
#include "MainForm.h"

using namespace LilithPort;

void MainForm::Begin()
{
	ServerName = gcnew String(MTOPTION.SERVER_NAME);
	ConnectIP = gcnew String(MTOPTION.CONNECTION_IP);
	
	ServerMode = SM_NORMAL;
	MTINFO.VERSION_CHECKED = false;

	MemberInfo^ me = gcnew MemberInfo;
	me->IP_EP    = gcnew IPEndPoint(0, 0);
	me->ID       = 0;
	me->NAME     = gcnew String(MTOPTION.NAME);
	me->COMMENT  = gcnew String(MTOPTION.COMMENT);
	me->LOOKING  = gcnew String(MTOPTION.LOOKING);
	me->AVOIDING  = gcnew String(MTOPTION.AVOIDING);
	me->TYPE     = MTOPTION.CONNECTION_TYPE;
	me->STATE    = MS_FREE;
	me->RESPONSE = timeGetTime();

	if(MTOPTION.CONNECTION_TYPE == CT_SERVER && ServerName->Length > 0){

		MemberInfoBackUp^ bk = gcnew MemberInfoBackUp;
		bk->IP_EP    = gcnew IPEndPoint(0, 0);
		bk->ID       = 0;

		// “ÁŽêƒT[ƒoƒ‚[ƒh”»’è
		if(ServerName[0] == '#'){
			ServerMode = SM_NORA;
			ListView = LV_BLIND;
			me->NAME = gcnew String("?");
			me->COMMENT = String::Empty;
			me->LOOKING = String::Empty;
			me->AVOIDING = String::Empty;

			MemberList->Add(me);
			MemberListBackUp->Add(bk);
			listBoxMember->Items->Add(gcnew String("Anonymous Mode"));
		}
		else{
			MemberList->Add(me);
			MemberListBackUp->Add(bk);
			listBoxMember->Items->Add(me->NAME);

			if(ServerName[0] == '+'){
				ServerMode = SM_MIX;
			}
			else if(ServerName[0] == '@'){
				ServerMode = SM_MATCH;
			}
		}
	}
	else{
		MemberList->Add(me);
		listBoxMember->Items->Add(me->NAME);
	}

	try{
		// ‰ñüÝ’è
		if(MTOPTION.CONNECTION_TYPE == CT_SERVER || MTOPTION.CONNECTION_TYPE == CT_HOST){

			UDP = gcnew UdpClient(MTOPTION.OPEN_PORT);
		}
		else if(MTOPTION.CONNECTION_TYPE == CT_CLIENT){
			UDP = gcnew UdpClient;
		}
	}
	catch(SocketException^ e){
		UDP = nullptr;

		if(e->ErrorCode == WSAEADDRINUSE){
			WriteMessage("IP/Port already in use. Close all open versions of LilithPort and try again.\n", ErrorMessageColor);
		}
		else{
			WriteMessage(String::Format("ERROR: UDP initialiation failed.\n", e->ErrorCode), ErrorMessageColor);
			if(MTINFO.DEBUG){
				WriteMessage(e->ToString() + "\n", DebugMessageColor);
			}
		}
	}

	// ƒlƒbƒg‚ÉÚ‘±
	if(UDP != nullptr && MTOPTION.CONNECTION_TYPE != CT_FREE){

		// Ý’è‚ð•Û‘¶
		SaveMTOption();

		// Ž©“®‹xŒeƒXƒŒƒbƒhŠJŽn
		if(MTOPTION.AUTO_REST && MemberList[0]->STATE == MS_FREE){
			AutoRestThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::RunAutoRest));
			AutoRestThread->Start();
		}

		if(MTOPTION.CONNECTION_TYPE == CT_SERVER){

			// ŽIŽóMŠJŽn
			UDP->BeginReceive(gcnew AsyncCallback(ReceivePackets), this);

			// ƒ\ƒi[‹N“®
			Ranging = true;
			SonarThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::RunSonar));
			SonarThread->Start();

			this->Text += String::Format("  [{0}] [Server Port:{1}]", ServerName, MTOPTION.OPEN_PORT);
			WriteTime(0, SystemMessageColor);
			WriteMessage("Server Initializing\n[MOTD]---------------------\n", SystemMessageColor);

			// WelcomeƒƒbƒZ[ƒW‚Ì•\Ž¦
			ReplaceWelcomeTab(true);
			richTextBoxLog->SelectionFont = gcnew Drawing::Font(richTextBoxLog->Font->FontFamily, richTextBoxLog->Font->Size + 2);
			richTextBoxLog->SelectionColor = TalkMessageColor;
			richTextBoxLog->SelectionBackColor = NoticeBackColor;
			richTextBoxLog->AppendText(gcnew String(MTOPTION.WELCOME)+"\n");

			WriteMessage("-------------------------------\n", SystemMessageColor);
			
			// IPƒAƒhƒŒƒX‚ðŽæ“¾‚µ‚Ä•\Ž¦
			if(MTOPTION.GET_IP_ENABLE){
				GetIPAddress();
			}

		}
		else{
			// ƒNƒ‰ƒCƒAƒ“ƒg
			_int64 address;
			int port = MTOPTION.PORT;
			array<String^>^ host = ConnectIP->Split(gcnew array<wchar_t>{':'}, 2, StringSplitOptions::RemoveEmptyEntries);

			// ƒ|[ƒg":"Žw’è
			if(host->Length > 1){
				try{
					port = Convert::ToInt32(host[1]);
				}
				catch(Exception^){
					port = MTOPTION.PORT;
				}
			}

			try{

				// MTSPアドレス接続
				if(host[0]->Length == 5){
					try{
						if(MTINFO.DEBUG){
							WriteMessage("Converting MTSP address\n", DebugMessageColor);
						}
						address = MTDecryptionIP(host[0]);
					}
					catch(Exception^ e){
						if(MTINFO.DEBUG){
							WriteMessage("Converting MTSP address\n", ErrorMessageColor);
							WriteMessage(e->ToString() + "\n", DebugMessageColor);
						}
					}
				}

				// 変換アドレス接続(ASCII)
				if(address == 0){
					try{
						if(MTINFO.DEBUG){
							WriteMessage("Converting ASCII address...\n", DebugMessageColor);
						}
						address = DecryptionIP(host[0], true);
					}
					catch(Exception^ e){
						if(MTINFO.DEBUG){
							WriteMessage("ASCII address conversion failed!\n", ErrorMessageColor);
							WriteMessage(e->ToString() + "\n", DebugMessageColor);
						}
					}
				}

				// DNS接続
				if(address == 0){
					try{
						if(MTINFO.DEBUG){
							WriteMessage("Resolving DNS...\n", DebugMessageColor);
						}
						address = Dns::GetHostEntry(host[0])->AddressList[0]->Address;
					}
					catch(Exception^ e){
						if(MTINFO.DEBUG){
							WriteMessage("DNS Error\n", ErrorMessageColor);
							WriteMessage(e->ToString() + "\n", DebugMessageColor);
						}
						address = 0;
					}
				}

				// 変換アドレス接続(Unicode)
				if(address == 0){
					try{
						if(MTINFO.DEBUG){
							WriteMessage("Converting Unicode address...\n", DebugMessageColor);
						}
						address = DecryptionIP(host[0], false);
					}
					catch(Exception^ e){
						if(MTINFO.DEBUG){
							WriteMessage("Unicode address conversion failed!\n", ErrorMessageColor);
							WriteMessage(e->ToString() + "\n", DebugMessageColor);
						}
					}
				}

				// ƒ[ƒJƒ‹Ú‘±—p
				/*
				if(address == 0){
					try{
						if((host[0]->StartsWith("192.168.", StringComparison::OrdinalIgnoreCase))) {
							address = IPAddress::Parse(host[0])->Address;
						}
					}
					catch(Exception^ e){
						if(MTINFO.DEBUG){
							WriteMessage("ƒ[ƒJƒ‹Ú‘±Ž¸”s\n", ErrorMessageColor);
							WriteMessage(e->ToString() + "\n", DebugMessageColor);
						}
						address = 0;
					}
				}
				*/
				

				if(address == 0){
					WriteMessage("ERROR: Could not find destination address.\n", ErrorMessageColor);
					throw gcnew SocketException;
				}

				IPEndPoint^ ep = gcnew IPEndPoint(address, port);
				if(MTINFO.DEBUG){
					WriteMessage(String::Format("Connected to Server\n", ep), DebugMessageColor);
				}

				PacketPacker^ pp = gcnew PacketPacker;
				array<BYTE>^ name = Encoding::Unicode->GetBytes(me->NAME);
				array<BYTE>^ cmnt = Encoding::Unicode->GetBytes(me->COMMENT);
				array<BYTE>^ look = Encoding::Unicode->GetBytes(me->LOOKING);
				array<BYTE>^ avoi = Encoding::Unicode->GetBytes(me->AVOIDING);

				pp->Pack(MTOPTION.CONNECTION_TYPE == CT_CLIENT ? (BYTE)PH_REQ_CONNECTION : (BYTE)PH_REQ_CONNECTION_H);
				pp->Pack(TYMT_VERSION);
				pp->Pack((BYTE)name->Length);
				pp->Pack(name);
				pp->Pack((BYTE)cmnt->Length);
				pp->Pack(cmnt);
				pp->Pack((BYTE)look->Length);
				pp->Pack(look);
				pp->Pack((BYTE)avoi->Length);
				pp->Pack(avoi);
				

				// ƒT[ƒo‚ÉÚ‘±—v‹
				UDP->Send(pp->Packet, pp->Length, ep);


				// •ÔM‘Ò‚¿
				UDP->Client->ReceiveTimeout = TIME_OUT;
				array<BYTE>^ rcv = UDP->Receive(ep);
				UDP->Client->ReceiveTimeout = 0;

				PacketDivider^ pd = gcnew PacketDivider(rcv);

				// Ú‘±OK
				if(pd->Divide() == PH_RES_CONNECTION){
					int len = pd->Divide();

					ServerName = Encoding::Unicode->GetString(pd->Divide(len));
					MemberList[0]->ID = BitConverter::ToUInt16(pd->Divide(2), 0);

					if(MemberList[0]->ID > MAX_ID){
						throw gcnew SocketException;
					}

					MemberInfo^ mi = gcnew MemberInfo;
					mi->IP_EP    = ep;
					mi->ID       = 0;
					mi->TYPE     = CT_SERVER;
					mi->NUM_VS   = 0;
					mi->RESPONSE = timeGetTime();

					// –¼‘O
					len = pd->Divide();
					mi->NAME = Encoding::Unicode->GetString(pd->Divide(len));

					// ƒRƒƒ“ƒg
					len = pd->Divide();
					mi->COMMENT = Encoding::Unicode->GetString(pd->Divide(len));

					try {
						len = pd->Divide();
						mi->LOOKING = Encoding::Unicode->GetString(pd->Divide(len));
					
						len = pd->Divide();
						mi->AVOIDING = Encoding::Unicode->GetString(pd->Divide(len));
					} catch (Exception^ ex) {
						MessageBox::Show("The server you attempted to join is incompatible with this version of LilithPort (v" + LP_VERSION + ")\n\nTo connect to this server please use LilithPort v109 (1.0.9.x) or earlier. 1.0.9.x is still supported until further notice.\n\nIf you are the server operator, you can optionally update your server to the latest version which supports LilithPort v110 (1.1.0.0) or later.", "Server Incompatible", MessageBoxButtons::OK, MessageBoxIcon::Error);
						return;
					}

					// ó‘Ô
					mi->STATE = pd->Divide();

					MemberList->Add(mi);
					AddListView(mi);

					// ƒo[ƒWƒ‡ƒ“Šm”F
					UINT ServerVersion;
					try{
						// 1.03ˆÈã‚È‚ç—ˆ‚é
						ServerVersion = pd->Divide();
						MTINFO.VERSION_CHECKED = true;
					}
					catch(Exception^){
						MTINFO.VERSION_CHECKED = false;
					}
					if(MTINFO.VERSION_CHECKED) {
						WriteMessage(String::Format("Server Version : v{0}\n", ServerVersion), SystemMessageColor);
						if(LP_VERSION > ServerVersion){
							WriteMessage("WARNING: Server version is out of date. Please update at:\n https://github.com/WindowsLogic-Productions/LilithPort/releases.\n", ErrorMessageColor);
						}
						else if(LP_VERSION < ServerVersion) {
							WriteMessage("WARNING: Client version is out of date. Please update at:\n https://github.com/WindowsLogic-Productions/LilithPort/releases.\n", ErrorMessageColor);
						}
					}
					else{
						WriteMessage("WARNING: MTSP Server Joined", ErrorMessageColor);
					}

					// ‘ ŽóMŠJŽn
					UDP->BeginReceive(gcnew AsyncCallback(ReceivePackets), this);


					if(MTOPTION.CONNECTION_TYPE == CT_HOST){
						this->Text += String::Format("  [{0}] [Host Port:{1}]", ServerName, MTOPTION.OPEN_PORT);
					}
					else{
						this->Text += String::Format("  [{0}] [Client]", ServerName);
					}
					WriteMessage(String::Format("Connected to {0} (ID = {1})\n", ServerName, me->ID), SystemMessageColor);

					if(ServerName->Length > 0){
						if(ServerName[0] == '+'){
							ServerMode = SM_MIX;
							WriteMessage("Server is in mixed mode.\n", SystemMessageColor);
						}
						if(ServerName[0] == '@'){
							ServerMode = SM_MATCH;
							WriteMessage("Chat is disabled on this server.\n", SystemMessageColor);
						}
						else if(ServerName[0] == '#'){
							ServerMode = SM_NORA;
							WriteMessage("Anonymous server: chat and names are invisible.\n", SystemMessageColor);

							ListView = LV_BLIND;
							listBoxMember->Items[0] = gcnew String("Anonymous Mode");
						}
					}

					// ƒƒ“ƒo[ƒŠƒXƒg‚ð—v‹
					pp->Clear();
					pp->Pack(PH_REQ_LIST);
					pp->Pack(0);
					pp->Pack(0);
					UDP->Send(pp->Packet, pp->Length, ep);

					// ƒ\ƒi[‹N“®
					Ranging = true;
					SonarThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::RunSonar));
					SonarThread->Start();
				}
				else{
					throw gcnew SocketException;
				}
			}
			catch(SocketException^ e){
				Leave(false);

				if (e->ErrorCode == 0) {
					if (me->ID == 0xFFFF) {
						WriteMessage(String::Format(L"ERROR: {0} is full!\n", ServerName), ErrorMessageColor);
					}
					else if (me->ID == 0xFFFE) {
						WriteMessage(L"ERROR: Program version is different.\n", ErrorMessageColor);
					}
					else if (me->ID > MAX_ID) {
						WriteMessage(String::Format(L"ERROR: {0} could not issue any more player IDs.\n", ServerName), ErrorMessageColor);
					}
					else if (address != 0) {
						WriteMessage(L"ERROR: Could not connect to the server.\nThe connection may be busy.\n", ErrorMessageColor);
					}
				}
				else {
					if (e->ErrorCode == WSAECONNRESET) {
						WriteMessage(L"ERROR: The connection was reset. \n", ErrorMessageColor);
					}
					else if (e->ErrorCode == WSAETIMEDOUT) {
						WriteMessage(L"ERROR: The connection timed out.\nThis could be due to many reasons: the server is not running, or the address may be incorrect.\n", ErrorMessageColor);
					}
					else if (e->ErrorCode != WSAHOST_NOT_FOUND) {
						WriteMessage(String::Format(L"Unknown socket error: {0}\n", e->ErrorCode), ErrorMessageColor);
					}
					if (MTINFO.DEBUG) {
						WriteMessage(e->ToString() + L"\n", DebugMessageColor);
					}
				}
			}
		}
	}
	else if(MTOPTION.CONNECTION_TYPE != CT_FREE){
		Leave(false);
	}
	else{
		this->Text += "  [Free Play]";
	}
}

void MainForm::PacketSendAllMember(array<BYTE>^% datagram, UINT received_id)
{
	Monitor::Enter(MemberList);
	try{
		for(int i = 1; i < MemberList->Count; i++){
			if(received_id != MemberList[i]->ID){
				UDP->BeginSend(datagram, datagram->Length, MemberList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
			}
		}
	}
	finally{
		Monitor::Exit(MemberList);
	}
}

void MainForm::SendPackets(IAsyncResult^ asyncResult)
{
	UdpClient^ udp = (UdpClient^)asyncResult->AsyncState;

	try{
		udp->EndSend(asyncResult);
	}
	catch(ObjectDisposedException^){
	}
}

void MainForm::ReceivePackets(IAsyncResult^ asyncResult)
{
	int i;
	UINT16 id;
	array<BYTE> ^send;

	MainForm^ form = safe_cast<MainForm^>(asyncResult->AsyncState);
	IPEndPoint^ ep = gcnew IPEndPoint(IPAddress::Any, 0);

	try{
		array<BYTE>^ rcv = UDP->EndReceive(asyncResult, ep);
		UDP->BeginReceive(gcnew AsyncCallback(ReceivePackets), form);

		if(rcv->Length > MAX_PACKET){
			if(MTINFO.DEBUG){
				form->WriteMessage(String::Format("Large Packet > {0} from {1}\n", rcv[0], ep->ToString()), DebugMessageColor);
			}
			return;
		}

		switch(rcv[0]){
		case PH_PING:
			rcv[0] = PH_PONG;
			UDP->Send(rcv, rcv->Length, ep);
			if(MTINFO.DEBUG){
				form->WriteMessage(String::Format("Ping—v‹ from {0}\n", ep), DebugMessageColor);
			}
			break;

		case PH_PONG:
			if(rcv->Length == 1 && Ping > 0){
				form->WriteMessage(String::Format("Ping : {0}ms\n", timeGetTime() - Ping), SystemMessageColor);
				Ping = 0;
			}
			break;

		case PH_REQ_CONNECTION:
		case PH_REQ_CONNECTION_H:
			// ŽI‚¶‚á‚È‚¢‚Ì‚ÉÚ‘±—v‹‚ª—ˆ‚½
			if(MTOPTION.CONNECTION_TYPE != CT_SERVER){
				break;
			}

			Monitor::Enter(MemberList);
			try{
				array<BYTE>^ a_id;
				array<BYTE>^ saba = Encoding::Unicode->GetBytes(ServerName);
				array<BYTE>^ name = Encoding::Unicode->GetBytes(MemberList[0]->NAME);
				array<BYTE>^ cmnt = Encoding::Unicode->GetBytes(MemberList[0]->COMMENT);
				array<BYTE>^ look = Encoding::Unicode->GetBytes(MemberList[0]->LOOKING);
				array<BYTE>^ avoi = Encoding::Unicode->GetBytes(MemberList[0]->AVOIDING);

				MemberInfo^ mi;
				PacketPacker^  pp = gcnew PacketPacker;
				PacketDivider^ pd = gcnew PacketDivider(rcv);

				i = pd->Divide() == PH_REQ_CONNECTION ? CT_CLIENT : CT_HOST;

				// ƒT[ƒo–¼
				pp->Pack(PH_RES_CONNECTION);
				pp->Pack((BYTE)saba->Length);
				pp->Pack(saba);

				// ID‚Ì”­s
				if(MemberList->Count > (int)MTOPTION.MAX_CONNECTION){
					// –žŽº
					a_id = gcnew array<BYTE>{ 0xFF, 0xFF };
				}
				else if(pd->Divide() != TYMT_VERSION){
					// ƒo[ƒWƒ‡ƒ“ˆá‚¢
					a_id = gcnew array<BYTE>{ 0xFE, 0xFF };
				}
				else{
					a_id = BitConverter::GetBytes(++IDCounter);

					if(IDCounter <= MAX_ID){
						if(IDCounter == MAX_ID){
							form->WriteMessage("ERROR: issued ID is above the max ID!\n", ErrorMessageColor);
						}

						// ƒƒ“ƒo[“o˜^
						mi = gcnew MemberInfo;
						mi->IP_EP    = ep;
						mi->ID       = IDCounter;
						mi->TYPE     = i;
						mi->STATE    = MS_FREE;
						mi->NUM_VS   = 0;
						mi->RESPONSE = timeGetTime();

						// –¼‘O
						i = pd->Divide();
						mi->NAME = Encoding::Unicode->GetString(pd->Divide(i));

						// ƒRƒƒ“ƒg
						i = pd->Divide();
						mi->COMMENT = Encoding::Unicode->GetString(pd->Divide(i));

						i = pd->Divide();
						mi->LOOKING = Encoding::Unicode->GetString(pd->Divide(i));

						i = pd->Divide();
						mi->AVOIDING = Encoding::Unicode->GetString(pd->Divide(i));

						if(ListView == LV_BLIND){
							mi->NAME = gcnew String("Ÿ");
							mi->COMMENT = String::Empty;
							mi->LOOKING = String::Empty;
							mi->AVOIDING = String::Empty;
						}

						MemberList->Add(mi);
						form->AddListView(mi);

						// ƒoƒbƒNƒAƒbƒv
						MemberInfoBackUp^ mibk = gcnew MemberInfoBackUp;
						mibk->IP_EP = ep;
						mibk->ID    = IDCounter;
						MemberListBackUp->Add(mibk);
						

						form->WriteComment(mi->NAME, mi->TYPE, mi->COMMENT);

						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Connect from {0}:{1}\n", ep->Address, ep->Port), DebugMessageColor);
						}
					}
					else if(IDCounter > MAX_ID){
						IDCounter = MAX_ID;
					}
				}

				// ID
				pp->Pack(a_id);

				// ŽIŽå‚ÌƒjƒbƒNƒl[ƒ€
				pp->Pack((BYTE)name->Length);
				pp->Pack(name);

				// ŽIŽå‚ÌƒRƒƒ“ƒg
				pp->Pack((BYTE)cmnt->Length);
				pp->Pack(cmnt);
				pp->Pack((BYTE)look->Length);
				pp->Pack(look);
				pp->Pack((BYTE)avoi->Length);
				pp->Pack(avoi);

				// ó‘Ô
				pp->Pack((BYTE)MemberList[0]->STATE);

				// ƒo[ƒWƒ‡ƒ“î•ñ‘—M
				pp->Pack((BYTE)LP_VERSION);

				// “o˜^Š®—¹
				UDP->Send(pp->Packet, pp->Length, ep);

				if(mi != nullptr){
					pp->Clear();

					// ‘Sˆõ‚É“üŽº‚ð’Ê’m
					name = Encoding::Unicode->GetBytes(mi->NAME);
					cmnt = Encoding::Unicode->GetBytes(mi->COMMENT);
					look = Encoding::Unicode->GetBytes(mi->LOOKING);
					avoi = Encoding::Unicode->GetBytes(mi->AVOIDING);
					array<BYTE>^ address = ep->Address->GetAddressBytes();
					array<BYTE>^ port = BitConverter::GetBytes((UINT16)ep->Port);

					a_id[1] |= mi->TYPE << 6;

					pp->Pack(PH_NEW_MEMBER);
					pp->Pack(a_id);
					pp->Pack((BYTE)name->Length);
					pp->Pack(name);
					pp->Pack((BYTE)cmnt->Length);
					pp->Pack(cmnt);
					pp->Pack((BYTE)look->Length);
					pp->Pack(look);
					pp->Pack((BYTE)avoi->Length);
					pp->Pack(avoi);
					pp->Pack(MS_FREE);
					pp->Pack(address);
					pp->Pack(port);

					// ŠÈˆÕˆÃ†
					Monitor::Enter(ServerName);
					try{
						CipherRand(ServerName->GetHashCode());

						for(i = 1; i < pp->Length; i++){
							pp->Packet[i] ^= CipherRand();
						}
					}
					finally{
						Monitor::Exit(ServerName);
					}

					for(i = 1; i < MemberList->Count - 1; i++){
						UDP->BeginSend(pp->Packet, pp->Length, MemberList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
					}
				}
			}
			finally{
				Monitor::Exit(MemberList);
			}

			// WelcomeƒƒbƒZ[ƒW
			ReplaceWelcomeTab(true);
			i = _tcslen(MTOPTION.WELCOME)*2;

			if(i > 0){
				Thread::Sleep(50);

				PacketPacker^ pp = gcnew PacketPacker;

				pp->Pack(PH_NOTICE);
				pp->Pack((BYTE)i);
				pp->Pack(Encoding::Unicode->GetBytes(gcnew String(MTOPTION.WELCOME)));
				UDP->Send(pp->Packet, pp->Length, ep);
			}
			break;

		case PH_MESSAGE:
			if(ServerMode >= SM_MATCH){
				break;
			}

			if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
				form->TalkMessage(BitConverter::ToUInt16(rcv, 1), rcv);
			}
			else{
				// ŽóMONLY
				form->TalkMessage(0xFFFF, rcv);
			}
			break;

		case PH_NOTICE:
			if(UDP != nullptr){
				form->WriteMessage("[MOTD]---------------------\n", SystemMessageColor);
				form->WriteNotice(Encoding::Unicode->GetString(rcv, 2, (rcv->Length)-2));
				form->WriteMessage("-------------------------------\n", SystemMessageColor);
			}
			break;

		case PH_REQ_LIST:
			if(MTOPTION.CONNECTION_TYPE != CT_SERVER){
				return;
			}
			else{
				id = BitConverter::ToUInt16(rcv, 1);
				array<BYTE> ^a_id, ^name, ^cmnt, ^look, ^avoi, ^address, ^port;
				PacketPacker^ pp = gcnew PacketPacker;

				Monitor::Enter(MemberList);
				try{
					for(i = 1; i < MemberList->Count; i++){
						// id == 0‚Å‘Sˆõ•ª
						if(id == 0 || id == MemberList[i]->ID){
							a_id = BitConverter::GetBytes((UINT16)(MemberList[i]->ID | MemberList[i]->TYPE << 14));
							name = Encoding::Unicode->GetBytes(MemberList[i]->NAME);
							cmnt = Encoding::Unicode->GetBytes(MemberList[i]->COMMENT);
							look = Encoding::Unicode->GetBytes(MemberList[i]->LOOKING);
							avoi = Encoding::Unicode->GetBytes(MemberList[i]->AVOIDING);
							address = MemberList[i]->IP_EP->Address->GetAddressBytes();
							port = BitConverter::GetBytes((UINT16)MemberList[i]->IP_EP->Port);

							pp->Clear();
							pp->Pack(PH_RES_LIST);
							pp->Pack(a_id);
							pp->Pack((BYTE)name->Length);
							pp->Pack(name);
							pp->Pack((BYTE)cmnt->Length);
							pp->Pack(cmnt);
							pp->Pack((BYTE)look->Length);
							pp->Pack(look);
							pp->Pack((BYTE)avoi->Length);
							pp->Pack(avoi);
							pp->Pack((BYTE)MemberList[i]->STATE);
							pp->Pack(address);
							pp->Pack(port);

							// ŠÈˆÕˆÃ†
							Monitor::Enter(ServerName);
							try{
								CipherRand(ServerName->GetHashCode());

								for(int j = 1; j < pp->Length; j++){
									pp->Packet[j] ^= CipherRand();
								}
							}
							finally{
								Monitor::Exit(ServerName);
							}

							UDP->BeginSend(pp->Packet, pp->Length, ep, gcnew AsyncCallback(SendPackets), UDP);

							if(id > 0){
								break;
							}
						}
					}
					if(MTINFO.DEBUG){
						if(id == 0){
							form->WriteMessage("Replied to player list request.\n", DebugMessageColor);
						}
					}

					if(id > 0 && i >= MemberList->Count){
						// サーバでもクラでも見えない状態
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Searching Unknown ({0})\n", id), ErrorMessageColor);
						}

						// ƒoƒbƒNƒAƒbƒvŒŸõ
						if(id <= MemberListBackUp->Count){
							try{
								// Unknown’Ê’m
								array<BYTE>^ send = gcnew array<BYTE>(3){ PH_LOST, 0xFF, 0xFF };
								UDP->BeginSend(send, send->Length, MemberListBackUp[id]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
								form->WriteMessage(String::Format("Failed to send notice to Unknown ({0})\n", id), SystemMessageColor);
							}
							catch(Exception^){
								if(MTINFO.DEBUG){
									form->WriteMessage(String::Format("Unknown({0})‚Ö‚Ì’Ê’m‚ÉŽ¸”s‚µ‚Ü‚µ‚½B\n", id), ErrorMessageColor);
								}
							}
						}else{
							// ƒoƒbƒNƒAƒbƒvƒŠƒXƒgˆÈã‚ÌID
							form->WriteMessage(String::Format("This ID is not present in the log. Shut down and restart the server after waiting a while.\n", id), ErrorMessageColor);
						}
					}
				}
				finally{
					Monitor::Exit(MemberList);
				}
			}
			break;

		case PH_RES_LIST:
		case PH_NEW_MEMBER:
			// ŠÈˆÕ•œ†
			Monitor::Enter(ServerName);
			try{
				CipherRand(ServerName->GetHashCode());

				for(i = 1; i < rcv->Length; i++){
					rcv[i] ^= CipherRand();
				}
			}
			finally{
				Monitor::Exit(ServerName);
			}

			id = BitConverter::ToUInt16(rcv, 1);

			if((id & 0x3FFF) != MemberList[0]->ID){
				int len;
				PacketDivider^ pd = gcnew PacketDivider(rcv);
				pd->Length = 3;

				Monitor::Enter(MemberList);
				try{
					MemberInfo^ mi = gcnew MemberInfo;
					mi->IP_EP  = gcnew IPEndPoint(0, 0);
					mi->ID     = id & 0x3FFF;
					mi->TYPE   = id >> 14;
					mi->NUM_VS = 0;

					// –¼‘O
					len = pd->Divide();
					mi->NAME = Encoding::Unicode->GetString(pd->Divide(len));

					// ƒRƒƒ“ƒg
					len = pd->Divide();
					mi->COMMENT = Encoding::Unicode->GetString(pd->Divide(len));

					len = pd->Divide();
					mi->LOOKING = Encoding::Unicode->GetString(pd->Divide(len));

					len = pd->Divide();
					mi->AVOIDING = Encoding::Unicode->GetString(pd->Divide(len));

					// ó‘Ô
					mi->STATE = pd->Divide();

					// ƒGƒ“ƒhƒ|ƒCƒ“ƒg
					mi->IP_EP->Address = gcnew IPAddress(pd->Divide(4));
					mi->IP_EP->Port    = BitConverter::ToUInt16(pd->Divide(2), 0);

					MemberList->Add(mi);
					form->AddListView(mi);

					// ˆ¥ŽA‘ã‚í‚è
					UDP->Send(gcnew array<BYTE>{PH_PING}, 1, mi->IP_EP);

					if(rcv[0] == PH_NEW_MEMBER){
						form->WriteComment(mi->NAME, mi->TYPE, mi->COMMENT);
					}
				}
				finally{
					Monitor::Exit(MemberList);
				}
			}
			break;

		case PH_QUIT:
		case PH_LOST:
			id = BitConverter::ToUInt16(rcv, 1);

			Monitor::Enter(MemberList);
			try{
				if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
					int member = 0;
					// ‘Sˆõ‚É’Ê’B
					for(i = 1; i < MemberList->Count; i++){
						if(id != MemberList[i]->ID){
							UDP->BeginSend(rcv, rcv->Length, MemberList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
						}
						else{
							member = i;
						}
					}

					if(member > 0){
						if(ListView != LV_BLIND){
							form->WriteTime(0, SystemMessageColor);
							form->WriteMessage(MemberList[member]->NAME + " has left.\n", SystemMessageColor);
						}

						// ŠÏí’†Ž~
						if(MemberList[0]->STATE == MS_WATCH || MemberList[0]->STATE == MS_COUCH){
							if(TargetID == MemberList[member]->ID){
								form->QuitWatch(false);
							}
						}
						MemberList->RemoveAt(member);
						form->listBoxMember->Items->RemoveAt(member);
					}
				}
				else{
					if(id == 0){
						form->WriteTime(0, SystemMessageColor);
						form->WriteMessage(ServerName + "  was shut down.\n", SystemMessageColor);
						MemberList[0]->STATE = 0xFF;
					}
					else if(id == 0xFFFF){
						form->WriteMessage(ServerName + " is not registered. Please try to connect at a later time.\n", ErrorMessageColor);
						MemberList[0]->STATE = 0xFF;
					}
					else{
						for(i = 0; i < MemberList->Count; i++){
							if(id == MemberList[i]->ID){
								if(i == 0){
									form->WriteTime(0, SystemMessageColor);
									form->WriteMessage("ERROR: Connection to the server lost.\n", ErrorMessageColor);
									MemberList[0]->STATE = 0xFF;
								}
								else{
									if(ListView != LV_BLIND){
										if(rcv[0] == PH_QUIT){
											form->WriteTime(0, SystemMessageColor);
											form->WriteMessage(MemberList[i]->NAME + " has left.\n", SystemMessageColor);
										}
										else{
											form->WriteTime(0, SystemMessageColor);
											form->WriteMessage(MemberList[i]->NAME + "'s connection was interrupted.\n", ErrorMessageColor);
											if(MTINFO.DEBUG){
												form->WriteMessage(String::Format("ID = {0}\n", MemberList[i]->ID), DebugMessageColor);
												form->WriteMessage(String::Format("IP_EP = Hidden\n", MemberList[i]->IP_EP), DebugMessageColor);
											}
										}
									}

									// ŠÏí’†Ž~
									if(MemberList[0]->STATE == MS_WATCH || MemberList[0]->STATE == MS_COUCH){
										if(TargetID == MemberList[i]->ID){
											form->QuitWatch(false);
										}
									}

									MemberList->RemoveAt(i);
									form->listBoxMember->Items->RemoveAt(i);
								}

								break;
							}
						}
					}
				}
			}
			finally{
				Monitor::Exit(MemberList);
			}

			if(MemberList[0]->STATE == 0xFF){
				form->Leave(false);
			}
			break;

		case PH_CHANGE_STATE:
			id = BitConverter::ToUInt16(rcv, 1);

			Monitor::Enter(MemberList);
			try{
				for(i = 0; i < MemberList->Count; i++){
					if(id == MemberList[i]->ID){

						if(MemberList[i]->STATE == MS_SEEK && rcv[3] != MS_SEEK){
							form->WriteTime(0, SystemMessageColor);
							form->WriteMessage(String::Format("{0} is now in seek mode\n", MemberList[i]->NAME), SystemMessageColor);
						}

						MemberList[i]->STATE = rcv[3];
						form->listBoxMember->Refresh();

						if(MemberList[i]->STATE == MS_SEEK){
							form->WriteTime(0, SystemMessageColor);
							form->WriteMessage(String::Format("{0} is now in seek mode.\n", MemberList[i]->NAME), SystemMessageColor);
							if(MTOPTION.SEEK_SOUND_ENABLE){
								try{
									Media::SoundPlayer^ wav = gcnew Media::SoundPlayer(gcnew String(MTOPTION.SEEK_SOUND));
									wav->Play();
								}
								catch(Exception^){
								}
							}
						}

						if(MTOPTION.CONNECTION_TYPE != CT_SERVER){
							break;
						}
					}
					else if(i != 0 && MTOPTION.CONNECTION_TYPE == CT_SERVER){
						UDP->BeginSend(rcv, rcv->Length, MemberList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
					}
				}
			}
			finally{
				Monitor::Exit(MemberList);
			}
			break;

		case PH_REQ_STATE:
			id = BitConverter::ToUInt16(rcv, 1);

			Monitor::Enter(MemberList);
			try{
				send = gcnew array<BYTE>(4){ PH_RES_STATE, rcv[1], rcv[2], 0xFF };
				BYTE state;

				if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
					for(i = 0; i < MemberList->Count; i++){
						if(id == MemberList[i]->ID){
							state = (BYTE)MemberList[i]->STATE;

							if(state == MS_READY){
								state = MS_FREE;
							}

							send[3] = state;
							break;
						}
					}
				}
				else if(id == MemberList[0]->ID){
					state = (BYTE)MemberList[0]->STATE;

					if(state == MS_READY){
						state = MS_FREE;
					}

					send[3] = state;
				}

				UDP->Send(send, send->Length, ep);
			}
			finally{
				Monitor::Exit(MemberList);
			}
			break;

		case PH_RES_STATE:
			id = BitConverter::ToUInt16(rcv, 1);

			Monitor::Enter(MemberList);
			try{
				for(i = 1; i < MemberList->Count; i++){
					if(id == MemberList[i]->ID){
						MemberList[i]->RESPONSE = timeGetTime();

						if(rcv[3] == 0xFF){
							// ‚»‚ñ‚Èl‚¢‚È‚©‚Á‚½
							form->WriteTime(0, SystemMessageColor);
							form->WriteMessage(MemberList[i]->NAME + " is new.\n", SystemMessageColor);

							if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
								// ‘Sˆõ‚É’Ê’B
								rcv[0] = PH_LOST;

								for(int j = 1; j < MemberList->Count; j++){
									if(id != MemberList[j]->ID){
										UDP->BeginSend(rcv, 3, MemberList[j]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
									}
								}
							}
							else{
								// ‚¢‚È‚©‚Á‚½’Ê’m ‰ž‹}ˆ’uVer
								// ‘ ‚©‚ç‚Ì‚ÝŒ©‚¦‚Ä‚¢‚éó‘Ô
								// ƒT[ƒo‚É‚Íî•ñ‚ª‚È‚¢‚Ì‚ÅŠÔÚ“I‚É’Ê’m‚µ‚Ä‚à‚ç‚¤
								rcv[0] = PH_LOST;
								rcv[1] = 0xFF;
								rcv[2] = 0xFF;
								UDP->Send(rcv, 3, MemberList[i]->IP_EP);
							}

							MemberList->RemoveAt(i);
							form->listBoxMember->Items->RemoveAt(i);
						}
						else if(MemberList[i]->STATE != rcv[3]){
							if(MTINFO.DEBUG){
								form->WriteMessage(String::Format(MemberList[i]->NAME + " > state:{0}\n", rcv[3]), DebugMessageColor);
							}
							form->WriteTime(0, SystemMessageColor);
							form->WriteMessage(MemberList[i]->NAME + " updated state.", SystemMessageColor);
							if(MTOPTION.CONNECTION_TYPE != CT_SERVER && MemberList[i]->ID == 0){
								form->WriteMessage("Connection to the server may have been lost. Please try to reconnect.\n", ErrorMessageColor);
							}
							MemberList[i]->STATE = rcv[3];
							form->listBoxMember->Refresh();
						}
						break;
					}
				}
			}
			finally{
				Monitor::Exit(MemberList);
			}
			break;

		case PH_CHANGE_COMMENT:
			if(ServerMode >= SM_MATCH){
				break;
			}

			id = BitConverter::ToUInt16(rcv, 1);

			Monitor::Enter(MemberList);
			try{
				for(i = 1; i < MemberList->Count; i++){
					if(id == MemberList[i]->ID){
						MemberList[i]->COMMENT = Encoding::Unicode->GetString(rcv, 4, rcv[3]);
						MemberList[i]->AVOIDING = Encoding::Unicode->GetString(rcv, 4, rcv[3]);
						MemberList[i]->LOOKING = Encoding::Unicode->GetString(rcv, 4, rcv[3]);
						break;
					}
				}

				if(i < MemberList->Count && ListView == LV_COMMENT){
					if(MemberList[i]->COMMENT->Length > 0){
						form->listBoxMember->Items[i] = MemberList[i]->COMMENT;
						form->listBoxMember->Items[i] = MemberList[i]->LOOKING;
						form->listBoxMember->Items[i] = MemberList[i]->AVOIDING;
					}
					else{
						form->listBoxMember->Items[i] = gcnew String("Ÿ");
					}
				}
			}
			finally{
				Monitor::Exit(MemberList);
			}

			if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
				form->PacketSendAllMember(rcv, id);
			}
			break;

		case PH_DICE:
			if(MTOPTION.CONNECTION_TYPE != CT_SERVER){
				Monitor::Enter(form->richTextBoxLog);
				try{
					form->richTextBoxLog->SelectionStart = form->richTextBoxLog->Text->Length;

					form->richTextBoxLog->SelectionColor = TalkMessageColor;
					form->richTextBoxLog->SelectionBackColor = NoticeBackColor;
					form->richTextBoxLog->AppendText(Byte(rcv[1]).ToString() + "\n");

					form->richTextBoxLog->SelectionStart = form->richTextBoxLog->Text->Length;
					if(!MTOPTION.LOG_LOCK) {
						form->richTextBoxLog->ScrollToCaret();
					}
				}
				catch(Exception ^e){
					WriteErrorLog(e->ToString(), "RichTextBox");
				}
				finally{
					Monitor::Exit(form->richTextBoxLog);
				}
			}
			break;

		case PH_REQ_VS:

			if(MemberList[0]->STATE == MS_SEEK && GameThread != nullptr){
				form->QuitGame();
				GameThread = nullptr;
				Thread::Sleep(500);
			}

			// €”õŽžŠÔ‚ª’·‚·‚¬‚½ê‡‚ÍV‹KÚ‘±Žó•t
			if(NetVS != nullptr && MemberList[0]->STATE == MS_READY){
				if((timeGetTime() - NetVS->START_UP) > TIME_OUT*2 + 1000){
					MemberList[0]->STATE = MS_FREE;
					delete NetVS;
					NetVS = nullptr;
				}
			}

			send = gcnew array<BYTE>(2);
			send[0] = PH_RES_VS;
			send[1] = (BYTE)MemberList[0]->STATE;

			// ŠiƒcƒN‚¶‚á‚È‚¢‚æ
			try{
				String^ exe = gcnew String(MTOPTION.GAME_EXE);
				FileVersionInfo^ info = FileVersionInfo::GetVersionInfo(exe);
				
				//TODO: FIX HACK
				if (info->LegalCopyright != "(C)2001 ENTERBRAIN,INC / OUTBACK" && info->FileDescription != "２Ｄ格闘ツクール2nd." && info->FileDescription != "２Ｄ格闘ツクール９５"){
					throw gcnew Exception;
				}
				/*
				else if(ServerMode == SM_MIX){
					// ¬ÝƒT[ƒoƒ‚[ƒh‚È‚Ì‚ÅAŽÀsƒtƒ@ƒCƒ‹‚Ìƒ`ƒFƒbƒN
					if((INT32)(Path::GetFileNameWithoutExtension(exe)->GetHashCode()) != BitConverter::ToInt32(rcv, 3)){
						send[1] = 0xFE;
					}
				}
				*/
				else{
					if((INT32)(Path::GetFileNameWithoutExtension(exe)->GetHashCode()) != BitConverter::ToInt32(rcv, 3)){
						send[1] = 0xFE;
					}
					//if(info->FileDescription == "‚Q‚cŠi“¬ƒcƒN[ƒ‹2nd."){
						MTINFO.KGT2K = true;
					//}
					//else{
						//MTINFO.KGT2K = false;
					//}
				}
			}
			catch(Exception^){
				send[1] = 0xFF;
				form->WriteMessage("Missing Executable\n", ErrorMessageColor);
				form->WriteMessage("Please use a valid Fighter Maker executable.\n", ErrorMessageColor);
			}

			UDP->BeginSend(send, send->Length, ep, gcnew AsyncCallback(SendPackets), UDP);

			if((send[1] == MS_FREE)||(send[1] == MS_SEEK)){
				MemberList[0]->STATE = MS_READY;

				id = BitConverter::ToUInt16(rcv, 1);

				Monitor::Enter(MemberList);
				try{
					for(i = 0; i < MemberList->Count; i++){
						if(id == MemberList[i]->ID) break;
					}

					if(i >= MemberList->Count){
						form->WriteMessage("ERROR: A match request was received from someone not on the list.\n", ErrorMessageColor);
						MemberList[0]->STATE = MS_FREE;

						if(MTOPTION.CONNECTION_TYPE != CT_SERVER){
							// ’m‚ç‚È‚¢l‚ª‚¢‚½‚Ì‚ÅƒT[ƒo‚É–â‚¢‡‚í‚¹
							array<BYTE>^ send = gcnew array<BYTE>(3){ PH_REQ_LIST };
							Array::Copy(BitConverter::GetBytes(id), 0, send, 1, 2);
							UDP->Send(send, 3, MemberList[1]->IP_EP);
						}
						break;
					}

					NetVS = gcnew VersusInfo;
					NetVS->SEQUENCE = VS_SETTING;
					NetVS->SLEEPING = false;
					NetVS->WAITING  = false;
					NetVS->START_UP = timeGetTime();
					NetVS->IP_EP    = ep;
					NetVS->PING     = gcnew array<UINT>(5){ 0, 0, 0, 0, 0 };
					NetVS->PONG     = gcnew array<UINT>(5){ 0, 0, 0, 0, 0 };
					NetVS->SEND     = gcnew array<BYTE>(32){ PH_VS_DATA };
					NetVS->L_FRAME  = 0;
					NetVS->L_READ   = 0;
					NetVS->R_FRAME  = 0;
					NetVS->R_READ   = 0;

					// ŠÏí—pID
					P1ID = MemberList[0]->ID;
					P2ID = id;

					// –¼‘O
					ZeroMemory(MTINFO.P1_NAME, sizeof(MTINFO.P1_NAME));
					ZeroMemory(MTINFO.P2_NAME, sizeof(MTINFO.P2_NAME));

					if(ListView != LV_BLIND){
						_tcscpy_s(MTINFO.P1_NAME, MTOPTION.NAME);

						IntPtr mp = Runtime::InteropServices::Marshal::StringToHGlobalAuto(MemberList[i]->NAME);
						_tcscpy_s(MTINFO.P2_NAME, static_cast<PTCHAR>(mp.ToPointer()));
						Runtime::InteropServices::Marshal::FreeHGlobal(mp);
					}

					// ‘Îí‰ñ”
					MemberList[i]->NUM_VS++;

					if(ListView != LV_BLIND){
						form->WriteTime(0, SystemMessageColor);
						form->WriteMessage(MemberList[i]->NAME, NameColor[MemberList[i]->TYPE]);
						form->WriteMessage("A new challenger!\n", SystemMessageColor);
					}
				}
				finally{
					Monitor::Exit(MemberList);
				}
			}
			break;

		case PH_RES_VS:
			if((rcv[1] != MS_FREE)&&(rcv[1] != MS_SEEK)){
				switch(rcv[1]){
				case MS_REST:
					form->WriteMessage(L"Challenger is currently on break.\n", SystemMessageColor);
					break;
				case MS_VS:
					form->WriteMessage(L"Challenger is currently playing.\n", SystemMessageColor);
					break;
				case MS_WATCH:
				case MS_COUCH:
					form->WriteMessage(L"Challenger is currently spectating.\n", SystemMessageColor);
					break;
				case MS_SEEK:
					break;
				case MS_READY:
					form->WriteMessage(L"Challenger is currently preparing for battle against another person.\n", SystemMessageColor);
					break;
				case 0xFE:
					form->WriteMessage(L"ERROR: You and your challenger have a different game or game version. Make sure your game executable's file name is the same as that of your opponent.\n", ErrorMessageColor);
					break;
				default:
					form->WriteMessage(L"Challenger is currently not in a state for matchmaking.\n", SystemMessageColor);
					break;
				}

				NetVS->SEQUENCE = VS_ERROR;

				if(VersusThread != nullptr && VersusThread->IsAlive && NetVS->SLEEPING){
					VersusThread->Interrupt();
				}
			}
			else if(NetVS != nullptr){
				NetVS->SEQUENCE = VS_PING;

				if(VersusThread != nullptr && VersusThread->IsAlive && NetVS->SLEEPING){
					VersusThread->Interrupt();
				}
			}
			break;

		case PH_REQ_VS_PING:
			rcv[0] = PH_RES_VS_PONG;
			UDP->Send(rcv, rcv->Length, ep);
			break;

		case PH_RES_VS_PONG:
			if(NetVS != nullptr){
				NetVS->PONG[rcv[1]] = timeGetTime();

				for(id = 0, i = 0; id < 5; id++){
					if(NetVS->PONG[id] > 0){
						i++;

						if(i == 4){
							if(NetVS->SEQUENCE == VS_PING){
								NetVS->SEQUENCE = VS_SETTING;
							
								if(VersusThread != nullptr && VersusThread->IsAlive && NetVS->SLEEPING){
									VersusThread->Interrupt();
								}
							}
							break;
						}
					}
				}
			}
			break;

		case PH_REQ_VS_SETTING:
			if(NetVS != nullptr && NetVS->SEQUENCE == VS_SETTING){
				send = gcnew array<BYTE>(11){ PH_RES_VS_SETTING };

				// ‘ÎíÝ’è
				MTINFO.SEED          = XorShift();
				MTINFO.MAX_STAGE     = MTOPTION.MAX_STAGE;
				MTINFO.STAGE_SELECT  = MTOPTION.STAGE_SELECT;
				MTINFO.ROUND         = MTOPTION.ROUND;
				MTINFO.TIMER         = MTOPTION.TIMER;
				MTINFO.TEAM_ROUND_HP = MTOPTION.TEAM_ROUND_HP;

				// ƒfƒBƒŒƒC
				if(MTOPTION.DELAY == 0 || rcv[1] > MTOPTION.DELAY){
					send[1] = rcv[1];
				}
				else{
					send[1] = (BYTE)MTOPTION.DELAY;
				}

				NetVS->SEQUENCE = VS_DATA;
				NetVS->DELAY    = send[1];
				NetVS->LOCAL    = gcnew array<UINT16>(NetVS->DELAY*4);
				NetVS->REMOTE   = gcnew array<UINT16>(NetVS->DELAY*4);

				for(i = 0; i < NetVS->LOCAL->Length; i++){
					NetVS->LOCAL[i]  = 0xFFFF;
					NetVS->REMOTE[i] = 0xFFFF;
				}

				for(UINT i = 0; i < NetVS->DELAY; i++){
					NetVS->LOCAL[i] = 0x0000;
				}

				Array::Copy(BitConverter::GetBytes(MTINFO.SEED), 0, send, 2, 4);
				send[6]  = (BYTE)MTINFO.MAX_STAGE;
				send[7]  = (BYTE)MTINFO.STAGE_SELECT;
				send[8]  = (BYTE)MTINFO.ROUND;
				send[9]  = (BYTE)MTINFO.TIMER;
				send[10] = (BYTE)MTINFO.TEAM_ROUND_HP;
				UDP->Send(send, send->Length, ep);

				MTINFO.CONTROL = 0;

				// ‘ÎíŠJŽn
				form->WriteTime(0, SystemMessageColor);
				form->WriteMessage(String::Format("Starting match... (Delay: {0})\n", NetVS->DELAY), SystemMessageColor);

				// ‰¹‚Å‚¨’m‚ç‚¹
				if(MTOPTION.VS_SOUND_ENABLE){
					try{
						Media::SoundPlayer^ wav = gcnew Media::SoundPlayer(gcnew String(MTOPTION.VS_SOUND));
						wav->Play();
					}
					catch(Exception^){
					}
				}

				// ‘Ò‚¿Žó‚¯—p‚¿‚å‚Á‚ÆƒXƒŠ[ƒv
				Thread::Sleep(1000);

				GameThread = gcnew Thread(gcnew ParameterizedThreadStart(form, &MainForm::RunGame));
				GameThread->Start((UINT)RT_VS);
			}
			break;

		case PH_RES_VS_SETTING:
			if(NetVS != nullptr){
				NetVS->DELAY  = rcv[1];
				NetVS->LOCAL  = gcnew array<UINT16>(NetVS->DELAY*4);
				NetVS->REMOTE = gcnew array<UINT16>(NetVS->DELAY*4);

				for(i = 0; i < NetVS->LOCAL->Length; i++){
					NetVS->LOCAL[i]  = 0xFFFF;
					NetVS->REMOTE[i] = 0xFFFF;
				}

				for(UINT i = 0; i < NetVS->DELAY; i++){
					NetVS->LOCAL[i] = 0x0000;
				}

				MTINFO.SEED         = BitConverter::ToUInt32(rcv, 2);
				MTINFO.MAX_STAGE    = rcv[6];
				MTINFO.STAGE_SELECT = rcv[7];
				MTINFO.ROUND        = rcv[8];
				MTINFO.TIMER        = rcv[9];
				// v1.04ˆÈãŒÝŠ·
				try{
					if(rcv[10]){
						MTINFO.TEAM_ROUND_HP = true;
					}else{
						MTINFO.TEAM_ROUND_HP = false;
					}
				}
				catch(Exception^){
					MTINFO.TEAM_ROUND_HP = false;
					form->WriteMessage("ERROR: Opponent is using an old version of LilithPort or MTSP!\n", ErrorMessageColor);
				}
				

				if(VersusThread != nullptr && VersusThread->IsAlive && NetVS->SEQUENCE == VS_SETTING && NetVS->SLEEPING){
					NetVS->SEQUENCE = VS_STANDBY;
					VersusThread->Interrupt();
				}
			}
			break;

		case PH_VS_END:
			if(NetVS != nullptr){
				form->WriteMessage("The match was ended.\n", SystemMessageColor);

				NetVS->SEQUENCE = VS_END;
				form->QuitGame();

				Monitor::Enter(NetVS->REMOTE);
				try{
					if(NetVS->WAITING == 2){
						NetVS->WAITING = 1;
						Monitor::Pulse(NetVS->REMOTE);
					}
				}
				finally{
					Monitor::Exit(NetVS->REMOTE);
				}

				if(MTINFO.DEBUG){
					form->WriteMessage(String::Format("Frame > {0}\n", NetVS->L_FRAME), DebugMessageColor);
				}
			}
			break;

		case PH_VS_DATA:
			if(NetVS != nullptr && NetVS->SEQUENCE == VS_DATA){
				UINT f = BitConverter::ToUInt32(rcv, 1);

				Monitor::Enter(NetVS->REMOTE);
				try{
					if(NetVS->R_FRAME > f + NetVS->DELAY){
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Invalid packet (delay): {0} / {1}\n", f, NetVS->R_FRAME), DebugMessageColor);
						}
						break;
					}
					else if(f > NetVS->R_FRAME + NetVS->DELAY){
						// まずこない
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Invalid packet (preceding): {0} / {1}\n", f, NetVS->R_FRAME), DebugMessageColor);
						}
						break;
					}

					for(UINT i = 0; i <= NetVS->DELAY; i++){
						if((f + i) < NetVS->R_FRAME){
							continue;
						}

						NetVS->REMOTE[(f + i) % NetVS->REMOTE->Length] = BitConverter::ToUInt16(rcv, 5 + i*2);

						if((f + i) == NetVS->R_FRAME && NetVS->WAITING == 2){
							NetVS->WAITING = 1;
							Monitor::Pulse(NetVS->REMOTE);
						}
					}
				}
				finally{
					Monitor::Exit(NetVS->REMOTE);
				}
			}
			break;

		case PH_REQ_VS_DATA:
			if(NetVS != nullptr && NetVS->SEQUENCE == VS_DATA){
				send = gcnew array<BYTE>(7);
				send[0] = PH_RES_VS_DATA;
				Array::Copy(rcv, 1, send, 1, 4);
				send[5] = 0xFF;
				send[6] = 0xFF;

				UINT f = BitConverter::ToUInt32(rcv, 1);

				Monitor::Enter(NetVS->LOCAL);
				try{
					if(f >= NetVS->L_FRAME + NetVS->DELAY){
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Invalid packet (prior request): {0} / {1}\n", f, NetVS->L_FRAME), DebugMessageColor);
						}
						UDP->BeginSend(send, send->Length, ep, gcnew AsyncCallback(SendPackets), UDP);
						break;
					}
					else if(NetVS->L_FRAME > f + NetVS->DELAY*2){
						// 既にデータがないのでこれがくるとゲームにならない
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Invalid packet (delayed request): {0} / {1}\n", f, NetVS->L_FRAME), DebugMessageColor);
						}
						break;
					}

					if(MTINFO.DEBUG){
						form->WriteMessage(String::Format("Request packet: {0} / {1}\n", f, NetVS->L_FRAME), DebugMessageColor);
					}

					Array::Copy(BitConverter::GetBytes(NetVS->LOCAL[f % NetVS->LOCAL->Length]), 0, send, 5, 2);
					UDP->BeginSend(send, send->Length, ep, gcnew AsyncCallback(SendPackets), UDP);
				}
				finally{
					Monitor::Exit(NetVS->LOCAL);
				}
			}
			break;

		case PH_RES_VS_DATA:
			if(NetVS != nullptr && NetVS->WAITING > 0){
				UINT32 f = BitConverter::ToUInt32(rcv, 1);

				Monitor::Enter(NetVS->REMOTE);
				try{
					if(f != NetVS->R_FRAME) break;

					UINT16 w = BitConverter::ToUInt16(rcv, 5);

					NetVS->REMOTE[NetVS->R_READ] = w;

					if(MTINFO.DEBUG){
						form->WriteMessage(String::Format("Reply packet: {0} : {1}\n", f, w), DebugMessageColor);
					}

					if(NetVS->WAITING == 2){
						NetVS->WAITING = 1;
						Monitor::Pulse(NetVS->REMOTE);
					}
				}
				finally{
					Monitor::Exit(NetVS->REMOTE);
				}
			}
			break;

		case PH_REQ_WATCH:
			rcv[0] = PH_RES_WATCH;
			id = BitConverter::ToUInt16(rcv, 1);

			if(MTOPTION.ALLOW_SPECTATOR == false){
				// ŠÏí•s‹–‰Â
				rcv[1] = 1;
				UDP->Send(rcv, 2, ep);
				break;
			}
			else if(SpectatorList->Count >= (int)MTOPTION.MAX_CONNECTION){
				// –žÈ
				rcv[1] = 2;
				UDP->Send(rcv, 2, ep);
				break;
			}
			else if(InputFrame > (UINT)InputHistory->Length - 200){
				// ’x
				rcv[1] = 3;
				UDP->Send(rcv, 2, ep);
				break;
			}
			else if(WatchTarget != nullptr && id == TargetID){
				// ‘ŠŒÝŠÏí
				rcv[1] = 4;
				UDP->Send(rcv, 2, ep);
				break;
			}
			else{
				SpectatorInfo^ si = gcnew SpectatorInfo;
				si->ID    = id;
				si->IP_EP = ep;
				si->FRAME = 0;

				if(SpectacleThread != nullptr && SpectacleThread->IsAlive && AllowWatch){
					// Šù‚É‚Í‚¶‚ß‚Ä‚é
					send = gcnew array<BYTE>(14);
					send[0] = PH_RES_WATCH;
					send[1] = 0;
					Array::Copy(BitConverter::GetBytes(P1ID), 0, send, 2, 2);
					Array::Copy(BitConverter::GetBytes(P2ID), 0, send, 4, 2);
					Array::Copy(BitConverter::GetBytes(MTINFO.SEED), 0, send, 6, 4);
					send[10] = MTINFO.MAX_STAGE;
					send[11] = MTINFO.STAGE_SELECT;
					send[12] = MTINFO.ROUND;
					send[13] = MTINFO.TIMER;

					UDP->Send(send, send->Length, ep);

					// ‚È‚©‚È‚©ƒfƒ“ƒWƒƒƒ‰ƒX‚ÈSleep
					Thread::Sleep(100);
				}

				Monitor::Enter(InputHistory);
				try{
					SpectatorList->Add(si);
				}
				finally{
					Monitor::Exit(InputHistory);
				}

				if(ListView != LV_BLIND){
					Monitor::Enter(MemberList);
					try{
						for(i = 0 ; i < MemberList->Count; i++){
							if(id == MemberList[i]->ID){
								break;
							}
						}

						if(i >= MemberList->Count){
							// 外来客
							form->WriteMessage("A spectator has joined.\n", SystemMessageColor);
						}
						else{
							form->WriteMessage(MemberList[i]->NAME + " has joined the match as a spectator.\n", SystemMessageColor);
						}
					}
					finally{
						Monitor::Exit(MemberList);
					}
				}
			}
			break;

		case PH_RES_WATCH:
			if(rcv[1] > 0){
				switch(rcv[1]){
				case 1:
					form->WriteMessage(L"Spectating is disabled for this match.\n", SystemMessageColor);
					break;

				case 2:
					form->WriteMessage(L"All spectator seats are taken!\n", SystemMessageColor);
					break;

				case 3:
					form->WriteMessage(L"You did not join in time to spectate.\n", SystemMessageColor);
					break;

				case 4:
					form->WriteMessage(L"This person is already watching you!\n", SystemMessageColor);
					break;
				}

				form->QuitWatch(false);
			}
			else{
				if(MemberList[0]->STATE == MS_WATCH || MemberList[0]->STATE == MS_COUCH){
					ZeroMemory(MTINFO.P1_NAME, sizeof(MTINFO.P1_NAME));
					ZeroMemory(MTINFO.P2_NAME, sizeof(MTINFO.P2_NAME));

					P1ID = BitConverter::ToUInt16(rcv, 2);
					P2ID = BitConverter::ToUInt16(rcv, 4);
					MTINFO.SEED = BitConverter::ToUInt32(rcv, 6);

					MTINFO.MAX_STAGE    = rcv[10];
					MTINFO.STAGE_SELECT = rcv[11];
					MTINFO.ROUND        = rcv[12];
					MTINFO.TIMER        = rcv[13];

					IntPtr mp;
					int c = 0;

					if(ListView != LV_BLIND){
						Monitor::Enter(MemberList);
						try{
							// –¼‘O
							for(i = 0; i < MemberList->Count; i++){
								if(MemberList[i]->ID == P1ID){
									mp = Runtime::InteropServices::Marshal::StringToHGlobalAuto(MemberList[i]->NAME);
									_tcscpy_s(MTINFO.P1_NAME, static_cast<PTCHAR>(mp.ToPointer()));
									Runtime::InteropServices::Marshal::FreeHGlobal(mp);

									c++;
								}
								else if(MemberList[i]->ID == P2ID){
									mp = Runtime::InteropServices::Marshal::StringToHGlobalAuto(MemberList[i]->NAME);
									_tcscpy_s(MTINFO.P2_NAME, static_cast<PTCHAR>(mp.ToPointer()));
									Runtime::InteropServices::Marshal::FreeHGlobal(mp);

									c++;
								}

								if(c == 2) break;
							}
						}
						finally{
							Monitor::Exit(MemberList);
						}
					}

					for(i = 0; i < WatchHistory->Length; i++){
						WatchHistory[i] = 0xFFFF;
					}
					WaitingWatch = 1;

					GameThread = gcnew Thread(gcnew ParameterizedThreadStart(form, &MainForm::RunGame));
					GameThread->Start((UINT)RT_WATCH);

					form->WriteMessage("Now spectating. Starting game.\n", SystemMessageColor);
				}
			}
			break;

		case PH_WATCH_DATA:
			if(WatchTarget != nullptr && WaitingWatch > 0){
				UINT32 f = BitConverter::ToUInt32(rcv, 1);

				if(f > WatchFrame + WatchHistory->Length - 20){
					form->WriteMessage("ERROR: Could not synchronize; leaving spectator mode.\n", ErrorMessageColor);
					form->QuitWatch(true);
				}

				Monitor::Enter(WatchHistory);
				try{
					if(WatchFrame > f + 20){
						if(MTINFO.DEBUG){
							form->WriteMessage(String::Format("Spectator packet (delay): {0} / {1}\n", f, WatchFrame), DebugMessageColor);
						}
						break;
					}

					for(UINT i = 0; i < 20; i++){
						if((f + i) < WatchFrame){
							continue;
						}

						WatchHistory[(f + i) % WatchHistory->Length] = BitConverter::ToUInt16(rcv, 5 + i*2);

						if((f + i) == WatchFrame && WaitingWatch == 2){
							WaitingWatch = 1;
							Monitor::Pulse(WatchHistory);
						}
					}
				}
				finally{
					Monitor::Exit(WatchHistory);
				}
			}
			break;

		case PH_WATCH_END:
			id = BitConverter::ToUInt16(rcv, 1);

			if(id == MemberList[0]->ID){
				// •Â–‹‚Ì‚¨’m‚ç‚¹
				form->WriteMessage("The match was ended. Leaving spectator mode.\n", SystemMessageColor);
				form->QuitWatch(false);
			}
			else{
				// ŠÏí’†Ž~‚Ì‚¨’m‚ç‚¹
				Monitor::Enter(InputHistory);
				try{
					if(MTINFO.DEBUG){
						form->WriteMessage(String::Format("Spectator end: {0}\n", id), DebugMessageColor);
					}

					for(i = 0; i < SpectatorList->Count; i++){
						if(id == SpectatorList[i]->ID){
							SpectatorList->RemoveAt(i);
							break;
						}
					}
				}
				finally{
					Monitor::Exit(InputHistory);
				}
			}
			break;

		case PH_SECRET:
			// ‰B‚µƒRƒ}ƒ“ƒh
			switch(rcv[1]){
			case ST_PING:
				rcv[1] = ST_PONG;
				UDP->Send(rcv, rcv->Length, ep);
				break;

			case ST_PONG:
				{
					id = BitConverter::ToUInt16(rcv, 2);
					UINT32 time = timeGetTime() - BitConverter::ToUInt32(rcv, 4);

					Monitor::Enter(MemberList);
					try{
						for(i = 0; i < MemberList->Count; i++){
							if(id == MemberList[i]->ID){
								Monitor::Enter(form->richTextBoxLog);
								try{
									form->richTextBoxLog->SelectionStart = form->richTextBoxLog->Text->Length;

									form->richTextBoxLog->SelectionColor = NameColor[MemberList[i]->TYPE];
									form->richTextBoxLog->AppendText(MemberList[i]->NAME);

									form->richTextBoxLog->SelectionColor = SecretColor;
									form->richTextBoxLog->AppendText(" at ");

									form->richTextBoxLog->SelectionColor = TalkMessageColor;
									form->richTextBoxLog->AppendText(UInt32(time).ToString());

									form->richTextBoxLog->SelectionColor = SecretColor;
									form->richTextBoxLog->AppendText("!!\n");

									form->richTextBoxLog->SelectionStart = form->richTextBoxLog->Text->Length;
									if(!MTOPTION.LOG_LOCK) {
										form->richTextBoxLog->ScrollToCaret();
									}
								}
								catch(Exception ^e){
									WriteErrorLog(e->ToString(), "RichTextBox");
								}
								finally{
									Monitor::Exit(form->richTextBoxLog);
								}
								break;
							}
						}
					}
					finally{
						Monitor::Exit(MemberList);
					}
				}
				break;

			default:
				break;
			}
			break;

		default:
			if(MTINFO.DEBUG){
				form->WriteMessage(String::Format("Unknown Packet > {0} from {1}\n", rcv[0], ep->ToString()), DebugMessageColor);
			}
			break;
		}
	}
	catch(ObjectDisposedException^){
		// UDPÚ‘±I—¹
		if(UDP != nullptr){
			UDP = nullptr;
			form->WriteMessage("You have left the server.\n", SystemMessageColor);
		}
	}
	catch(SocketException^ e){
		UDP->BeginReceive(gcnew AsyncCallback(ReceivePackets), form);

		if(e->ErrorCode == WSAECONNRESET){
			// ƒpƒPƒbƒg‚ª’e‚©‚ê‚½
			if(MTINFO.DEBUG){
				form->WriteMessage("ERROR > WSAECONNRESET\n", DebugMessageColor);
			}
		}
		else{
			form->WriteMessage(String::Format("SOCKET ERROR: ({0})\n", e->ErrorCode), ErrorMessageColor);
			if(MTINFO.DEBUG){
				form->WriteMessage(e->ToString() + "\n", DebugMessageColor);
			}
		}
	}
	catch(Exception^ e){
		form->WriteMessage(e->ToString() + "\n", ErrorMessageColor);
	}
}
// IPŽæ“¾ƒ^ƒCƒ€ƒAƒEƒg—pƒ^ƒCƒ}[
void MainForm::TimerGetIP()
{
	GetIPSleeping = true;
	try{
		// 15•b‚®‚ç‚¢‚Åƒ^ƒCƒ€ƒAƒEƒg
		Thread::Sleep(1000*15);
	}
	catch(ThreadInterruptedException^){
	}
	GetIPSleeping = false;

	try{
		if(GetIPClient != nullptr){
			GetIPClient->CancelAsync();
		}
	}
	catch(Exception^ e) {
		WriteMessage(e->ToString() + "\n", ErrorMessageColor);
	}
}

// HPŽ‚¿‰z‚µƒ^ƒCƒ}[
void MainForm::SetTeamHP(){
	if(MTINFO.DEBUG){
		WriteMessage("EVENT:SET_TEAM_HP\n", DebugMessageColor);
		WriteMessage(String::Format("PROCESS:{0}, WINNER:P{1}, HP:{2}\n", (UINT)MTINFO.PROCESS, ((UINT)MTINFO.WINNER)+1, MTINFO.P_HP), DebugMessageColor);
	}
	Thread::Sleep(1000);
	try {
		if(MTINFO.WINNER){
			WriteProcessMemory(MTINFO.PROCESS, (LPVOID)P2_HP, &MTINFO.P_HP, 4, NULL);
		}else{
			WriteProcessMemory(MTINFO.PROCESS, (LPVOID)P1_HP, &MTINFO.P_HP, 4, NULL);
		}
	}
	catch(Exception^ e){
		if(MTINFO.DEBUG){
			WriteMessage(String::Format("{0}\n", e->ToString()), DebugMessageColor);
		}
	}
}
void MainForm::RunSonar()
{
	int i, j;
	bool leave = false;
	array<BYTE>^ send = gcnew array<BYTE>(3){ PH_REQ_STATE, 0x00, 0x00 };

	SonarSleeping = false;

	// 20s‚Éˆê“xƒpƒPƒbƒg‚ð‘—‚èA100sˆÈã‰ž“š‚ª‚È‚©‚Á‚½‚çØ’f‚·‚é
	while(Ranging){
		SonarSleeping = true;
		try{
			Thread::Sleep(20*1000);
		}
		catch(ThreadInterruptedException^){
		}
		SonarSleeping = false;

		// ƒƒO‚ÌŽ©“®•Û‘¶
		if(MTOPTION.AUTO_SAVE){
			if(((timeGetTime() - MemberList[0]->RESPONSE) / 60000) >= MTOPTION.AUTO_SAVE){
				String^ path = gcnew String(MTOPTION.PATH);
				path += "auto.log";

				Monitor::Enter(richTextBoxLog);
				try{
					richTextBoxLog->SaveFile(path, RichTextBoxStreamType::PlainText);
				}
				catch(Exception ^e){
					WriteErrorLog(e->ToString(), "SaveLog");
				}
				finally{
					Monitor::Exit(richTextBoxLog);
				}

				MemberList[0]->RESPONSE = timeGetTime();
			}
		}

		if(!Ranging) return;

		Monitor::Enter(MemberList);
		try{
			if(MTOPTION.CONNECTION_TYPE == CT_SERVER){
				listBoxMember->BeginUpdate();

				for(i = 1; i < MemberList->Count; i++){
					Array::Copy(BitConverter::GetBytes(MemberList[i]->ID), 0, send, 1, 2);

					if((timeGetTime() - MemberList[i]->RESPONSE) > 100*1000){
						WriteTime(0, SystemMessageColor);
						WriteMessage(MemberList[i]->NAME + " was interrupted.\n", ErrorMessageColor);

						send[0] = PH_LOST;

						for(j = 1; j < MemberList->Count; j++){
							UDP->BeginSend(send, send->Length, MemberList[j]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
						}

						MemberList->RemoveAt(i);
						listBoxMember->Items->RemoveAt(i);

						send[0] = PH_REQ_STATE;
						i--;
					}
					else{
						// Šm”FƒpƒP
						UDP->BeginSend(send, send->Length, MemberList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
					}
				}

				listBoxMember->EndUpdate();
			}
			else{
				if((timeGetTime() - MemberList[1]->RESPONSE) > 100*1000){
					WriteMessage("Connection with the server timed out.\n", ErrorMessageColor);
					leave   = true;
					Ranging = false;
				}
				else{
					UDP->Send(send, send->Length, MemberList[1]->IP_EP);
				}
			}
		}
		catch(Exception^){
			WriteMessage("An exception was thrown in the sonar thread.\n", ErrorMessageColor);
		}
		finally{
			Monitor::Exit(MemberList);
		}
	}

	if(leave){
		Leave(false);
	}
}

void MainForm::RunGame(Object^ obj)
{
	UINT run_type = (UINT)obj;
	bool record_replay   = MTOPTION.RECORD_REPLAY;
	bool allow_spectator = MTOPTION.ALLOW_SPECTATOR;
	UINT sim_delay       = MTOPTION.SIMULATE_DELAY;

	// ‘—MŠÔŠu‚ðŒvŽZ
	if(run_type == RT_VS){
		if(NetVS == nullptr){
			// “¯Žž“Ê‘Îô
			WriteMessage("An error occurred. This may be caused by you and your opponent having sent a match request at the same time.\n", ErrorMessageColor);
			return;
		}

		NetVS->INTERVAL  = 0;
		NetVS->INTERVAL2 = 0;

		if(MTOPTION.INTERVAL > 0){
			if(MTOPTION.INTERVAL == 1){
				// Low
				NetVS->INTERVAL = (NetVS->DELAY + 1) / 4;

				if(NetVS->INTERVAL == 1){
					switch(NetVS->DELAY){
					case 3:
						NetVS->INTERVAL = 0;
						break;
					case 4:
						NetVS->INTERVAL2 = 5;
						break;
					case 5:
					case 6:
						NetVS->INTERVAL2 = 3;
						break;
					}
				}
			}
			else if(MTOPTION.INTERVAL == 2){
				// Middle
				NetVS->INTERVAL = (NetVS->DELAY + 1) / 3;

				if(NetVS->INTERVAL == 1){
					switch(NetVS->DELAY){
					case 2:
						NetVS->INTERVAL = 0;
						break;
					case 3:
					case 4:
						NetVS->INTERVAL2 = 4;
						break;
					}
				}
			}
			else if(MTOPTION.INTERVAL == 3){
				// High
				NetVS->INTERVAL = (NetVS->DELAY + 1) / 2;

				if(NetVS->INTERVAL == 1){
					// Delay:2 ‚Ì‚Ý
					NetVS->INTERVAL2 = 3;
				}
			}
		}
	}

	TCHAR wdir[_MAX_PATH], buf[_MAX_PATH], drive[_MAX_DRIVE];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DEBUG_EVENT de;
	CONTEXT c;
	HANDLE thread = NULL;
	DWORD thread_id = 0;
	DWORD state, e_code, e_address, mem;
	DWORD stage_loop = 0, blt_count = 0, input_count = 0, rand_count = 0, now_time, last_time = 0;
	int i, append_cap, last_cap = 0, num_vs = 0, p1_win = 0, p2_win = 0, timer = 0, p1_round = 0, p2_round = 0;
	bool vs_end = true, single_p = 0;

	MTINFO.INITIALIZED = false;
	MTINFO.SHOW_TOP = false;
	ZeroMemory(MTINFO.ORIGINAL_TITLE, sizeof(MTINFO.ORIGINAL_TITLE));
	ZeroMemory(MTINFO.TITLE, sizeof(MTINFO.TITLE));

	// ƒŠƒvƒŒƒCƒtƒ@ƒCƒ‹
	BinaryWriter^ bw = nullptr;
	BinaryReader^ br = nullptr;

	REPLAY_INFO ri;
	// deque‚ª‚ ‚é‚½‚ß‰Šú‰»‚ÍŽè“®‚Å
	ri.CONTROL = 0;
	ri.KEY[0] = ri.KEY[1] = 0xFFFF;
	ri.COUNT[0] = ri.COUNT[1] = 0;

	// ƒfƒBƒŒƒCƒVƒ~ƒ…ƒŒ[ƒg
	std::deque<UINT16> sim_que;
	std::deque<UINT16> sim_que_p2;

	if(run_type == RT_FREE && sim_delay > 0){
		for(UINT i = 0; i < sim_delay; i++){
			sim_que.push_back(0);
			sim_que_p2.push_back(0);
		}
	}

	try{
		String ^path, ^file;
		array<TCHAR>^ header = gcnew array<TCHAR>{'T', 'Y', 'M', 'T', 'R'};
		BYTE len;

		if(run_type == RT_PLAYBACK){
			br = gcnew BinaryReader(File::OpenRead(ReplayFilePath));

			// ƒwƒbƒ_
			br->ReadChars(header->Length);
			ri.VERSION = br->ReadChar();

			// P1ƒl[ƒ€
			len = br->ReadByte();
			for(i = 0; i < len; i++){
				MTINFO.P1_NAME[i] = br->ReadChar();
			}

			// P2ƒl[ƒ€
			len = br->ReadByte();
			for(i = 0; i < len; i++){
				MTINFO.P2_NAME[i] = br->ReadChar();
			}

			// —”ƒV[ƒh
			MTINFO.SEED = br->ReadUInt32();
			MTINFO.TEAM_ROUND_HP = MTOPTION.TEAM_ROUND_HP;

			if(ri.VERSION >= _T('3')){
				MTINFO.MAX_STAGE     = br->ReadByte();
				MTINFO.STAGE_SELECT  = br->ReadByte();
				MTINFO.ROUND         = br->ReadByte();
				MTINFO.TIMER         = br->ReadByte();
			}
			else{
				MTINFO.MAX_STAGE     = MTOPTION.MAX_STAGE;
				MTINFO.STAGE_SELECT  = MTOPTION.STAGE_SELECT;
				MTINFO.ROUND         = MTOPTION.ROUND;
				MTINFO.TIMER         = MTOPTION.TIMER;
			}
		}
		else if(record_replay){
			path = gcnew String(MTOPTION.REPLAY_FOLDER);

			// ƒQ[ƒ€•Ê‚ÉƒŠƒvƒŒƒC‚ðU‚è•ª‚¯
			if(MTOPTION.REPLAY_DIVIDE){
				path += "\\" + Path::GetFileNameWithoutExtension(gcnew String(MTOPTION.GAME_EXE));
			}

			if(!Directory::Exists(path)){
				Directory::CreateDirectory(path);
			}

			if(run_type == RT_VS){
				if(ListView == LV_BLIND){
					file = String::Format("vs_{0}.mtr", DateTime::Now.ToString("yyMMdd-HHmmss"));
				}
				else{
					file = String::Format("{0}_vs_{1}_{2}.mtr", gcnew String(MTINFO.P1_NAME), gcnew String(MTINFO.P2_NAME), DateTime::Now.ToString("yyMMdd-HHmmss"));
				}
			}
			else{
				file = String::Format("{0}_{1}.mtr", gcnew String(MTINFO.P1_NAME), DateTime::Now.ToString("yyMMdd-HHmmss"));
			}

			path += "\\" + file;
			bw = gcnew BinaryWriter(File::Create(path));

			WriteMessage(String::Format("Replay file will be created: \"{0}\"\n", file), SystemMessageColor);

			// ƒwƒbƒ_
			bw->Write(header);

			switch(MTOPTION.REPLAY_VERSION){
			case 1:
				ri.VERSION = _T('3');
				break;

			case 2:
			default:
				ri.VERSION = _T('4');
				break;
			}

			bw->Write(ri.VERSION);

			// P1ƒl[ƒ€
			len = (BYTE)_tcslen(MTINFO.P1_NAME);
			bw->Write(len);
			for(i = 0; i < len; i++){
				bw->Write(MTINFO.P1_NAME[i]);
			}

			// P2ƒl[ƒ€
			len = (BYTE)_tcslen(MTINFO.P2_NAME);
			bw->Write(len);
			for(i = 0; i < len; i++){
				bw->Write(MTINFO.P2_NAME[i]);
			}

			// —”ƒV[ƒh
			bw->Write(MTINFO.SEED);

			// ŠeŽíÝ’è
			bw->Write((BYTE)MTINFO.MAX_STAGE);
			bw->Write((BYTE)MTINFO.STAGE_SELECT);
			bw->Write((BYTE)MTINFO.ROUND);
			bw->Write((BYTE)MTINFO.TIMER);
		}
	}
	catch(IOException^ e){
		WriteMessage(String::Format("Error writing to replay file:\n{0}\n", e->ToString()), ErrorMessageColor);

		if(bw != nullptr){
			bw->Close();
			bw = nullptr;
		}
	}

	// –¼‘O•\Ž¦—p
	int is_p1 = _tcslen(MTINFO.P1_NAME);
	int is_p2 = _tcslen(MTINFO.P2_NAME);

	// ƒ‰ƒ“ƒ_ƒ€ƒXƒe[ƒW—p
	RandomStage(MTINFO.SEED);

	// ƒXƒe[ƒWƒ‹[ƒv—p
	if(MTINFO.STAGE_SELECT > MTINFO.MAX_STAGE){
		stage_loop = MTINFO.MAX_STAGE;
	}

	// ŠÏíƒpƒP‘—M€”õ
	if(allow_spectator){
		AllowWatch = false;
		InputFrame = 0;

		SpectacleThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::RunSpectacle));
		SpectacleThread->Start();
	}

	// ‘Îí’†’Ê’m
	if(run_type != RT_WATCH){
		if(MemberList[0]->STATE == MS_SEEK && run_type == RT_FREE){
			//WriteMessage("aiueo\n", ErrorMessageColor);
		}else{
			ChangeState((BYTE)MS_VS);
		}
	}

	try{
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		// ì‹ÆƒfƒBƒŒƒNƒgƒŠ
		_tsplitpath_s(MTOPTION.GAME_EXE, drive, _MAX_DRIVE, buf, _MAX_DIR, NULL, 0, NULL, 0);
		_stprintf_s(wdir, _T("%s%s"), drive, buf);
		
		if(CreateProcess(MTOPTION.GAME_EXE, NULL, NULL, NULL, false, DEBUG_PROCESS, NULL, wdir, &si, &pi)){
			if(run_type == RT_PLAYBACK){
				WriteMessage(String::Format("Opening \"{0}\" for playback...\n", Path::GetFileName(ReplayFilePath)), SystemMessageColor);
			}
			else{
				WriteMessage(String::Format("Starting \"{0}\"...\n", Path::GetFileNameWithoutExtension(gcnew String(MTOPTION.GAME_EXE))), SystemMessageColor);
			}
		}
		else{
			WriteMessage(String::Format("ERROR({0}) > Could not run\n", GetLastError(), gcnew String(MTOPTION.GAME_EXE)), ErrorMessageColor);
			return;
		}

		if (MTOPTION.SHOW_GAME_OPTION) {
			WriteMessage(String::Format(L"[Current settings]----------------------\n"
				L"Max stages: {0} / "
				L"Random stage: {1} / "
				L"# of rounds: {2} / "
				L"Round timer: {3}\n"
				L"Carry over HP between rounds: {4}\n"
				L"-------------------------------\n"
				, MTINFO.MAX_STAGE, MTINFO.STAGE_SELECT, MTINFO.ROUND, MTINFO.TIMER, MTINFO.TEAM_ROUND_HP == true ? "ON" : "OFF"), SystemMessageColor);
		}

		// ‹N“®‘Ò‚¿
		Thread::Sleep(300);

		MTINFO.PROCESS    = pi.hProcess;
		MTINFO.PROCESS_ID = pi.dwProcessId;

		while(WaitForDebugEvent(&de, INFINITE)){
			state = DBG_CONTINUE;

			switch(de.dwDebugEventCode){
			case EXCEPTION_DEBUG_EVENT:
				e_code    = de.u.Exception.ExceptionRecord.ExceptionCode;
				e_address = (DWORD)de.u.Exception.ExceptionRecord.ExceptionAddress;

				if (e_code == ERROR_NOACCESS) {
					if(MTINFO.DEBUG){
						WriteMessage(String::Format("ERROR_NOACCESS > {0} : {1}\n", e_code, e_address), DebugMessageColor);
					}
					state = DBG_EXCEPTION_NOT_HANDLED;
				}

				if(e_code == EXCEPTION_ACCESS_VIOLATION){
					ULONG_PTR info0 = de.u.Exception.ExceptionRecord.ExceptionInformation[0];
					ULONG_PTR info1 = de.u.Exception.ExceptionRecord.ExceptionInformation[1];

					if(MTINFO.DEBUG){
						WriteMessage(String::Format("EXCEPTION_ACCESS_VIOLATION > {0:X8}@{1:X8} : {2}@{3:X8}\n", e_code, e_address, info0 ? "read" : "write", info1), DebugMessageColor);
					}

					state = DBG_EXCEPTION_NOT_HANDLED;
				}

				if(e_code == EXCEPTION_BREAKPOINT){
					if(de.dwThreadId != thread_id){
						break;
					}

					switch(e_address){
					case VS_ROUND:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Eax++;
						SetThreadContext(thread, &c);

						if(MTINFO.DEBUG){
							WriteMessage("EVENT:VS_ROUND\n", DebugMessageColor);
						}

						if(vs_end){
							num_vs++;
							vs_end = false;
						}
						break;

					case VS_ROUND_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Edx = 0;
						SetThreadContext(thread, &c);

						if(vs_end){
							num_vs++;
							vs_end = false;
						}
						break;

					case ROUND_END:
						ReadProcessMemory(pi.hProcess, (LPVOID)ROUND_ESI, &mem, 4, NULL);

						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Esi = mem;

						if(vs_end == false){

							// HPŽ‚¿‰z‚µ  Edx:1P_ROUND  Esi:2P_ROUND  Eax:ROUND
							if(MTINFO.TEAM_ROUND_HP){
								if(c.Edx > (UINT)p1_round && c.Esi > (UINT)p2_round){
									// ‘Š‘Å‚¿
								}else{
									if(c.Edx > (UINT)p1_round){
										p1_round = c.Edx;
										ReadProcessMemory(pi.hProcess, (LPVOID)P1_HP, &MTINFO.P_HP, 4, NULL);
										MTINFO.WINNER = 0;
										SetTeamHPThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::SetTeamHP));
										SetTeamHPThread->Start();
									}
									if(c.Esi > (UINT)p2_round){
										p2_round = c.Esi;
										ReadProcessMemory(pi.hProcess, (LPVOID)P2_HP, &MTINFO.P_HP, 4, NULL);
										MTINFO.WINNER = 1;
										SetTeamHPThread = gcnew Thread(gcnew ThreadStart(this, &MainForm::SetTeamHP));
										SetTeamHPThread->Start();
									}
								}
							}
							
							// ‘ÎíI—¹Žž‚Í‚Q‰ñ”ò‚ñ‚Å‚­‚é
							if(c.Eax == c.Edx){
								p1_win++;
								vs_end = true;
							}
							if(c.Eax == c.Esi){
								p2_win++;
								vs_end = true;
							}

							if(vs_end){
								timer = 0;
								p1_round = 0;
								p2_round = 0;
							}
						}

						SetThreadContext(thread, &c);
						break;

					case ROUND_END_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Ebx = 0;
						SetThreadContext(thread, &c);

						if(vs_end == false){
							// ‘ÎíI—¹Žž‚Í‚R‰ñ”ò‚ñ‚Å‚­‚é
							ReadProcessMemory(pi.hProcess, (LPVOID)P1_WIN_95, &mem, 4, NULL);
							if(mem == 2){
								p1_win++;
								vs_end = true;
							}
							ReadProcessMemory(pi.hProcess, (LPVOID)P2_WIN_95, &mem, 4, NULL);
							if(mem == 2){
								p2_win++;
								vs_end = true;
							}
						}
						break;

					case RAND_FUNC:
						rand_count++;
						ReadProcessMemory(pi.hProcess, (LPVOID)RANDOM_SEED, &mem, 4, NULL);

						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Eax = mem;
						SetThreadContext(thread, &c);
						break;

					case RAND_FUNC_95:
						rand_count++;
						ReadProcessMemory(pi.hProcess, (LPVOID)RANDOM_SEED_95, &mem, 4, NULL);

						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Eax = mem;
						SetThreadContext(thread, &c);
						break;

					case STAGE_SELECT:
					case STAGE_SELECT_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);

						if(MTINFO.DEBUG){
							WriteMessage("EVENT:STAGE_SELECT\n", DebugMessageColor);
						}

						if(MTINFO.RAND_SEARCH){
							stage_loop = 0;

							MTINFO.MAX_STAGE    = MTOPTION.MAX_STAGE;
							MTINFO.STAGE_SELECT = MTOPTION.STAGE_SELECT;
						}

						if(stage_loop > 0){
							i = stage_loop - 1;

							stage_loop++;

							if(stage_loop > MTINFO.STAGE_SELECT){
								stage_loop = MTINFO.MAX_STAGE;
							}
						}
						else{
							if(MTINFO.STAGE_SELECT == 0){
								i = RandomStage() % MTINFO.MAX_STAGE;
							}
							else{
								i = MTINFO.STAGE_SELECT - 1;
							}
						}

						if(e_address == STAGE_SELECT){
							c.Eax = i;
						}
						else{
							c.Ecx = i;
						}

						SetThreadContext(thread, &c);
						break;

					case SINGLE_CONTROL_HOOK:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Ecx = 0;
						SetThreadContext(thread, &c);
						break;

					case STORY_KEY:
					case VS_P2_KEY:
						input_count++;

					case VS_P1_KEY:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);

						if(run_type == RT_VS){
							if(e_address == STORY_KEY){
								c.Eax = LocalInput((UINT16)c.Eax);
								c.Eax |= RemoteInput();
							}
							else if((e_address == VS_P1_KEY && MTINFO.CONTROL == 0) || (e_address == VS_P2_KEY && MTINFO.CONTROL == 1)){
								c.Eax = LocalInput((UINT16)c.Eax);
							}
							else{
								c.Eax = RemoteInput();
							}
						}
						else if(run_type == RT_WATCH && WaitingWatch > 0){
							// ŠÏíƒoƒbƒtƒ@‚©‚ç“Ç‚Ýž‚Ý
							c.Eax = WatchHistory[WatchFrame % WatchHistory->Length];

							if(c.Eax == 0xFFFF){
								Monitor::Enter(WatchHistory);
								try{
									WaitingWatch = 2;
									Monitor::Wait(WatchHistory, TIME_OUT*2);
								}
								finally{
									Monitor::Exit(WatchHistory);
								}

								c.Eax = WatchHistory[WatchFrame % WatchHistory->Length];

								if(c.Eax == 0xFFFF && WaitingWatch == 2){
									WriteMessage("Stopped spectating due to timeout.\n", ErrorMessageColor);
									QuitWatch(true);
								}
							}

							Monitor::Enter(WatchHistory);
							try{
								WatchHistory[WatchFrame % WatchHistory->Length] = 0xFFFF;
								WatchFrame++;
							}
							finally{
								Monitor::Exit(WatchHistory);
							}
						}
						else if(run_type == RT_PLAYBACK){
							c.Eax = ReadReplayData(br, ri);
						}
						else if(run_type == RT_FREE && sim_delay > 0){
							// ƒfƒBƒŒƒCƒVƒ~ƒ…ƒŒ[ƒg
							if(e_address != VS_P2_KEY){
								sim_que.push_back((UINT16)c.Eax);
								c.Eax = sim_que.front();
								sim_que.pop_front();
							}else{
								sim_que_p2.push_back((UINT16)c.Eax);
								c.Eax = sim_que_p2.front();
								sim_que_p2.pop_front();
							}
						}

						if(record_replay || allow_spectator){
							RecordInput((UINT16)c.Eax, bw, ri, allow_spectator);
						}

						// ƒƒ‚ƒŠ‚É‘‚«ž‚Ý
						if(e_address == VS_P2_KEY){
							WriteProcessMemory(pi.hProcess, (LPVOID)P2_INPUT, &c.Eax, 4, NULL);
						}
						else{
							WriteProcessMemory(pi.hProcess, (LPVOID)P1_INPUT, &c.Eax, 4, NULL);
						}

						SetThreadContext(thread, &c);
						break;

					case UNCHECK_JOYSTICK_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Eax = 0;
						SetThreadContext(thread, &c);
						break;

					case STORY_P2_KEY_95:
					case VS_P2_KEY_95:
						input_count++;

					case STORY_P1_KEY_95:
					case VS_P1_KEY_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);

						if(e_address == STORY_P1_KEY_95){
							ReadProcessMemory(pi.hProcess, (LPVOID)INPUT_COUNTER_95, &mem, 4, NULL);
							c.Ecx = mem;
						}
						else if(e_address == STORY_P2_KEY_95){
							ReadProcessMemory(pi.hProcess, (LPVOID)INPUT_COUNTER_95, &mem, 4, NULL);
							c.Edx = mem;
						}

						// ƒL[“ü—Í‚ ‚ê‚±‚ê 95Ver
						if(run_type == RT_VS){
							if((e_address == STORY_P1_KEY_95 && MTINFO.CONTROL == 0) || (e_address == STORY_P2_KEY_95 && MTINFO.CONTROL == 1) ||
								(e_address == VS_P1_KEY_95 && MTINFO.CONTROL == 0) || (e_address == VS_P2_KEY_95 && MTINFO.CONTROL == 1)){
								c.Eax = LocalInput((UINT16)c.Eax);
							}
							else{
								c.Eax = RemoteInput();
							}
						}
						else if(run_type == RT_WATCH && WaitingWatch > 0){
							// ŠÏíƒoƒbƒtƒ@‚©‚ç“Ç‚Ýž‚Ý
							c.Eax = WatchHistory[WatchFrame % WatchHistory->Length];

							if(c.Eax == 0xFFFF){
								Monitor::Enter(WatchHistory);
								try{
									WaitingWatch = 2;
									Monitor::Wait(WatchHistory, TIME_OUT*2);
								}
								finally{
									Monitor::Exit(WatchHistory);
								}

								c.Eax = WatchHistory[WatchFrame % WatchHistory->Length];

								if(c.Eax == 0xFFFF && WaitingWatch == 2){
									WriteMessage("Stopped spectating due to timeout.\n", ErrorMessageColor);
									QuitWatch(true);
								}
							}

							Monitor::Enter(WatchHistory);
							try{
								WatchHistory[WatchFrame % WatchHistory->Length] = 0xFFFF;
								WatchFrame++;
							}
							finally{
								Monitor::Exit(WatchHistory);
							}
						}
						else if(run_type == RT_PLAYBACK){
							c.Eax = ReadReplayData(br, ri);
						}
						else if(run_type == RT_FREE && sim_delay > 0){
							// 2P“ü—Í‚ÌƒVƒ~ƒ…ƒŒ[ƒg‚Í‚µ‚È‚¢
							
							if(e_address != STORY_P2_KEY_95 && e_address != VS_P2_KEY_95){
								sim_que.push_back((UINT16)c.Eax);
								c.Eax = sim_que.front();
								sim_que.pop_front();
							}
							
						}

						if(record_replay || allow_spectator){
							RecordInput((UINT16)c.Eax, bw, ri, allow_spectator);
						}

						if(e_address == VS_P1_KEY_95){
							WriteProcessMemory(pi.hProcess, (LPVOID)P1_INPUT_95, &c.Eax, 4, NULL);
						}
						else if(e_address == VS_P2_KEY_95){
							WriteProcessMemory(pi.hProcess, (LPVOID)P2_INPUT_95, &c.Eax, 4, NULL);
						}

						SetThreadContext(thread, &c);
						break;

					case FRAME_RATE:
					case FRAME_RATE_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Esi = c.Eax;
						SetThreadContext(thread, &c);

						blt_count++;
						now_time = timeGetTime();

						if(now_time - last_time >= 1000){
							last_time = now_time;

							append_cap = 0;
							_tcscpy_s(MTINFO.TITLE, MTINFO.ORIGINAL_TITLE);

							if(MTOPTION.DISPLAY_NAME){
								if(is_p1 == 0 && is_p2 == 0){
									append_cap = _stprintf_s(MTINFO.TITLE, _T("%s [Free Play]"), MTINFO.TITLE);
								}
								else if(is_p2 == 0){
									append_cap = _stprintf_s(MTINFO.TITLE, _T("%s [%s]"), MTINFO.TITLE, MTINFO.P1_NAME);
								}
								else{
									append_cap = _stprintf_s(MTINFO.TITLE, _T("%s [%s vs %s]"), MTINFO.TITLE, MTINFO.P1_NAME, MTINFO.P2_NAME);
								}
							}
							if(MTOPTION.DISPLAY_VERSUS){
								if(MTOPTION.SHOW_RESULT){
									append_cap = _stprintf_s(MTINFO.TITLE, _T("%s  Match: %d (%d - %d)"), MTINFO.TITLE, num_vs, p1_win, p2_win);
								}else{
									append_cap = _stprintf_s(MTINFO.TITLE, _T("%s  Match: %d"), MTINFO.TITLE, num_vs);
								}
							}
							if(MTOPTION.DISPLAY_FRAMERATE){
								append_cap = _stprintf_s(MTINFO.TITLE, _T("%s  FPS: %3d"), MTINFO.TITLE, blt_count, input_count);
							}
							if(MTOPTION.DISPLAY_RAND){
								append_cap = _stprintf_s(MTINFO.TITLE, _T("%s  Rand: %d"), MTINFO.TITLE, rand_count);
							}

							blt_count   = 0;
							input_count = 0;
							rand_count  = 0;

							if(append_cap > 0 || last_cap > 0 || MTINFO.HWND == NULL){
								last_cap = append_cap;

								Thread^ cap = gcnew Thread(gcnew ThreadStart(&SetCaption));
								cap->Start();
							}
						}
						break;

					case BGM_VOLUME:
					case BGM_VOLUME_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Edx = MTINFO.VOLUME[MTOPTION.BGM_VOLUME/5];
						SetThreadContext(thread, &c);
						break;

					case SE_VOLUME:
					case SE_VOLUME_95:
						c.ContextFlags = CONTEXT_INTEGER;
						GetThreadContext(thread, &c);
						c.Edx = MTINFO.VOLUME[MTOPTION.SE_VOLUME/5];
						SetThreadContext(thread, &c);
						break;
					}

					// ƒfƒoƒbƒO:ƒCƒxƒ“ƒg•\Ž¦
					if(MTINFO.DEBUG){
						/*
						if (e_address != VS_P1_KEY && e_address != RAND_FUNC && e_address != VS_P2_KEY
							 && e_address != FRAME_RATE && e_address != SE_VOLUME && e_address != ROUND_END
							 && e_address != STAGE_SELECT && e_address != VS_ROUND) {
							int nValue = e_address;
							char pszText[32];
							_itoa_s(nValue, pszText, 32 * sizeof(char), 16);
							WriteMessage(String::Format("EVENT:{0}\n", gcnew String(pszText)), DebugMessageColor);
						}
						*/
						// ‰ðÍ—p
						if (e_address != VS_P1_KEY && e_address != RAND_FUNC && e_address != VS_P2_KEY
							 && e_address != FRAME_RATE && e_address != SE_VOLUME && e_address != VS_ROUND) {
							int nValue = e_address;
							char pszText[32];
							_itoa_s(nValue, pszText, 32 * sizeof(char), 16);
							WriteMessage(String::Format("EVENT:{0}\n", gcnew String(pszText)), DebugMessageColor);
						}
						
					}
				}
				break;

			case CREATE_THREAD_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("CREATE_THREAD_DEBUG_EVENT\n", DebugMessageColor);
				}
				break;

			case CREATE_PROCESS_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("CREATE_PROCESS_DEBUG_EVENT\n", DebugMessageColor);
					WriteMessage(String::Format("{0}\n", MTINFO.SEED), DebugMessageColor);
				}

				thread    = de.u.CreateProcessInfo.hThread;
				thread_id = de.dwThreadId;

				// ‚±‚±‚©‚ç‘‚«Š·‚¦ƒ][ƒ“
				if(MTINFO.KGT2K){
					// 2nd.—p
					mem = MTOPTION.HIT_JUDGE;
					WriteProcessMemory(pi.hProcess, (LPVOID)HIT_JUDGE, &mem, 4, NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)HIT_JUDGE_SET, HIT_JUDGE_SET_CODE, sizeof(HIT_JUDGE_SET_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)RANDOM_SEED, &MTINFO.SEED, 4, NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)STAGE_SELECT, STAGE_SELECT_CODE, sizeof(STAGE_SELECT_CODE), NULL);

					WriteProcessMemory(pi.hProcess, (LPVOID)ROUND_SET, ROUND_SET_CODE, sizeof(ROUND_SET_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)TIMER_SET, TIMER_SET_CODE, sizeof(TIMER_SET_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)DEFAULT_ROUND, &MTINFO.ROUND, 4, NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)DEFAULT_TIMER, &MTINFO.TIMER, 4, NULL);

					// ƒ`[ƒ€ƒvƒŒƒCƒ‰ƒEƒ“ƒh”
					WriteProcessMemory(pi.hProcess, (LPVOID)TEAM_ROUND_SET, TEAM_ROUND_SET_CODE, sizeof(TEAM_ROUND_SET_CODE), NULL);
					if(MTINFO.ROUND > 4) {
						WriteProcessMemory(pi.hProcess, (LPVOID)TEAM_ROUND, &MAX_TEAM_ROUND, 4, NULL);
					}else{
						WriteProcessMemory(pi.hProcess, (LPVOID)TEAM_ROUND, &MTINFO.ROUND, 4, NULL);
					}


					// ƒ^ƒCƒgƒ‹ƒo[•\Ž¦—p
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_ROUND, VS_ROUND_CODE, sizeof(VS_ROUND_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)ROUND_END, ROUND_END_CODE, sizeof(ROUND_END_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)FRAME_RATE, FRAME_RATE_CODE, sizeof(FRAME_RATE_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)RAND_FUNC, RAND_FUNC_CODE, sizeof(RAND_FUNC_CODE), NULL);

					// ‰¹—Ê
					WriteProcessMemory(pi.hProcess, (LPVOID)VOLUME_SET_1, VOLUME_SET_1_CODE, sizeof(VOLUME_SET_1_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VOLUME_SET_2, VOLUME_SET_2_CODE, sizeof(VOLUME_SET_2_CODE), NULL);

					// ”M‘Ñ’†‚Íí‚É1P“ü—Í
					if(run_type == RT_VS){
						WriteProcessMemory(pi.hProcess, (LPVOID)SINGLE_CONTROL_HOOK, SINGLE_CONTROL_HOOK_CODE, sizeof(SINGLE_CONTROL_HOOK_CODE), NULL);
						WriteProcessMemory(pi.hProcess, (LPVOID)VS_CONTROL_HOOK, VS_CONTROL_HOOK_CODE, sizeof(VS_CONTROL_HOOK_CODE), NULL);
					}

					// ƒL[î•ñ‚ðŽæ“¾
					WriteProcessMemory(pi.hProcess, (LPVOID)STORY_KEY, STORY_KEY_CODE, sizeof(STORY_KEY_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_P1_KEY, VS_P1_KEY_CODE, sizeof(VS_P1_KEY_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_P2_KEY, VS_P2_KEY_CODE, sizeof(VS_P2_KEY_CODE), NULL);

					// ƒEƒBƒ“ƒhƒEƒTƒCƒY‚ð640x480‚É
					if(MTOPTION.CHANGE_WINDOW_SIZE){
						TCHAR val[32];

						_stprintf_s(buf, _T("%sgame.ini"), wdir);

						if(File::Exists(gcnew String(buf))){
							_itot_s(640, val, 10);
							WritePrivateProfileString(_T("GamePlay"), _T("GameWindowSize_x"), val, buf);
							_itot_s(480, val, 10);
							WritePrivateProfileString(_T("GamePlay"), _T("GameWindowSize_y"), val, buf);
						}
					}
				}
				else{
					// 95—p
					mem = MTOPTION.HIT_JUDGE;
					WriteProcessMemory(pi.hProcess, (LPVOID)HIT_JUDGE_95, &mem, 4, NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)HIT_JUDGE_SET_95, HIT_JUDGE_SET_95_CODE, sizeof(HIT_JUDGE_SET_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)RANDOM_SEED_95, &MTINFO.SEED, 4, NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)STAGE_SELECT_95, STAGE_SELECT_95_CODE, sizeof(STAGE_SELECT_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)TIMER_SET_95, TIMER_SET_95_CODE, sizeof(TIMER_SET_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)DEFAULT_TIMER_95, &MTINFO.TIMER, 4, NULL);

					// ƒ^ƒCƒgƒ‹ƒo[•\Ž¦—p
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_ROUND_95, VS_ROUND_95_CODE, sizeof(VS_ROUND_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)ROUND_END_95, ROUND_END_95_CODE, sizeof(ROUND_END_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)FRAME_RATE_95, FRAME_RATE_95_CODE, sizeof(FRAME_RATE_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)RAND_FUNC_95, RAND_FUNC_95_CODE, sizeof(RAND_FUNC_95_CODE), NULL);

					// ‰¹—Ê
					WriteProcessMemory(pi.hProcess, (LPVOID)VOLUME_SET_1_95, VOLUME_SET_1_95_CODE, sizeof(VOLUME_SET_1_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VOLUME_SET_2_95, VOLUME_SET_2_95_CODE, sizeof(VOLUME_SET_2_95_CODE), NULL);

					// JoyStick‚ÍƒGƒ‰[‹N‚±‚·‚Ì‚ÅƒIƒt‚É‚·‚é
					WriteProcessMemory(pi.hProcess, (LPVOID)UNCHECK_JOYSTICK_95, UNCHECK_JOYSTICK_95_CODE, sizeof(UNCHECK_JOYSTICK_95_CODE), NULL);

					// ”M‘Ñ’†‚Íí‚É1P“ü—Í
					if(run_type == RT_VS){
						WriteProcessMemory(pi.hProcess, (LPVOID)CONTROL_HOOK1_95, CONTROL_HOOK1_95_CODE, sizeof(CONTROL_HOOK1_95_CODE), NULL);
						WriteProcessMemory(pi.hProcess, (LPVOID)CONTROL_HOOK2_95, CONTROL_HOOK2_95_CODE, sizeof(CONTROL_HOOK2_95_CODE), NULL);
					}

					// ƒL[î•ñ‚ðŽæ“¾
					WriteProcessMemory(pi.hProcess, (LPVOID)STORY_P1_KEY_95, STORY_P1_KEY_95_CODE, sizeof(STORY_P1_KEY_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)STORY_P2_KEY_95, STORY_P2_KEY_95_CODE, sizeof(STORY_P2_KEY_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_P1_KEY_95, VS_P1_KEY_95_CODE, sizeof(VS_P1_KEY_95_CODE), NULL);
					WriteProcessMemory(pi.hProcess, (LPVOID)VS_P2_KEY_95, VS_P2_KEY_95_CODE, sizeof(VS_P2_KEY_95_CODE), NULL);
				}

				FlushInstructionCache(pi.hProcess, NULL, 0);
				MTINFO.INITIALIZED = true;
				break;

			case EXIT_THREAD_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("EXIT_THREAD_DEBUG_EVENT\n", DebugMessageColor);
				}
				break;

			case EXIT_PROCESS_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("EXIT_PROCESS_DEBUG_EVENT\n", DebugMessageColor);
				}
				break;

			case LOAD_DLL_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					TCHAR buf[MAX_PATH];
					LONG_PTR data;

					ReadProcessMemory(pi.hProcess, de.u.LoadDll.lpImageName, &data, sizeof(LONG_PTR), NULL);
					ReadProcessMemory(pi.hProcess, (LPVOID)data, buf, sizeof(TCHAR)*MAX_PATH, NULL);

					WriteMessage(String::Format("LOAD_DLL_DEBUG_EVENT > \"{0}\"\n", gcnew String(buf)), DebugMessageColor);
				}
				break;

			case UNLOAD_DLL_DEBUG_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("UNLOAD_DLL_DEBUG_EVENT\n", DebugMessageColor);
				}
				break;

			case OUTPUT_DEBUG_STRING_EVENT:
				if(MTINFO.DEBUG){
					int size = de.u.DebugString.nDebugStringLength;
					char *str = (char *)malloc(size);
					ReadProcessMemory(pi.hProcess, de.u.DebugString.lpDebugStringData, str, size, NULL);

					if(str[size-2] == '\n'){
						str[size-2] = '\0';
					}

					WriteMessage(String::Format("OUTPUT_DEBUG_STRING_EVENT > {0}\n", gcnew String(str)), DebugMessageColor);
					free(str);
				}
				break;

			case RIP_EVENT:
				if(MTINFO.DEBUG){
					WriteMessage("RIP_EVENT\n", DebugMessageColor);
				}
				break;
			}

			ContinueDebugEvent(de.dwProcessId, de.dwThreadId, state);
			if(de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) break;
		}
	}
	catch(Exception^ e){
		WriteMessage(e->ToString() + "\n", ErrorMessageColor);
	}
	finally{
		AllowWatch = false;
		InputFrame = 0;

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		

		if(br != nullptr){
			br->Close();
		}
		if(bw != nullptr){
			bw->Write(ri.KEY[0]);
			bw->Write(ri.COUNT[0]);
			bw->Write((UINT16)(ri.KEY[1] | 0x0800));
			bw->Write(ri.COUNT[1]);

			bw->Close();
		}
		

		if(MTINFO.INITIALIZED){
			if(WaitingWatch > 0){
				WaitingWatch = 0;
				WriteMessage("Stopped watching.\n", SystemMessageColor);
			}
			else if(_tcslen(MTINFO.ORIGINAL_TITLE) > 0){
				WriteMessage(String::Format("Ended \"{0}\".\n", gcnew String(MTINFO.ORIGINAL_TITLE)), SystemMessageColor);

				if(run_type == RT_VS){
					if(MTINFO.CONTROL == 1){
						i = p1_win;
						p1_win = p2_win;
						p2_win = i;
					}
					WriteTime(0, SystemMessageColor);
					WriteMessage(String::Format("Score: {0} matches, {1} wins (P1), {2} wins (P2)\n", num_vs, p1_win, p2_win), SecretColor);
				}
			}
			else{
				WriteMessage("Ended the game.\n", SystemMessageColor);
			}
		}

		if(NetVS != nullptr && NetVS->SEQUENCE == VS_DATA){
			NetVS->SEQUENCE = VS_END;
			UDP->Send(gcnew array<BYTE>{ PH_VS_END }, 1, NetVS->IP_EP);

			// ƒƒbƒNŠJ•ú‘Ò‚¿
			Monitor::Enter(NetVS->LOCAL);
			Monitor::Exit(NetVS->LOCAL);
			Monitor::Enter(NetVS->REMOTE);
			Monitor::Exit(NetVS->REMOTE);

			delete NetVS;
			NetVS = nullptr;
		}

		if(allow_spectator){
			AllowWatch = false;
			SpectacleThread->Join();
		}

		if(AfterCloseUDP){
			AfterCloseUDP = false;
			UDP->Close();
		}
		else if(run_type == RT_WATCH){
			QuitWatch(true);
		}
		else{
			// ‘ÎíI—¹‚ð’Ê’m
			if(MTOPTION.AFTER_REST && run_type == RT_VS){
				ChangeState((BYTE)MS_REST);
			}
			else{
				if(MemberList[0]->STATE != MS_SEEK){
					ChangeState((BYTE)MS_FREE);
				}
			}
		}

		MTINFO.INITIALIZED = false;
		MTINFO.SHOW_TOP    = false;
		MTINFO.PROCESS     = NULL;
		MTINFO.PROCESS_ID  = 0;
		MTINFO.HWND        = NULL;
		ZeroMemory(MTINFO.ORIGINAL_TITLE, sizeof(MTINFO.ORIGINAL_TITLE));
		ZeroMemory(MTINFO.TITLE, sizeof(MTINFO.TITLE));

		if(IsFormClosing){
			this->Close();
		}
	}
}

void MainForm::RunVersus()
{
	array<BYTE>^ send = gcnew array<BYTE>(7);

	try{
		// ‘Îí—v‹
		send[0] = PH_REQ_VS;
		Array::Copy(BitConverter::GetBytes(MemberList[0]->ID), 0, send, 1, 2);
		Array::Copy(BitConverter::GetBytes((INT32)(Path::GetFileNameWithoutExtension(gcnew String(MTOPTION.GAME_EXE))->GetHashCode())), 0, send, 3, 4);
		UDP->Send(send, 7, NetVS->IP_EP);

		NetVS->SLEEPING = true;
		try{
			Thread::Sleep(TIME_OUT);
		}
		catch(ThreadInterruptedException^){
		}
		NetVS->SLEEPING = false;

		if(NetVS->SEQUENCE != VS_PING){
			if(NetVS->SEQUENCE != VS_ERROR){
				WriteMessage("\nERROR: There was no response from the opponent.\n", ErrorMessageColor);
			}
			return;
		}

		// PING‚Å‘Ñˆæ’²¸
		send[0] = PH_REQ_VS_PING;

		for(BYTE i = 0; i < 5; i++){
			NetVS->PING[i] = timeGetTime();
			send[1] = i;
			UDP->Send(send, 2, NetVS->IP_EP);

			Thread::Sleep(10);
		}

		if(NetVS->SEQUENCE == VS_PING){
			NetVS->SLEEPING = true;
			try{
				// Å’á‚Å‚à‚P•bŠÔ‚ÉƒpƒP‚S‚Â‚Í—~‚µ‚¢‚Æ‚±‚ë
				Thread::Sleep(1000);
			}
			catch(ThreadInterruptedException^){
			}
			NetVS->SLEEPING = false;
		}

		if(NetVS->SEQUENCE != VS_SETTING){
			WriteMessage("ERROR: The connection to the opponent is unstable.\n", ErrorMessageColor);
			return;
		}

		// DELAY‚ÌÝ’è
		send[0] = PH_REQ_VS_SETTING;

		if(MTOPTION.DELAY == 0){
			// ƒI[ƒgÝ’è
			int d = 0, c = 0;

			for(int i = 0; i < 5; i++){
				if(NetVS->PONG[i] > 0){
					d += NetVS->PONG[i] - NetVS->PING[i];
					c++;

					if(c == 4) break;
				}
			}

			if(c < 4){
				WriteMessage("ERROR: Could not determine the delay from the opponent's ping.B\n", ErrorMessageColor);
				return;
			}

			c = d/40 + 2;  // Delay = PING/10 + 2

			if(c > 12) c = 12;

			NetVS->DELAY = c;
		}
		else{
			NetVS->DELAY = MTOPTION.DELAY;
		}

		send[1] = (BYTE)NetVS->DELAY;
		UDP->Send(send, 2, NetVS->IP_EP);

		NetVS->SLEEPING = true;
		try{
			Thread::Sleep(TIME_OUT);
		}
		catch(ThreadInterruptedException^){
		}
		NetVS->SLEEPING = false;

		if(NetVS->SEQUENCE != VS_STANDBY){
			WriteMessage("ERROR: The match initialization process could not be completed.\n", ErrorMessageColor);
			return;
		}

		NetVS->SEQUENCE = VS_DATA;

		// ‘ÎíŠJŽn
		WriteMessage(String::Format("Match starting! (Delay: {0})\n", NetVS->DELAY), SystemMessageColor);

		GameThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &MainForm::RunGame));
		GameThread->Start((UINT)RT_VS);
	}
	finally{
		if(NetVS->SEQUENCE < VS_DATA || NetVS->SEQUENCE == VS_ERROR){
			MemberList[0]->STATE = MS_FREE;

			delete NetVS;
			NetVS = nullptr;
		}
	}
}

void MainForm::RunSpectacle()
{
	int i, d;
	UINT32 frame = 0, s_frame;

	// ŠJŽn‡}
	array<BYTE>^ send = gcnew array<BYTE>(14);
	send[0] = PH_RES_WATCH;
	send[1] = 0;
	Array::Copy(BitConverter::GetBytes(P1ID), 0, send, 2, 2);
	Array::Copy(BitConverter::GetBytes(P2ID), 0, send, 4, 2);
	Array::Copy(BitConverter::GetBytes(MTINFO.SEED), 0, send, 6, 4);
	send[10] = MTINFO.MAX_STAGE;
	send[11] = MTINFO.STAGE_SELECT;
	send[12] = MTINFO.ROUND;
	send[13] = MTINFO.TIMER;

	Monitor::Enter(InputHistory);
	try{
		for(i = 0; i < SpectatorList->Count; i++){
			UDP->BeginSend(send, send->Length, SpectatorList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
		}
	}
	finally{
		Monitor::Exit(InputHistory);
	}

	Thread::Sleep(100);

	// ƒf[ƒ^‘—M€”õ
	send = gcnew array<BYTE>(45){ PH_WATCH_DATA };

	// €”õŠ®—¹
	AllowWatch = true;

	// ƒf[ƒ^‘—M
	while(AllowWatch){
		Monitor::Enter(InputHistory);
		try{
			frame = InputFrame;

			for(i = 0; i < SpectatorList->Count; i++){
				s_frame = SpectatorList[i]->FRAME;
/*
				// ƒf[ƒ^‚ÍŠù‚Éƒoƒbƒtƒ@‚©‚ç‚È‚­‚È‚Á‚Ä‚é
				if(frame > s_frame && (frame - s_frame) >= (UINT)InputHistory->Length - 20){
					if(MTINFO.DEBUG) WriteMessage(String::Format("ŠÏíŽÒ‚Ì’Ç‚¢o‚µ > {0} / {1}\n", SpectatorList[i]->ID, SpectatorList->Count), DebugMessageColor);

					send[0] = PH_WATCH_END;
					Array::Copy(BitConverter::GetBytes(SpectatorList[i]->ID), 0, send, 1, 2);
					UDP->BeginSend(send, 3, SpectatorList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);

					SpectatorList->RemoveAt(i);
					i--;

					send[0] = PH_WATCH_DATA;
					continue;
				}
*/
				// 5ƒtƒŒ[ƒ€‚²‚Æ‚É10ƒtƒŒ[ƒ€•ª‘—‚é
				if(s_frame + 20 < frame){
					Array::Copy(BitConverter::GetBytes(s_frame), 0, send, 1, 4);

					for(d = 0; d < 20; d++){
						Array::Copy(BitConverter::GetBytes(InputHistory[(s_frame + d) % InputHistory->Length]), 0, send, 5 + d*2, 2);
					}

					UDP->BeginSend(send, send->Length, SpectatorList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);

					// 0frame‚ÌŽž‚Í”O‚Ì‚½‚ß‚QŒÂ‘—M
					if(s_frame == 0){
						UDP->BeginSend(send, send->Length, SpectatorList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
					}

					SpectatorList[i]->FRAME += 10;
				}
			}
		}
		finally{
			Monitor::Exit(InputHistory);
		}

		// ‚¿‚å‚Á‚Æ‚¾‚¯‹xŒe
		Thread::Sleep(20);
	}

	// I—¹‡}
	send[0] = PH_WATCH_END;

	Monitor::Enter(InputHistory);
	try{
		for(i = 0; i < SpectatorList->Count; i++){
			Array::Copy(BitConverter::GetBytes(SpectatorList[i]->ID), 0, send, 1, 2);
			UDP->BeginSend(send, 3, SpectatorList[i]->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
		}

		SpectatorList->Clear();
	}
	finally{
		Monitor::Exit(InputHistory);
	}
}

UINT16 MainForm::LocalInput(UINT16 eax)
{
	// ¶‰E“¯Žž‰Ÿ‚µ‹ÖŽ~ if((eax & 3) == 3) eax ^= 2
	if((eax & 0x0003) == 0x0003){
		eax &= 0xFFFD;
	}
	
	Monitor::Enter(NetVS->LOCAL);
	try{
		NetVS->LOCAL[(NetVS->L_READ + NetVS->DELAY) % NetVS->LOCAL->Length] = eax;
		eax = NetVS->LOCAL[NetVS->L_READ];

		bool send = true;

		if(NetVS->INTERVAL > 0){
			if(NetVS->INTERVAL == 1){
				// ŠÔŠu‚P(’èŠú‹xŒeƒ‚[ƒh)
				if((NetVS->L_FRAME % NetVS->INTERVAL2) == (NetVS->INTERVAL2 - 1)){
					send = false;
				}
			}
			else{
				// ŠÔŠu‚QˆÈã
				if((NetVS->L_FRAME % NetVS->INTERVAL) != 0){
					send = false;
				}
			}
		}

		if(send){
			// ‘—MŠJŽn
			Array::Copy(BitConverter::GetBytes(NetVS->L_FRAME), 0, NetVS->SEND, 1, 4);

			UINT i;
			for(i = 0; i <= NetVS->DELAY; i++){
				Array::Copy(BitConverter::GetBytes(NetVS->LOCAL[(NetVS->L_READ + i) % NetVS->LOCAL->Length]), 0, NetVS->SEND, 5 + i*2, 2);
			}

			UDP->BeginSend(NetVS->SEND, 5 + i*2, NetVS->IP_EP, gcnew AsyncCallback(SendPackets), UDP);
		}

		NetVS->L_FRAME++;
		NetVS->L_READ++;
		NetVS->L_READ %= NetVS->LOCAL->Length;
	}
	finally{
		Monitor::Exit(NetVS->LOCAL);
	}

	return eax;
}

UINT16 MainForm::RemoteInput()
{
	if(NetVS->SEQUENCE != VS_DATA) return 0;

	UINT16 eax;

	eax = NetVS->REMOTE[NetVS->R_READ];

	// ƒpƒP—ˆ‚Ä‚È‚¢
	if(eax == 0xFFFF){
		Monitor::Enter(NetVS->REMOTE);
		try{
			// ¡—ˆ‚Ä‚é‚©‚à
			NetVS->WAITING = 2;
			Monitor::Wait(NetVS->REMOTE, NetVS->DELAY*10);
			eax = NetVS->REMOTE[NetVS->R_READ];
		}
		finally{
			Monitor::Exit(NetVS->REMOTE);
		}
	}

	// ƒƒX‚Á‚½H
	if(eax == 0xFFFF){
		NetVS->WAITING = 1;

		array<BYTE>^ send = gcnew array<BYTE>(5){ PH_REQ_VS_DATA };
		Array::Copy(BitConverter::GetBytes(NetVS->R_FRAME), 0, send, 1, 4);

		UINT i = 0;

		while(NetVS->SEQUENCE == VS_DATA){
			UDP->BeginSend(send, send->Length, NetVS->IP_EP, gcnew AsyncCallback(SendPackets), UDP);

			Monitor::Enter(NetVS->REMOTE);
			try{
				NetVS->WAITING = 2;
				Monitor::Wait(NetVS->REMOTE, NetVS->DELAY*10);
			}
			finally{
				Monitor::Exit(NetVS->REMOTE);
			}

			if(NetVS->WAITING == 2){
				NetVS->WAITING = 1;
				if(++i > 100){
					NetVS->SEQUENCE = VS_TIMEOUT;
				}
				continue;
			}

			eax = NetVS->REMOTE[NetVS->R_READ];

			if(eax != 0xFFFF){
				break;
			}
			else{
				i = 0;
				// ‚±‚±‚ª‚©‚Ì—L–¼‚Èƒ‰ƒO‚Ì¶ŽY’n‚Å‚·
				// Delay*10 / 2   ƒfƒBƒŒƒC‚Ì”¼ƒtƒŒ[ƒ€•ª‚Í‘Ò‹@‚µ‚Ä‚à‚æ‚³‚»‚¤H
				Thread::Sleep(NetVS->DELAY*5);
			}
		}

		if(eax == 0xFFFF && NetVS->SEQUENCE == VS_TIMEOUT){
			WriteMessage("The match ended due to a timeout.\n", ErrorMessageColor);
			if(MTINFO.DEBUG){
				WriteMessage(String::Format("Frame > L:{0} / R:{1} (delay:{2})\n", NetVS->L_FRAME, NetVS->R_FRAME, NetVS->DELAY), DebugMessageColor);
			}

			QuitGame();
			return 0;
		}

		NetVS->WAITING = 0;
	}

	if(NetVS->SEQUENCE != VS_DATA) return 0;

	if((eax & 0x0003) == 0x0003){
		WriteMessage("Received illegal packet: Press left and right simultaneously.\n", ErrorMessageColor);
		if(MTINFO.DEBUG){
			WriteMessage(String::Format("IP = {0}\n", NetVS->IP_EP->Address), DebugMessageColor);
		}

		QuitGame();
		return 0xFFFF;
	}

	Monitor::Enter(NetVS->REMOTE);
	try{
		NetVS->REMOTE[NetVS->R_READ] = 0xFFFF;

		NetVS->R_FRAME++;
		NetVS->R_READ++;
		NetVS->R_READ %= NetVS->REMOTE->Length;
	}
	finally{
		Monitor::Exit(NetVS->REMOTE);
	}

	return eax;
}

UINT16 MainForm::ReadReplayData(BinaryReader^ br, REPLAY_INFO& ri)
{
	if(br == nullptr){
		return 0;
	}

	UINT16 eax;

	try{
		if(ri.VERSION == _T('1') || ri.VERSION == _T('3')){
			eax = br->ReadUInt16();
		}
		else{
			// ƒŠƒvƒŒƒCƒtƒ@ƒCƒ‹‚ÌRLEƒfƒR[ƒh
			if(ri.COUNT[ri.CONTROL] == 0){
				REPLAY_DATA rd;
				bool ctrl;

				if(ri.DEQUE[ri.CONTROL].size() > 0){
					// ƒLƒ…[‚Éƒf[ƒ^‚ ‚è
					rd = ri.DEQUE[ri.CONTROL].front();
					ri.DEQUE[ri.CONTROL].pop_front();
				}
				else{
					rd.KEY   = br->ReadUInt16();
					rd.COUNT = br->ReadByte();
					ctrl     = (rd.KEY & 0x0800) > 0 ? true : false;

					while(ctrl != ri.CONTROL){
						// ƒLƒ…[‚É‘Þ”ð
						ri.DEQUE[ctrl].push_back(rd);

						// Ä“Ç‚Ýž‚Ý
						rd.KEY   = br->ReadUInt16();
						rd.COUNT = br->ReadByte();
						ctrl     = (rd.KEY & 0x0800) > 0 ? true : false;
					}
				}

				ri.KEY[ri.CONTROL]   = rd.KEY & 0x07FF;
				ri.COUNT[ri.CONTROL] = rd.COUNT;
			}
			else{
				ri.COUNT[ri.CONTROL]--;
			}

			eax = ri.KEY[ri.CONTROL];

			ri.CONTROL ^= 1;
		}
	}
	catch(EndOfStreamException^){
		WriteMessage("End of replay file.\n", SystemMessageColor);

		br->Close();
		br = nullptr;

		QuitGame();
		return 0;
	}
	catch(ObjectDisposedException^){
		br = nullptr;

		return 0;
	}

	return eax;
}

void MainForm::RecordInput(UINT16 eax, BinaryWriter^ bw, REPLAY_INFO& ri, bool watch)
{
	// ƒŠƒvƒŒƒCƒtƒ@ƒCƒ‹‚Ì‹L˜^
	if(bw != nullptr){
		if(ri.VERSION == _T('3')){
			bw->Write(eax);
		}
		else{
			// RLE
			if(ri.KEY[ri.CONTROL] == 0xFFFF){
				// ‰‰ñ
				ri.KEY[ri.CONTROL] = eax;
			}
			else{
				if(eax == ri.KEY[ri.CONTROL]){
					// ˆêŽü‚µ‚Ä‚«‚½
					if(ri.COUNT[ri.CONTROL] == 0xFF){
						bw->Write((UINT16)(ri.KEY[ri.CONTROL] | ri.CONTROL << 11));
						bw->Write(ri.COUNT[ri.CONTROL]);

						ri.COUNT[ri.CONTROL] = 0;
					}
					else{
						ri.COUNT[ri.CONTROL]++;
					}
				}
				else{
					bw->Write((UINT16)(ri.KEY[ri.CONTROL] | ri.CONTROL << 11));
					bw->Write(ri.COUNT[ri.CONTROL]);

					ri.KEY[ri.CONTROL]   = eax;
					ri.COUNT[ri.CONTROL] = 0;
				}
			}

			ri.CONTROL ^= 1;
		}
	}

	// ŠÏíOK
	if(watch && UDP != nullptr){
		Monitor::Enter(InputHistory);
		try{
			InputHistory[InputFrame % InputHistory->Length] = eax;
			InputFrame++;
		}
		finally{
			Monitor::Exit(InputHistory);
		}
	}
}
void MainForm::RunAutoRest() {
	// Ž©“®‹xŒe
	LASTINPUTINFO li;
	li.cbSize = sizeof(LASTINPUTINFO);
	DWORD te;
	int to = MTOPTION.AUTO_REST_TIME;  // ƒ^ƒCƒ€ƒAƒEƒgŽw’èŽžŠÔ
	AutoRestRanging = true;
	if(MTINFO.DEBUG){
		WriteMessage(String::Format("Auto rest thread starts in {0} minute(s)!\n", to), DebugMessageColor);
	}
	while(AutoRestRanging){
		AutoRestSleeping = true;
		try{
			Thread::Sleep(1000*1);
		}
		catch(ThreadInterruptedException^){
		}
		finally{
			AutoRestSleeping = false;
		}
		if(!AutoRestRanging) return;
		GetLastInputInfo(&li);
		te = GetTickCount();
		to = MTOPTION.AUTO_REST_TIME;

		if(MTINFO.DEBUG){
			// WriteMessage(String::Format("{0}•bŒo‰ß\n", ((te-li.dwTime)/1000) ), DebugMessageColor);
		}

		if(((te-li.dwTime)/1000) >= UINT(to*60)){
			// ƒtƒŠ[ó‘Ô‚È‚ç‹xŒeó‘Ô‚É‚·‚é
			if(UDP != nullptr && MemberList[0]->STATE == MS_FREE){
				ChangeState((BYTE)MS_REST);
				WriteMessage("Rest mode set to ON.\n", SystemMessageColor);
			}
			AutoRestRanging = false;
			AutoRestThread = nullptr;
		}
	}
}
void MainForm::ChangeSeek() {
	if(MTOPTION.CONNECTION_TYPE == CT_FREE) return;

	if(MemberList[0]->STATE == MS_FREE){
		ChangeState((BYTE)MS_SEEK);
		WriteMessage("Seek mode set to ON.", SystemMessageColor);
		WriteTime(0, SystemMessageColor);
		WriteMessage(String::Format("{0} is now in seek mode.\n", (ServerMode == SM_NORA ? L"<>" :
																			MemberList[0]->NAME)), SystemMessageColor);

	}
	else if(MemberList[0]->STATE == MS_SEEK){
		if(GameThread != nullptr && GameThread->IsAlive){
			ChangeState((BYTE)MS_VS);
		}else{
			ChangeState((BYTE)MS_FREE);
		}
		WriteMessage("Seek mode set to OFF.\n", SystemMessageColor);
		WriteTime(0, SystemMessageColor);
		WriteMessage(String::Format("{0} has left seek mode.\n", (ServerMode == SM_NORA ? L"<>" :
																			MemberList[0]->NAME)), SystemMessageColor);
	}
}
void MainForm::ChangeLogWordWrap() {
	MTOPTION.LOG_WORDWRAP ^= 1;
	richTextBoxLog->WordWrap = MTOPTION.LOG_WORDWRAP;
	toolStripMenuItemWordWrap->Checked = MTOPTION.LOG_WORDWRAP;
	if(MTOPTION.LOG_WORDWRAP){
		WriteMessage("Text wrap set to on.\n", SystemMessageColor);
	}else{
		WriteMessage("Text wrap set to off.", SystemMessageColor);
	}
}
void MainForm::ClearLog(){
	if(MessageBox::Show("This will remove the entire log.\nAre you sure you want to clear the log?", "Clear Log", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == ::DialogResult::Yes){
	}else{
		return;
	}
	Monitor::Enter(richTextBoxLog);
	try{
		// ƒƒOíœ
		richTextBoxLog->Clear();

		WriteTime(0, SystemMessageColor);
		WriteMessage("[Deleted]\n", SystemMessageColor);

		if(MemberList[0]->TYPE == CT_FREE){return;}

		if(MTOPTION.LOG_CLEAR_WITHOUT_WELCOME){ // ƒT[ƒo’mŽc‚·
			WriteMessage("[MOTD]---------------------\n", SystemMessageColor);
			richTextBoxLog->SelectionFont = gcnew Drawing::Font(richTextBoxLog->Font->FontFamily, richTextBoxLog->Font->Size + 2);
			richTextBoxLog->SelectionColor = TalkMessageColor;
			richTextBoxLog->SelectionBackColor = NoticeBackColor;
			richTextBoxLog->AppendText(gcnew String(MTOPTION.WELCOME)+"\n");
			WriteMessage("-------------------------------\n", SystemMessageColor);
		}
		
	}
	catch(Exception ^e){
		WriteErrorLog(e->ToString(), "SaveLog");
	}
	finally{
		Monitor::Exit(richTextBoxLog);
	}
}
void MainForm::SaveLog(){
	// Saves the current chat log to either a .rtf or .txt file determined by the LilithPort Settings.
	String^ path = gcnew String(MTOPTION.PATH);
	String^ file = String::Format("LilithPort_{0}.{1}", DateTime::Now.ToString("yyyyMMdd-HHmmss"),
														MTOPTION.LOG_FORMAT_RTF ? "rtf" : "txt");
	path += file;
	Monitor::Enter(richTextBoxLog);
	try{
		if(MTOPTION.LOG_FORMAT_RTF){
			richTextBoxLog->SaveFile(path, RichTextBoxStreamType::RichText);
		}else{
			richTextBoxLog->SaveFile(path, RichTextBoxStreamType::PlainText);
		}
	}
	catch(Exception ^e){
		WriteErrorLog(e->ToString(), "SaveLog");
	}
	finally{
		Monitor::Exit(richTextBoxLog);
	}

	WriteMessage(String::Format("Log written to \"{0}\"\n", file), SystemMessageColor);
}
