#pragma once

#include "stdafx.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace LilithPort {

	/// <summary>
	/// StartupForm �̊T�v
	///
	/// �x��: ���̃N���X�̖��O��ύX����ꍇ�A���̃N���X���ˑ����邷�ׂĂ� .resx �t�@�C���Ɋ֘A�t����ꂽ
	///          �}�l�[�W ���\�[�X �R���p�C�� �c�[���ɑ΂��� 'Resource File Name' �v���p�e�B��
	///          �ύX����K�v������܂��B���̕ύX���s��Ȃ��ƁA
	///          �f�U�C�i�ƁA���̃t�H�[���Ɋ֘A�t����ꂽ���[�J���C�Y�ς݃��\�[�X�Ƃ��A
	///          ���������݂ɗ��p�ł��Ȃ��Ȃ�܂��B
	/// </summary>
	public ref class StartupForm : public System::Windows::Forms::Form
	{
	public:
		StartupForm(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^ �R�[�h��ǉ����܂�
			//
		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~StartupForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::RadioButton^  radioButtonServer;
	private: System::Windows::Forms::RadioButton^  radioButtonHost;
	private: System::Windows::Forms::RadioButton^  radioButtonClient;
	private: System::Windows::Forms::Label^  labelServerName;
	private: System::Windows::Forms::TextBox^  textBoxIP;
	private: System::Windows::Forms::Label^  labelOpenPort;
	private: System::Windows::Forms::NumericUpDown^  numericUpDownOpenPort;
	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonCancel;







	private: System::Windows::Forms::GroupBox^  groupBoxStartMode;
	private: System::Windows::Forms::TextBox^  textBoxServerName;
	private: System::Windows::Forms::GroupBox^  groupBoxConnection;



	private: System::Windows::Forms::GroupBox^  groupBoxWelcome;
	private: System::Windows::Forms::RichTextBox^  textBoxWelcome;
	private: System::Windows::Forms::ToolTip^  toolTipStartupForm;
	private: System::Windows::Forms::Button^  buttonConnect;
	private: System::Windows::Forms::Button^  buttonOpenPortUPnP;
	private: System::Windows::Forms::Button^  buttonClosePortUPnP;
	private: System::Windows::Forms::Label^  labelButtonUPnP;
	private: System::Windows::Forms::TabControl^  startupTabs;

	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::Label^  labelComment;
	private: System::Windows::Forms::Label^  labelName;
	private: System::Windows::Forms::TextBox^  textBoxComment;
	private: System::Windows::Forms::TextBox^  textBoxName;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::Label^  label1;







	private: System::ComponentModel::IContainer^  components;
	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(StartupForm::typeid));
			this->radioButtonServer = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHost = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonClient = (gcnew System::Windows::Forms::RadioButton());
			this->labelServerName = (gcnew System::Windows::Forms::Label());
			this->textBoxIP = (gcnew System::Windows::Forms::TextBox());
			this->labelOpenPort = (gcnew System::Windows::Forms::Label());
			this->numericUpDownOpenPort = (gcnew System::Windows::Forms::NumericUpDown());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->groupBoxStartMode = (gcnew System::Windows::Forms::GroupBox());
			this->labelButtonUPnP = (gcnew System::Windows::Forms::Label());
			this->buttonClosePortUPnP = (gcnew System::Windows::Forms::Button());
			this->buttonOpenPortUPnP = (gcnew System::Windows::Forms::Button());
			this->textBoxServerName = (gcnew System::Windows::Forms::TextBox());
			this->groupBoxConnection = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxWelcome = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxWelcome = (gcnew System::Windows::Forms::RichTextBox());
			this->toolTipStartupForm = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->buttonConnect = (gcnew System::Windows::Forms::Button());
			this->labelComment = (gcnew System::Windows::Forms::Label());
			this->labelName = (gcnew System::Windows::Forms::Label());
			this->startupTabs = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->textBoxComment = (gcnew System::Windows::Forms::TextBox());
			this->textBoxName = (gcnew System::Windows::Forms::TextBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownOpenPort))->BeginInit();
			this->groupBoxStartMode->SuspendLayout();
			this->groupBoxConnection->SuspendLayout();
			this->groupBoxWelcome->SuspendLayout();
			this->startupTabs->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->SuspendLayout();
			// 
			// radioButtonServer
			// 
			this->radioButtonServer->AutoSize = true;
			this->radioButtonServer->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->radioButtonServer->Location = System::Drawing::Point(6, 6);
			this->radioButtonServer->Name = L"radioButtonServer";
			this->radioButtonServer->Size = System::Drawing::Size(62, 18);
			this->radioButtonServer->TabIndex = 4;
			this->radioButtonServer->Text = L"Server";
			this->toolTipStartupForm->SetToolTip(this->radioButtonServer, L"�T�[�o�����Ăđ��̐ڑ��҂�҂��󂯂܂��B\r\n�w�肵���Ҏ�|�[�g(UDP)���J������K�v��" 
				L"����܂��B");
			this->radioButtonServer->UseVisualStyleBackColor = true;
			this->radioButtonServer->CheckedChanged += gcnew System::EventHandler(this, &StartupForm::radioButtonServer_CheckedChanged);
			// 
			// radioButtonHost
			// 
			this->radioButtonHost->AutoSize = true;
			this->radioButtonHost->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->radioButtonHost->Location = System::Drawing::Point(6, 29);
			this->radioButtonHost->Name = L"radioButtonHost";
			this->radioButtonHost->Size = System::Drawing::Size(53, 18);
			this->radioButtonHost->TabIndex = 5;
			this->radioButtonHost->Text = L"Host";
			this->toolTipStartupForm->SetToolTip(this->radioButtonHost, L"�T�[�o�ɐڑ����܂��B\r\n�w�肵���Ҏ�|�[�g(UDP)��ʐM�Ɏg�p���܂��B\r\n(�|�[�g���J���" 
				L"���Ɠ��삪���肵�܂��B)");
			this->radioButtonHost->UseVisualStyleBackColor = true;
			this->radioButtonHost->CheckedChanged += gcnew System::EventHandler(this, &StartupForm::radioButtonHost_CheckedChanged);
			// 
			// radioButtonClient
			// 
			this->radioButtonClient->AutoSize = true;
			this->radioButtonClient->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->radioButtonClient->Location = System::Drawing::Point(6, 52);
			this->radioButtonClient->Name = L"radioButtonClient";
			this->radioButtonClient->Size = System::Drawing::Size(57, 18);
			this->radioButtonClient->TabIndex = 6;
			this->radioButtonClient->Text = L"Client";
			this->toolTipStartupForm->SetToolTip(this->radioButtonClient, L"�T�[�o�ɐڑ����܂��B\r\n�|�[�g�͎����I�Ɋ��蓖�Ă��܂��B\r\n(�|�[�g���J�����Ȃ��Ă�" 
				L"�ڑ��ł��܂����A���삪���s����ł��B)");
			this->radioButtonClient->UseVisualStyleBackColor = true;
			this->radioButtonClient->CheckedChanged += gcnew System::EventHandler(this, &StartupForm::radioButtonClient_CheckedChanged);
			// 
			// labelServerName
			// 
			this->labelServerName->AutoSize = true;
			this->labelServerName->Location = System::Drawing::Point(9, 27);
			this->labelServerName->Name = L"labelServerName";
			this->labelServerName->Size = System::Drawing::Size(70, 13);
			this->labelServerName->TabIndex = 0;
			this->labelServerName->Text = L"Server name:";
			this->toolTipStartupForm->SetToolTip(this->labelServerName, L"The name your server will have.\nThis is not the address.");
			this->labelServerName->MouseLeave += gcnew System::EventHandler(this, &StartupForm::label_MouseLeave);
			this->labelServerName->MouseEnter += gcnew System::EventHandler(this, &StartupForm::label_MouseEnter);
			// 
			// textBoxIP
			// 
			this->textBoxIP->Location = System::Drawing::Point(81, 25);
			this->textBoxIP->Name = L"textBoxIP";
			this->textBoxIP->Size = System::Drawing::Size(256, 20);
			this->textBoxIP->TabIndex = 11;
			this->textBoxIP->WordWrap = false;
			// 
			// labelOpenPort
			// 
			this->labelOpenPort->AutoSize = true;
			this->labelOpenPort->Location = System::Drawing::Point(9, 56);
			this->labelOpenPort->Name = L"labelOpenPort";
			this->labelOpenPort->Size = System::Drawing::Size(62, 13);
			this->labelOpenPort->TabIndex = 0;
			this->labelOpenPort->Text = L"Server port:";
			this->toolTipStartupForm->SetToolTip(this->labelOpenPort, resources->GetString(L"labelOpenPort.ToolTip"));
			this->labelOpenPort->MouseLeave += gcnew System::EventHandler(this, &StartupForm::label_MouseLeave);
			this->labelOpenPort->MouseEnter += gcnew System::EventHandler(this, &StartupForm::label_MouseEnter);
			// 
			// numericUpDownOpenPort
			// 
			this->numericUpDownOpenPort->Location = System::Drawing::Point(77, 54);
			this->numericUpDownOpenPort->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			this->numericUpDownOpenPort->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
			this->numericUpDownOpenPort->Name = L"numericUpDownOpenPort";
			this->numericUpDownOpenPort->Size = System::Drawing::Size(53, 20);
			this->numericUpDownOpenPort->TabIndex = 8;
			this->numericUpDownOpenPort->Tag = L"";
			this->numericUpDownOpenPort->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7500, 0, 0, 0});
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(272, 279);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(80, 26);
			this->buttonOK->TabIndex = 12;
			this->buttonOK->Text = L"OK";
			this->toolTipStartupForm->SetToolTip(this->buttonOK, L"�T�[�o�҂��󂯂��J�n���܂��B");
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &StartupForm::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(358, 279);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(80, 26);
			this->buttonCancel->TabIndex = 14;
			this->buttonCancel->Text = L"Cancel";
			this->toolTipStartupForm->SetToolTip(this->buttonCancel, L"�I�t���C���Ńt���[�v���C���[�h�Ɉڍs���܂��B");
			this->buttonCancel->UseVisualStyleBackColor = true;
			this->buttonCancel->Click += gcnew System::EventHandler(this, &StartupForm::buttonCancel_Click);
			// 
			// groupBoxStartMode
			// 
			this->groupBoxStartMode->Controls->Add(this->labelButtonUPnP);
			this->groupBoxStartMode->Controls->Add(this->buttonClosePortUPnP);
			this->groupBoxStartMode->Controls->Add(this->buttonOpenPortUPnP);
			this->groupBoxStartMode->Controls->Add(this->textBoxServerName);
			this->groupBoxStartMode->Controls->Add(this->labelOpenPort);
			this->groupBoxStartMode->Controls->Add(this->labelServerName);
			this->groupBoxStartMode->Controls->Add(this->numericUpDownOpenPort);
			this->groupBoxStartMode->Location = System::Drawing::Point(68, 6);
			this->groupBoxStartMode->Name = L"groupBoxStartMode";
			this->groupBoxStartMode->Size = System::Drawing::Size(343, 124);
			this->groupBoxStartMode->TabIndex = 8;
			this->groupBoxStartMode->TabStop = false;
			this->groupBoxStartMode->Text = L"Server";
			// 
			// labelButtonUPnP
			// 
			this->labelButtonUPnP->AutoSize = true;
			this->labelButtonUPnP->Location = System::Drawing::Point(9, 91);
			this->labelButtonUPnP->Name = L"labelButtonUPnP";
			this->labelButtonUPnP->Size = System::Drawing::Size(70, 13);
			this->labelButtonUPnP->TabIndex = 8;
			this->labelButtonUPnP->Text = L"UPnP toggle:";
			this->toolTipStartupForm->SetToolTip(this->labelButtonUPnP, L"Defines if you want to use UPnP mode.");
			this->labelButtonUPnP->MouseLeave += gcnew System::EventHandler(this, &StartupForm::label_MouseLeave);
			this->labelButtonUPnP->MouseEnter += gcnew System::EventHandler(this, &StartupForm::label_MouseEnter);
			// 
			// buttonClosePortUPnP
			// 
			this->buttonClosePortUPnP->Location = System::Drawing::Point(143, 84);
			this->buttonClosePortUPnP->Name = L"buttonClosePortUPnP";
			this->buttonClosePortUPnP->Size = System::Drawing::Size(52, 26);
			this->buttonClosePortUPnP->TabIndex = 10;
			this->buttonClosePortUPnP->Text = L"��";
			this->toolTipStartupForm->SetToolTip(this->buttonClosePortUPnP, L"�Ҏ�|�[�g�ԍ���UDP�|�[�g������܂��B\r\nLilithPort�ŊJ�������|�[�g�̂ݕ��ł���" 
				L"���B");
			this->buttonClosePortUPnP->UseVisualStyleBackColor = true;
			this->buttonClosePortUPnP->Click += gcnew System::EventHandler(this, &StartupForm::buttonClosePortUPnP_Click);
			// 
			// buttonOpenPortUPnP
			// 
			this->buttonOpenPortUPnP->Location = System::Drawing::Point(85, 84);
			this->buttonOpenPortUPnP->Name = L"buttonOpenPortUPnP";
			this->buttonOpenPortUPnP->Size = System::Drawing::Size(52, 26);
			this->buttonOpenPortUPnP->TabIndex = 9;
			this->buttonOpenPortUPnP->Text = L"�J��";
			this->toolTipStartupForm->SetToolTip(this->buttonOpenPortUPnP, L"�Ҏ�|�[�g�ԍ���UDP�|�[�g���J�����܂��B\r\n�g�p��͎蓮�ł̃|�[�g���𐄏����܂��B" 
				L"");
			this->buttonOpenPortUPnP->UseVisualStyleBackColor = true;
			this->buttonOpenPortUPnP->Click += gcnew System::EventHandler(this, &StartupForm::buttonOpenPortUPnP_Click);
			// 
			// textBoxServerName
			// 
			this->textBoxServerName->Location = System::Drawing::Point(85, 24);
			this->textBoxServerName->Name = L"textBoxServerName";
			this->textBoxServerName->Size = System::Drawing::Size(252, 20);
			this->textBoxServerName->TabIndex = 7;
			this->textBoxServerName->WordWrap = false;
			// 
			// groupBoxConnection
			// 
			this->groupBoxConnection->Controls->Add(this->label1);
			this->groupBoxConnection->Controls->Add(this->textBoxIP);
			this->groupBoxConnection->Location = System::Drawing::Point(68, 136);
			this->groupBoxConnection->Name = L"groupBoxConnection";
			this->groupBoxConnection->Size = System::Drawing::Size(343, 51);
			this->groupBoxConnection->TabIndex = 9;
			this->groupBoxConnection->TabStop = false;
			this->groupBoxConnection->Text = L"Client";
			this->toolTipStartupForm->SetToolTip(this->groupBoxConnection, L"IP�A�h���X, �z�X�g��, �ϊ��A�h���X�̂����ꂩ����͂��܂��B\r\n�w:�x�Ɍq���ă|�[�g��" 
				L"�����w�肵�܂��B\r\n�|�[�g�ԍ����ȗ������ꍇ�A7500�|�[�g�ɐڑ����܂��B\r\n(��:[ lili" 
				L"th.port.xx:7500 ], [ 123.456.xxx.xxx ])");
			// 
			// groupBoxWelcome
			// 
			this->groupBoxWelcome->Controls->Add(this->textBoxWelcome);
			this->groupBoxWelcome->Location = System::Drawing::Point(6, 52);
			this->groupBoxWelcome->Name = L"groupBoxWelcome";
			this->groupBoxWelcome->Size = System::Drawing::Size(409, 177);
			this->groupBoxWelcome->TabIndex = 11;
			this->groupBoxWelcome->TabStop = false;
			this->groupBoxWelcome->Text = L"�T�[�o���m";
			this->toolTipStartupForm->SetToolTip(this->groupBoxWelcome, L"This Welcome message will be shown when players join your server.");
			// 
			// textBoxWelcome
			// 
			this->textBoxWelcome->Location = System::Drawing::Point(99, 55);
			this->textBoxWelcome->MaxLength = 254;
			this->textBoxWelcome->Name = L"textBoxWelcome";
			this->textBoxWelcome->Size = System::Drawing::Size(167, 93);
			this->textBoxWelcome->TabIndex = 3;
			this->textBoxWelcome->Text = L"";
			this->textBoxWelcome->WordWrap = false;
			// 
			// toolTipStartupForm
			// 
			this->toolTipStartupForm->AutomaticDelay = 0;
			this->toolTipStartupForm->AutoPopDelay = 30000;
			this->toolTipStartupForm->InitialDelay = 0;
			this->toolTipStartupForm->ReshowDelay = 0;
			this->toolTipStartupForm->UseAnimation = false;
			this->toolTipStartupForm->UseFading = false;
			// 
			// buttonConnect
			// 
			this->buttonConnect->Location = System::Drawing::Point(9, 279);
			this->buttonConnect->Name = L"buttonConnect";
			this->buttonConnect->Size = System::Drawing::Size(80, 26);
			this->buttonConnect->TabIndex = 13;
			this->buttonConnect->Text = L"Connect";
			this->toolTipStartupForm->SetToolTip(this->buttonConnect, L"�I�����C���ڑ����J�n���܂��B");
			this->buttonConnect->UseVisualStyleBackColor = true;
			this->buttonConnect->Click += gcnew System::EventHandler(this, &StartupForm::buttonOK_Click);
			// 
			// labelComment
			// 
			this->labelComment->AutoSize = true;
			this->labelComment->Location = System::Drawing::Point(14, 41);
			this->labelComment->Name = L"labelComment";
			this->labelComment->Size = System::Drawing::Size(63, 13);
			this->labelComment->TabIndex = 8;
			this->labelComment->Text = L"Description:";
			this->toolTipStartupForm->SetToolTip(this->labelComment, L"You will automatically say this message when you join.");
			// 
			// labelName
			// 
			this->labelName->AutoSize = true;
			this->labelName->Location = System::Drawing::Point(13, 14);
			this->labelName->Name = L"labelName";
			this->labelName->Size = System::Drawing::Size(38, 13);
			this->labelName->TabIndex = 5;
			this->labelName->Text = L"Name:";
			this->toolTipStartupForm->SetToolTip(this->labelName, L"Your username. Don\'t use special characters.");
			// 
			// startupTabs
			// 
			this->startupTabs->Controls->Add(this->tabPage2);
			this->startupTabs->Controls->Add(this->tabPage1);
			this->startupTabs->Controls->Add(this->tabPage3);
			this->startupTabs->Location = System::Drawing::Point(9, 12);
			this->startupTabs->Name = L"startupTabs";
			this->startupTabs->SelectedIndex = 0;
			this->startupTabs->Size = System::Drawing::Size(429, 261);
			this->startupTabs->TabIndex = 15;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->labelComment);
			this->tabPage1->Controls->Add(this->labelName);
			this->tabPage1->Controls->Add(this->textBoxComment);
			this->tabPage1->Controls->Add(this->textBoxName);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(421, 235);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"User Profile";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// textBoxComment
			// 
			this->textBoxComment->Location = System::Drawing::Point(87, 38);
			this->textBoxComment->Name = L"textBoxComment";
			this->textBoxComment->Size = System::Drawing::Size(94, 20);
			this->textBoxComment->TabIndex = 7;
			this->textBoxComment->WordWrap = false;
			// 
			// textBoxName
			// 
			this->textBoxName->Location = System::Drawing::Point(87, 11);
			this->textBoxName->Name = L"textBoxName";
			this->textBoxName->Size = System::Drawing::Size(94, 20);
			this->textBoxName->TabIndex = 6;
			this->textBoxName->WordWrap = false;
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->groupBoxStartMode);
			this->tabPage2->Controls->Add(this->radioButtonServer);
			this->tabPage2->Controls->Add(this->groupBoxConnection);
			this->tabPage2->Controls->Add(this->radioButtonHost);
			this->tabPage2->Controls->Add(this->radioButtonClient);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(421, 235);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Connection";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->groupBoxWelcome);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(421, 235);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Welcome Message";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 28);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(69, 13);
			this->label1->TabIndex = 11;
			this->label1->Text = L"Address / IP:";
			this->toolTipStartupForm->SetToolTip(this->label1, L"Defines if you want to use UPnP mode.");
			// 
			// StartupForm
			// 
			this->AcceptButton = this->buttonOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::Black;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(450, 317);
			this->ControlBox = false;
			this->Controls->Add(this->startupTabs);
			this->Controls->Add(this->buttonConnect);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->buttonCancel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"StartupForm";
			this->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Welcome";
			this->TopMost = true;
			this->Load += gcnew System::EventHandler(this, &StartupForm::StartupForm_Load);
			this->Shown += gcnew System::EventHandler(this, &StartupForm::StartupForm_Shown);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &StartupForm::StartupForm_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownOpenPort))->EndInit();
			this->groupBoxStartMode->ResumeLayout(false);
			this->groupBoxStartMode->PerformLayout();
			this->groupBoxConnection->ResumeLayout(false);
			this->groupBoxConnection->PerformLayout();
			this->groupBoxWelcome->ResumeLayout(false);
			this->startupTabs->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	protected:

		// StartupForm.cpp
		bool StartupForm::CheckValidate();

	private:

		// �R���g���[���{�b�N�X����̕���F���p
		static bool ConnectionStart = false;

		System::Void StartupForm_Load(System::Object^  sender, System::EventArgs^  e) {
			ConnectionStart = false;

			textBoxServerName->MaxLength      = MAX_NAME;
			textBoxIP->MaxLength              = MAX_ARRAY;
			textBoxName->MaxLength            = MAX_NAME;
			textBoxComment->MaxLength         = MAX_NAME;

			switch(MTOPTION.CONNECTION_TYPE){
			case CT_SERVER:
			default:
				radioButtonServer->Checked = true;

				break;
			case CT_HOST:
				radioButtonHost->Checked = true;

				break;
			case CT_CLIENT:
				radioButtonClient->Checked = true;

				numericUpDownOpenPort->Enabled = false;
				break;
			}

			textBoxServerName->Text = gcnew String(MTOPTION.SERVER_NAME);
			textBoxIP->Text = gcnew String(MTOPTION.CONNECTION_IP);
			
			numericUpDownOpenPort->Value = MTOPTION.OPEN_PORT;
			textBoxName->Text = gcnew String(MTOPTION.NAME);

			textBoxComment->Text = gcnew String(MTOPTION.COMMENT);
			// �^�u�����s��
			ReplaceWelcomeTab(true);
			textBoxWelcome->Text = gcnew String(MTOPTION.WELCOME);

		}
		System::Void StartupForm_Shown(System::Object^  sender, System::EventArgs^  e) {
			if(radioButtonServer->Checked){
				buttonOK->Focus();
			}else{
				buttonConnect->Focus();
			}
		}
		System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e) {
			// �N���E�ڑ��{�^��
			if(!CheckValidate()){
				return;
			}
			ConnectionStart = true;

			this->Close();
		}
		System::Void buttonCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			// ����{�^��
			MTOPTION.CONNECTION_TYPE = CT_FREE;
			ConnectionStart = false;

			this->Close();
		}
		System::Void StartupForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			if(!ConnectionStart){
				MTOPTION.CONNECTION_TYPE = CT_FREE;
			}
		}
		System::Void radioButtonServer_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			numericUpDownOpenPort->Enabled = true;
			labelOpenPort->Enabled         = true;
			labelServerName->Enabled       = true;
			textBoxServerName->Enabled     = true;
			textBoxIP->Enabled             = false;
			groupBoxConnection->Enabled    = false;
			groupBoxWelcome->Enabled       = true;

			labelButtonUPnP->Enabled       = true;
			buttonOpenPortUPnP->Enabled    = true;
			buttonClosePortUPnP->Enabled   = true;

			buttonOK->Enabled = true;
			buttonConnect->Enabled = false;
		}

		System::Void radioButtonHost_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			numericUpDownOpenPort->Enabled = true;
			labelOpenPort->Enabled         = true;
			labelServerName->Enabled       = false;
			textBoxServerName->Enabled     = false;
			textBoxIP->Enabled             = true;
			groupBoxConnection->Enabled    = true;
			groupBoxWelcome->Enabled       = false;

			labelButtonUPnP->Enabled       = true;
			buttonOpenPortUPnP->Enabled    = true;
			buttonClosePortUPnP->Enabled   = true;

			buttonOK->Enabled = false;
			buttonConnect->Enabled = true;
		}
		System::Void radioButtonClient_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			numericUpDownOpenPort->Enabled = false;
			labelOpenPort->Enabled         = false;
			labelServerName->Enabled       = false;
			textBoxServerName->Enabled     = false;
			textBoxIP->Enabled             = true;
			groupBoxConnection->Enabled    = true;
			groupBoxWelcome->Enabled       = false;

			labelButtonUPnP->Enabled       = false;
			buttonOpenPortUPnP->Enabled    = false;
			buttonClosePortUPnP->Enabled   = false;

			buttonOK->Enabled = false;
			buttonConnect->Enabled = true;
		}
		System::Void label_MouseEnter(System::Object^  sender, System::EventArgs^  e) {
			// �}�E�X�I�[�o�[�Ń��x���ɉ���
			Label^ obj = (Label^)sender;
			obj->Font = gcnew System::Drawing::Font(this->Font, FontStyle::Underline);
		}
		System::Void label_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
			Label^ obj = (Label^)sender;
			obj->Font = gcnew System::Drawing::Font(this->Font, FontStyle::Regular);
		}
		System::Void buttonOpenPortUPnP_Click(System::Object^  sender, System::EventArgs^  e) {
			// �|�[�g�J���{�^��
			buttonOpenPortUPnP->Enabled = false;
			buttonClosePortUPnP->Enabled = false;

			MTOPTION.OPEN_PORT = (UINT)numericUpDownOpenPort->Value;
			UPnP_PortOpenClose(true, false);

			buttonOpenPortUPnP->Enabled = true;
			buttonClosePortUPnP->Enabled = true;
		}
		System::Void buttonClosePortUPnP_Click(System::Object^  sender, System::EventArgs^  e) {
			// �|�[�g���{�^��
			buttonOpenPortUPnP->Enabled = false;
			buttonClosePortUPnP->Enabled = false;

			MTOPTION.OPEN_PORT = (UINT)numericUpDownOpenPort->Value;
			UPnP_PortOpenClose(false, false);

			buttonOpenPortUPnP->Enabled = true;
			buttonClosePortUPnP->Enabled = true;
		}
	private: System::Void textBoxComment_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 }
};
}
