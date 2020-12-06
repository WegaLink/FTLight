#pragma once

#pragma unmanaged 

#include "AppCm.h"

#pragma managed 


namespace AppCm {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;

	// delegate for AppCm callbacks
	public delegate bool FuncUpdateGUI(AppCmProfile& Profile, AppCmDynamic& Var);

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class AppCmUI : public System::Windows::Forms::Form
	{
	public:
		AppCmUI()
		{
			InitializeComponent();

			// Drawing
			ContextLocal = gcnew BufferedGraphicsContext();
			ContextRemote = gcnew BufferedGraphicsContext();
			ContextTouch = gcnew BufferedGraphicsContext();
			BufferLocal = ContextLocal->Allocate(pictureBoxLocal->CreateGraphics(), pictureBoxLocal->DisplayRectangle);
			BufferRemote = ContextRemote->Allocate(pictureBoxRemote->CreateGraphics(), pictureBoxRemote->DisplayRectangle);
			BufferTouch = ContextTouch->Allocate(pictureBoxTouch->CreateGraphics(), pictureBoxTouch->DisplayRectangle);
			GraphicsPen = gcnew System::Drawing::Pen(System::Drawing::Color::White, 1);
			GraphicsBrush = gcnew System::Drawing::SolidBrush(System::Drawing::Color::White);
			GraphicsFont = gcnew System::Drawing::Font("Arial", 12);
			GraphicsFormat = gcnew System::Drawing::StringFormat();

			// generate managed callbacks for unmanaged code

			// FuncDrawTextOnScreen
			FuncUpdateGUI^ fpUpdateGUI = gcnew FuncUpdateGUI(this, &AppCmUI::AppCmUI_UpdateGUI);
			gchUpdateGUI = GCHandle::Alloc(fpUpdateGUI);
			IntPtr ipUpdateGUI = Marshal::GetFunctionPointerForDelegate(fpUpdateGUI);
			cbUpdateGUI = static_cast<UpdateGUI>(ipUpdateGUI.ToPointer());

			// initialize program functionality
			Viewer = new SERVICE_AppCm();

			// read program settings from disk
			Viewer->clearLogLevel();
			if (false == Viewer->readProfile(getConfigPath(PROGRAM_AppCm))){
				// silently use a default profile
				Viewer->setDefaultProfile();
#ifdef _DEBUG
				CmString Msg = "Configuration has changed:\n";
				Viewer->getDynamic().Message != "message" ? Msg += Viewer->getDynamic().Message.getText() : 0;
				Msg += "\nPlease close application to update configuration.";
				MessageBoxA(NULL, Msg.getText(), Viewer->getDynamic().Context.getText(), MB_OK);
				Viewer->clearLogLevel();
				CmString ConfigPath = getConfigPath(PROGRAM_AppCm);
				if (ConfigPath.isEmpty()){
					MessageBoxA(NULL, "Config path could not be opened.", "AppCm", MB_OK);
				}
				Viewer->processMessageGUI("Close", getConfigPath(PROGRAM_AppCm), 0);
				return;
#endif
			}

			// application version
			Text = PROGRAM_AppCm + " " + VERSION_AppCm + " - Cosmos Application Template ";

			// Establish UpdateGUI callback connections
			Viewer->Provider().registerUpdateGUICallback(cbUpdateGUI);

			// update window location and size
			int32 ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			int32 ScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
			CmPoint Position;
			CmSize Size;
			CmSize MinSize(MinimumSize.Width, MinimumSize.Height);
			CmSize ScreenSize(ScreenWidth, ScreenHeight);
			Viewer->getWindow(Position, Size, MinSize, ScreenSize);
			Top = Position.y();
			Left = Position.x();
			Width = Size.x();
			Height = Size.y();

			// save resulting canvas sizes
			Viewer->setCanvas(CmSize(pictureBoxLocal->Width, pictureBoxLocal->Height), CmSize(pictureBoxTouch->Width, pictureBoxTouch->Height), CmSize(pictureBoxRemote->Width, pictureBoxRemote->Height));

			// initialize GUI but prevent GUI messages during update
			isShutDown = false;
			isUpdateGUI = false;
			isUpdateControls = true;
			Viewer->updateGUI();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AppCmUI()
		{
			if (components)
			{
				delete components;
			}

			// delete Viewer
			if (NULL != Viewer) {
				// unregister UpdateGUI callback
				Viewer->Provider().unregisterUpdateGUICallback(cbUpdateGUI);

				delete Viewer;
			}

			// Drawing
			delete ContextLocal;
			delete ContextRemote;
			delete ContextTouch;
			delete BufferLocal;
			delete BufferRemote;
			delete BufferTouch;
			delete GraphicsPen;
			delete GraphicsFont;
			delete GraphicsBrush;

			// release callback function pointer
			gchUpdateGUI.Free();
		}
	private:
		
		// access to program functionality
		SERVICE_AppCm *Viewer = NULL;

		// UpdateGUI callback
		static GCHandle gchUpdateGUI;
		UpdateGUI cbUpdateGUI = NULL;
		int32 UpdateCount = 1;
		bool isUpdateGUI = false;
		bool isUpdateControls = false;
		bool isShutDown = false;

	private:
		// Drawing 
		BufferedGraphicsContext^ ContextLocal;
		BufferedGraphicsContext^ ContextRemote;
		BufferedGraphicsContext^ ContextTouch;
		BufferedGraphics^ BufferLocal;
		BufferedGraphics^ BufferRemote;
		BufferedGraphics^ BufferTouch;
		System::Drawing::Pen^ GraphicsPen;
		System::Drawing::Font^ GraphicsFont;
		System::Drawing::SolidBrush^ GraphicsBrush;
		System::Drawing::StringFormat^ GraphicsFormat = gcnew System::Drawing::StringFormat();
		double GraphicsRotation = 0;
private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

	private: System::Windows::Forms::PictureBox^  pictureBoxLocal;
	private: System::Windows::Forms::PictureBox^  pictureBoxRemote;
	private: System::Windows::Forms::PictureBox^  pictureBoxTouch;
	private: System::Windows::Forms::Label^  labelLocal;
	private: System::Windows::Forms::Label^  labelTouch;
	private: System::Windows::Forms::Label^  labelRemote;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutRemote;
	private: System::Windows::Forms::Button^  button_Close;
	private: System::Windows::Forms::ListBox^  listBoxTouchMode;
	private: System::Windows::Forms::ListBox^  listBoxRemoteDevice;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanelLocal;
	private: System::Windows::Forms::ListBox^  listBoxLocalDevice;
	private: System::Windows::Forms::Timer^  timerUpdate;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->pictureBoxRemote = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxTouch = (gcnew System::Windows::Forms::PictureBox());
			this->labelTouch = (gcnew System::Windows::Forms::Label());
			this->labelRemote = (gcnew System::Windows::Forms::Label());
			this->tableLayoutRemote = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->button_Close = (gcnew System::Windows::Forms::Button());
			this->listBoxTouchMode = (gcnew System::Windows::Forms::ListBox());
			this->listBoxRemoteDevice = (gcnew System::Windows::Forms::ListBox());
			this->pictureBoxLocal = (gcnew System::Windows::Forms::PictureBox());
			this->labelLocal = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanelLocal = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->listBoxLocalDevice = (gcnew System::Windows::Forms::ListBox());
			this->timerUpdate = (gcnew System::Windows::Forms::Timer(this->components));
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxRemote))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxTouch))->BeginInit();
			this->tableLayoutRemote->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxLocal))->BeginInit();
			this->tableLayoutPanelLocal->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 6;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				1.1F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				24)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				25)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				25)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				24)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				0.9F)));
			this->tableLayoutPanel1->Controls->Add(this->pictureBoxRemote, 3, 1);
			this->tableLayoutPanel1->Controls->Add(this->pictureBoxTouch, 2, 3);
			this->tableLayoutPanel1->Controls->Add(this->labelTouch, 2, 2);
			this->tableLayoutPanel1->Controls->Add(this->labelRemote, 4, 2);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutRemote, 4, 3);
			this->tableLayoutPanel1->Controls->Add(this->pictureBoxLocal, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->labelLocal, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanelLocal, 1, 3);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 5;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 2)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 28)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 2)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(1036, 812);
			this->tableLayoutPanel1->TabIndex = 8;
			// 
			// pictureBoxRemote
			// 
			this->pictureBoxRemote->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBoxRemote->BackColor = System::Drawing::Color::LightSteelBlue;
			this->tableLayoutPanel1->SetColumnSpan(this->pictureBoxRemote, 3);
			this->pictureBoxRemote->Location = System::Drawing::Point(521, 5);
			this->pictureBoxRemote->Name = L"pictureBoxRemote";
			this->pictureBoxRemote->Size = System::Drawing::Size(512, 384);
			this->pictureBoxRemote->TabIndex = 4;
			this->pictureBoxRemote->TabStop = false;
			// 
			// pictureBoxTouch
			// 
			this->pictureBoxTouch->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBoxTouch->BackColor = System::Drawing::Color::Wheat;
			this->tableLayoutPanel1->SetColumnSpan(this->pictureBoxTouch, 2);
			this->pictureBoxTouch->Location = System::Drawing::Point(262, 423);
			this->pictureBoxTouch->Name = L"pictureBoxTouch";
			this->pictureBoxTouch->Size = System::Drawing::Size(512, 384);
			this->pictureBoxTouch->TabIndex = 5;
			this->pictureBoxTouch->TabStop = false;
			// 
			// labelTouch
			// 
			this->labelTouch->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->labelTouch->AutoSize = true;
			this->labelTouch->BackColor = System::Drawing::Color::Wheat;
			this->tableLayoutPanel1->SetColumnSpan(this->labelTouch, 2);
			this->labelTouch->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelTouch->Location = System::Drawing::Point(262, 392);
			this->labelTouch->Name = L"labelTouch";
			this->labelTouch->Size = System::Drawing::Size(512, 28);
			this->labelTouch->TabIndex = 7;
			this->labelTouch->Text = L"Touch";
			this->labelTouch->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// labelRemote
			// 
			this->labelRemote->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->labelRemote->AutoSize = true;
			this->labelRemote->BackColor = System::Drawing::Color::LightSteelBlue;
			this->tableLayoutPanel1->SetColumnSpan(this->labelRemote, 2);
			this->labelRemote->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelRemote->Location = System::Drawing::Point(780, 392);
			this->labelRemote->Name = L"labelRemote";
			this->labelRemote->Size = System::Drawing::Size(253, 28);
			this->labelRemote->TabIndex = 8;
			this->labelRemote->Text = L"Remote";
			// 
			// tableLayoutRemote
			// 
			this->tableLayoutRemote->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutRemote->ColumnCount = 2;
			this->tableLayoutRemote->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutRemote->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				100)));
			this->tableLayoutRemote->Controls->Add(this->button_Close, 1, 4);
			this->tableLayoutRemote->Controls->Add(this->listBoxTouchMode, 0, 3);
			this->tableLayoutRemote->Controls->Add(this->listBoxRemoteDevice, 0, 0);
			this->tableLayoutRemote->Location = System::Drawing::Point(780, 423);
			this->tableLayoutRemote->Name = L"tableLayoutRemote";
			this->tableLayoutRemote->RowCount = 5;
			this->tableLayoutRemote->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 14.46809F)));
			this->tableLayoutRemote->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 85.53191F)));
			this->tableLayoutRemote->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 87)));
			this->tableLayoutRemote->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 29)));
			this->tableLayoutRemote->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 32)));
			this->tableLayoutRemote->Size = System::Drawing::Size(242, 384);
			this->tableLayoutRemote->TabIndex = 10;
			// 
			// button_Close
			// 
			this->button_Close->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom));
			this->button_Close->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_Close->Location = System::Drawing::Point(154, 354);
			this->button_Close->Name = L"button_Close";
			this->button_Close->Size = System::Drawing::Size(75, 27);
			this->button_Close->TabIndex = 9;
			this->button_Close->Text = L"Close";
			this->button_Close->UseVisualStyleBackColor = true;
			this->button_Close->Click += gcnew System::EventHandler(this, &AppCmUI::button_Close_Click);
			// 
			// listBoxTouchMode
			// 
			this->listBoxTouchMode->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->listBoxTouchMode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->listBoxTouchMode->FormattingEnabled = true;
			this->listBoxTouchMode->ItemHeight = 18;
			this->listBoxTouchMode->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"TEST", L"Mode 1", L"Mode 2" });
			this->listBoxTouchMode->Location = System::Drawing::Point(3, 325);
			this->listBoxTouchMode->Name = L"listBoxTouchMode";
			this->listBoxTouchMode->Size = System::Drawing::Size(136, 22);
			this->listBoxTouchMode->TabIndex = 10;
			this->listBoxTouchMode->SelectedIndexChanged += gcnew System::EventHandler(this, &AppCmUI::listBoxTouchMode_SelectedIndexChanged);
			// 
			// listBoxRemoteDevice
			// 
			this->listBoxRemoteDevice->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutRemote->SetColumnSpan(this->listBoxRemoteDevice, 2);
			this->listBoxRemoteDevice->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->listBoxRemoteDevice->FormattingEnabled = true;
			this->listBoxRemoteDevice->ItemHeight = 20;
			this->listBoxRemoteDevice->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				L"OFF", L"Baseline", L"Sine", L"Square",
					L"Triangle", L"Sawtooth up", L"Sawtooth down"
			});
			this->listBoxRemoteDevice->Location = System::Drawing::Point(3, 3);
			this->listBoxRemoteDevice->Name = L"listBoxRemoteDevice";
			this->listBoxRemoteDevice->Size = System::Drawing::Size(236, 24);
			this->listBoxRemoteDevice->TabIndex = 11;
			this->listBoxRemoteDevice->SelectedIndexChanged += gcnew System::EventHandler(this, &AppCmUI::listBoxRemoteDevice_SelectedIndexChanged);
			// 
			// pictureBoxLocal
			// 
			this->pictureBoxLocal->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBoxLocal->BackColor = System::Drawing::Color::DarkSeaGreen;
			this->tableLayoutPanel1->SetColumnSpan(this->pictureBoxLocal, 3);
			this->pictureBoxLocal->Location = System::Drawing::Point(3, 5);
			this->pictureBoxLocal->Name = L"pictureBoxLocal";
			this->pictureBoxLocal->Size = System::Drawing::Size(512, 384);
			this->pictureBoxLocal->TabIndex = 3;
			this->pictureBoxLocal->TabStop = false;
			// 
			// labelLocal
			// 
			this->labelLocal->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->labelLocal->AutoSize = true;
			this->labelLocal->BackColor = System::Drawing::Color::DarkSeaGreen;
			this->tableLayoutPanel1->SetColumnSpan(this->labelLocal, 2);
			this->labelLocal->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelLocal->Location = System::Drawing::Point(3, 392);
			this->labelLocal->Name = L"labelLocal";
			this->labelLocal->Size = System::Drawing::Size(253, 28);
			this->labelLocal->TabIndex = 6;
			this->labelLocal->Text = L"Local";
			this->labelLocal->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// tableLayoutPanelLocal
			// 
			this->tableLayoutPanelLocal->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutPanelLocal->ColumnCount = 2;
			this->tableLayoutPanelLocal->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanelLocal->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanelLocal->Controls->Add(this->listBoxLocalDevice, 0, 0);
			this->tableLayoutPanelLocal->Location = System::Drawing::Point(14, 423);
			this->tableLayoutPanelLocal->Name = L"tableLayoutPanelLocal";
			this->tableLayoutPanelLocal->RowCount = 5;
			this->tableLayoutPanelLocal->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent,
				13.58025F)));
			this->tableLayoutPanelLocal->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent,
				86.41975F)));
			this->tableLayoutPanelLocal->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				77)));
			this->tableLayoutPanelLocal->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				29)));
			this->tableLayoutPanelLocal->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				34)));
			this->tableLayoutPanelLocal->Size = System::Drawing::Size(242, 384);
			this->tableLayoutPanelLocal->TabIndex = 11;
			// 
			// listBoxLocalDevice
			// 
			this->listBoxLocalDevice->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutPanelLocal->SetColumnSpan(this->listBoxLocalDevice, 2);
			this->listBoxLocalDevice->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->listBoxLocalDevice->FormattingEnabled = true;
			this->listBoxLocalDevice->ItemHeight = 20;
			this->listBoxLocalDevice->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				L"OFF", L"Baseline", L"Sine", L"Square",
					L"Triangle", L"Sawtooth up", L"Sawtooth down"
			});
			this->listBoxLocalDevice->Location = System::Drawing::Point(3, 3);
			this->listBoxLocalDevice->Name = L"listBoxLocalDevice";
			this->listBoxLocalDevice->Size = System::Drawing::Size(236, 24);
			this->listBoxLocalDevice->TabIndex = 0;
			this->listBoxLocalDevice->SelectedIndexChanged += gcnew System::EventHandler(this, &AppCmUI::listBoxLocalDevice_SelectedIndexChanged);
			// 
			// timerUpdate
			// 
			this->timerUpdate->Enabled = true;
			this->timerUpdate->Interval = 40;
			this->timerUpdate->Tick += gcnew System::EventHandler(this, &AppCmUI::timerUpdateGUI_Tick);
			// 
			// AppCmUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1036, 812);
			this->Controls->Add(this->tableLayoutPanel1);
			this->MinimumSize = System::Drawing::Size(1044, 840);
			this->Name = L"AppCmUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"AppCm - Information Processing Network (IPN)";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &AppCmUI::AppCmUI_FormClosing);
			this->LocationChanged += gcnew System::EventHandler(this, &AppCmUI::AppCmUI_LocationChanged);
			this->SizeChanged += gcnew System::EventHandler(this, &AppCmUI::AppCmUI_SizeChanged);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxRemote))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxTouch))->EndInit();
			this->tableLayoutRemote->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxLocal))->EndInit();
			this->tableLayoutPanelLocal->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	// GUI helper functions
	private: System::Drawing::Color TranslateColor(const CmString& _Color, double _Alpha){

		System::Drawing::Color NewColor = System::Drawing::Color::Black;

		// check for RGB values (in a fix "RRR,GGG,BBB" pattern)
		if (_Color.getLength() == 0)
			NewColor = System::Drawing::Color::Black;
		else if (((_Color.getLength() == 11) || (_Color.getLength() == 12)) && _Color[3] == ',' && _Color[7] == ','){
			int32 Red = (_Color[0] - '0') * 100 + (_Color[1] - '0') * 10 + _Color[2] - '0';
			int32 Green = (_Color[4] - '0') * 100 + (_Color[5] - '0') * 10 + _Color[6] - '0';
			int32 Blue = (_Color[8] - '0') * 100 + (_Color[9] - '0') * 10 + _Color[10] - '0';
			NewColor = Color::FromArgb(Red, Green, Blue);
		}
		// translate color
		else if (_Color == "Black")						NewColor = System::Drawing::Color::Black;
		else if (_Color == "White")						NewColor = System::Drawing::Color::White;
		else if (_Color == "Gray")						NewColor = System::Drawing::Color::Gray;
		else if (_Color == "LightGray")				NewColor = System::Drawing::Color::LightGray;
		else if (_Color == "DarkGray")				NewColor = System::Drawing::Color::DarkGray;
		else if (_Color == "DimGray")					NewColor = System::Drawing::Color::DimGray;
		else if (_Color == "DarkSlateGray")		NewColor = System::Drawing::Color::DarkSlateGray;
		else if (_Color == "Red")							NewColor = System::Drawing::Color::Red;
		else if (_Color == "DarkRed")					NewColor = System::Drawing::Color::DarkRed;
		else if (_Color == "Green")						NewColor = System::Drawing::Color::Green;
		else if (_Color == "LightGreen")			NewColor = System::Drawing::Color::LightGreen;
		else if (_Color == "DarkGreen")				NewColor = System::Drawing::Color::DarkGreen;
		else if (_Color == "DarkSeaGreen")		NewColor = System::Drawing::Color::DarkSeaGreen;
		else if (_Color == "GreenYellow")			NewColor = System::Drawing::Color::GreenYellow;
		else if (_Color == "Aquamarine")			NewColor = System::Drawing::Color::Aquamarine;
		else if (_Color == "Blue")						NewColor = System::Drawing::Color::Blue;
		else if (_Color == "LightBlue")				NewColor = System::Drawing::Color::LightBlue;
		else if (_Color == "DarkBlue")				NewColor = System::Drawing::Color::DarkBlue;
		else if (_Color == "CornflowerBlue")	NewColor = System::Drawing::Color::CornflowerBlue;
		else if (_Color == "Cyan")						NewColor = System::Drawing::Color::Cyan;
		else if (_Color == "LightCyan")				NewColor = System::Drawing::Color::LightCyan;
		else if (_Color == "DarkCyan")				NewColor = System::Drawing::Color::DarkCyan;
		else if (_Color == "Yellow")					NewColor = System::Drawing::Color::Yellow;
		else if (_Color == "YellowGreen")			NewColor = System::Drawing::Color::YellowGreen;
		else if (_Color == "Orange")					NewColor = System::Drawing::Color::Orange;
		else if (_Color == "DarkOrange")			NewColor = System::Drawing::Color::DarkOrange;
		else if (_Color == "Purple")					NewColor = System::Drawing::Color::Purple;
		else if (_Color == "Magenta")					NewColor = System::Drawing::Color::Magenta;
		else if (_Color == "DarkMagenta")			NewColor = System::Drawing::Color::DarkMagenta;
		else if (_Color == "Gold")						NewColor = System::Drawing::Color::Gold;
		else if (_Color == "Goldenrod")				NewColor = System::Drawing::Color::Goldenrod;
		else if (_Color == "DarkSlateGray")		NewColor = System::Drawing::Color::DarkSlateGray;
		else if (_Color == "LightCoral")			NewColor = System::Drawing::Color::LightCoral;
		else if (_Color == "LightPink")				NewColor = System::Drawing::Color::LightPink;
		else if (_Color == "Salmon")					NewColor = System::Drawing::Color::Salmon;
		else if (_Color == "LightSalmon")			NewColor = System::Drawing::Color::LightSalmon;
		else if (_Color == "LightSeaGreen")		NewColor = System::Drawing::Color::LightSeaGreen;
		else if (_Color == "LightSkyBlue")		NewColor = System::Drawing::Color::LightSkyBlue;
		else if (_Color == "DeepSkyBlue")			NewColor = System::Drawing::Color::DeepSkyBlue;
		else if (_Color == "LightSteelBlue")	NewColor = System::Drawing::Color::LightSteelBlue;
		else if (_Color == "LightYellow")			NewColor = System::Drawing::Color::LightYellow;
		else if (_Color == "Honeydew")				NewColor = System::Drawing::Color::Honeydew;
		else if (_Color == "Indigo")					NewColor = System::Drawing::Color::Indigo;
		else if (_Color == "Wheat")						NewColor = System::Drawing::Color::Wheat;

		//   take _Alpha into account
		if (_Alpha < 0 || _Alpha > 1.0){
			_Alpha = 1.0;
		}
		int Alpha = (int)(255 * _Alpha);
		NewColor = Color::FromArgb(Alpha, NewColor);

		return NewColor;
	}
	private: System::Void processValueChanged(CmString _Key, String^ _Text, Decimal _Value){
		// do not force controls be updated
		processValueChanged(_Key, _Text, _Value, false);
	}
	private: System::Void processValueChanged(CmString _Key, String^ _Text, Decimal _Value, bool _isUpdateControls)
	{
		char Text[256];
		sprintf_s(Text, sizeof(Text), "%s", _Text);
		double Value = _Value.ToDouble(_Value);
		// forward a changed GUI control to application
		_isUpdateControls ? isUpdateControls = true : 0;
		Viewer->processMessageGUI(_Key, Text, Value);
	}
	private: System::Void processCheckedChanged(CmString _Key, String^ _Text, bool _Checked)
	{
		processValueChanged(_Key, _Text, _Checked ? 1 : 0);
	}
	private: System::Void processKeyDown(CmString _Key, String^ _Text, int32 _Value, System::Windows::Forms::KeyEventArgs^ e)
	{
		// forward a GUI control to application when the Return key is pressed
		if (System::Windows::Forms::Keys::Return == e->KeyCode){
			processValueChanged(_Key, _Text, _Value);
		}
	}
	private: void setText(Control^ _Control, CmString& _String){
		String^ _Text = gcnew System::String(_String.getText());
		_Control->Text = _Text;
	}
	private: bool drawText(CmValueFTL& _TextMatrix, BufferedGraphics^ _Buffer, PictureBox^  _pictureBox){

		// check if drawing is enabled
		if (false == _TextMatrix.getDrawingEnabled()) return true;
		if (isShutDown) return false;

		// put text lines on screen
		CmPoint2D Position;
		CmString Text;
		double FontSize;
		double Rotation;
		CmString Font;
		double Alpha;
		CmString Color;
		int32 Height = _pictureBox->Height;
		int32 TextCount = _TextMatrix.getItemCount();
		for (int i = 0; i < TextCount; i++){
			// check if chart text is enabled
			if (TextCount == 0 || false == _TextMatrix.getItemEnabled(i)) continue;

			// get complete set of parameters
			if (false == _TextMatrix.getChartText(i, Position, Text, FontSize, Rotation, Color, Alpha, Font)) return false;

			// transform parameters
			String^ GraphicsText = gcnew System::String(Text.getText());
			GraphicsFont = gcnew System::Drawing::Font(gcnew System::String(Font.getText()), (float)FontSize);
			GraphicsBrush = gcnew	System::Drawing::SolidBrush(TranslateColor(Color, (float)Alpha));

			// put text on screen
			// ToDo: reset previous rotation tranform
			Rotation -= GraphicsRotation;
			GraphicsRotation += Rotation;
			_Buffer->Graphics->RotateTransform((float)Rotation);
			_Buffer->Graphics->DrawString(GraphicsText, GraphicsFont, GraphicsBrush, Position.x(), Height - Position.y(), GraphicsFormat);
		}

		return true;
	}
	private: bool drawPolygon(CmValueFTL& _LinesMatrix, BufferedGraphics^ _Buffer, PictureBox^  _pictureBox){

	// check if drawing is enabled
	if (false == _LinesMatrix.getDrawingEnabled()) return true;
	if (isShutDown) return false;

	// draw lines on screen
	CmVector<float> PointX;
	CmVector<float> PointY;
	double LineWidth;
	CmString LineColor;
	CmString FillColor;
	double LineAlpha;
	double FillAlpha;
	int32 Height = _pictureBox->Height;
	int32 LineCount = _LinesMatrix.getItemCount();
	for (int l = 0; l < LineCount; l++){
		// get line (polygon)
		_LinesMatrix.getChartLine(l, PointX, PointY, LineWidth, LineColor, FillColor, LineAlpha, FillAlpha);
		// adjust style
		GraphicsPen->Width = (float)LineWidth;
		GraphicsPen->Color = TranslateColor(LineColor, (float)LineAlpha);
		GraphicsBrush->Color = TranslateColor(FillColor, (float)FillAlpha);
		// retrieve points
		uint32 TopPB = _pictureBox->Top;
		uint32 LeftPB = _pictureBox->Left;
		uint32 WidthPB = _pictureBox->Width;
		uint32 HeightPB = _pictureBox->Height;
		int32 PointCount = PointX.getLength();
		uint32 X;
		uint32 Y;
		array<Point>^ Points = gcnew array<Point>(PointCount);
		for (int i = 0; i < PointCount; i++){
			X = (uint32)floor(PointX[i]);
			Y = (uint32)(Height - PointY[i]);
			X < LeftPB ? LeftPB : X >= WidthPB ? WidthPB - 1 : X;
			Y < TopPB ? TopPB : Y >= HeightPB ? HeightPB - 1 : Y;
			Points[i] = Point(X, Y);
		}

		// draw a filled polygon on screen
		FillAlpha > 0 ? _Buffer->Graphics->FillPolygon(GraphicsBrush, Points) : 0;
		FillAlpha > 0 ? _Buffer->Graphics->DrawPolygon(GraphicsPen, Points) : _Buffer->Graphics->DrawCurve(GraphicsPen, Points, 0.0);
	}

	return true;
}
	private: bool drawChart(ChartDynamic& _dyn, BufferedGraphics^ _Buffer, System::Windows::Forms::PictureBox^ _pictureBox){
	// Show on Local canvas
	if (_dyn.Init.getDrawingEnabled()){
		// clear background/alpha
		CmString BackgroundColor;
		double BackgroundAlpha;
		if (false == _dyn.Init.getChartBackground(BackgroundColor, BackgroundAlpha)) return false;
		_Buffer->Graphics->Clear(TranslateColor(BackgroundColor, BackgroundAlpha));
		// display text
		if (false == drawText(_dyn.Text, _Buffer, _pictureBox)) return false;
		// draw grid
		if (false == drawPolygon(_dyn.LayoutGrid, _Buffer, _pictureBox)) return false;
		// display labels
		if (false == drawText(_dyn.LayoutLabels, _Buffer, _pictureBox)) return false;
		// draw signal run
		if (false == drawPolygon(_dyn.SignalPoints, _Buffer, _pictureBox)) return false;
		// display signal tags
		if (false == drawText(_dyn.SignalTags, _Buffer, _pictureBox)) return false;

		// show buffered drawings
		_Buffer->Render();
	}
	return true;
}
	private: CmString getConfigPath(char* Program){
		// path to AppData
		String^ AppData = System::Environment::GetFolderPath(System::Environment::SpecialFolder::CommonApplicationData);
		char AppDataPath[256];
		sprintf_s(AppDataPath, sizeof(AppDataPath), "%s", AppData);
		// get config path
		CmStringFTL ConfigPath;
		if (false == ConfigPath.getConfigPath(AppDataPath, COMPANY_AppCm, Program, VERSION_AppCm)){
			ConfigPath = "";
		}
		return ConfigPath;
	}

	// GUI default messages
	private: System::Void timerUpdateGUI_Tick(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		processValueChanged("Update", "", UpdateCount++);
	}
	// location/size change
	private: System::Void AppCmUI_LocationChanged(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		Viewer->setWindow(CmPoint(Left, Top), CmSize(Width, Height));
	}
	private: System::Void AppCmUI_SizeChanged(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		Viewer->setWindow(CmPoint(Left, Top), CmSize(Width, Height));
		// save also resulting canvas sizes
		Viewer->setCanvas(CmSize(pictureBoxLocal->Width, pictureBoxLocal->Height), CmSize(pictureBoxRemote->Width, pictureBoxRemote->Height), CmSize(pictureBoxTouch->Width, pictureBoxTouch->Height));

		// adjust picture boxes
		delete BufferLocal;
		BufferLocal = ContextLocal->Allocate(pictureBoxLocal->CreateGraphics(), pictureBoxLocal->DisplayRectangle);
		delete BufferRemote;
		BufferRemote = ContextRemote->Allocate(pictureBoxRemote->CreateGraphics(), pictureBoxRemote->DisplayRectangle);
		delete BufferTouch;
		BufferTouch = ContextTouch->Allocate(pictureBoxTouch->CreateGraphics(), pictureBoxTouch->DisplayRectangle);

		// update drawings
		Viewer->processMessageGUI("Draw", "", 0);
	}
	// close application/save settings
	private: System::Void button_Close_Click(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		Close();
	}
	private: System::Void AppCmUI_FormClosing(System::Object^  /*sender*/, System::Windows::Forms::FormClosingEventArgs^  /*e*/) {
		isShutDown = true;
		if (false == Viewer->processMessageGUI("Close", getConfigPath(PROGRAM_AppCm), 0)){
			MessageBoxA(NULL, Viewer->getDynamic().Message.getText(), Viewer->getDynamic().Context.getText(), MB_OK);
		}
	}
	private: System::Void button_Save_Click(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		if (false == Viewer->processMessageGUI("Save", getConfigPath(PROGRAM_AppCm), 0)){
			MessageBoxA(NULL, Viewer->getDynamic().Message.getText(), Viewer->getDynamic().Context.getText(), MB_OK);
		}
	}

	// GUI update
	private: bool AppCmUI_UpdateGUI(AppCmProfile& _pro, AppCmDynamic& /*_dyn*/) {

		// check for shutdown
		if (isShutDown || isUpdateGUI) return false;

		try{
			// switch off further updates while GUI update is running
			isUpdateGUI = true;

			// draw charts
			Viewer->enterSerialize();
			drawChart(_pro.LocalChart.dyn(), BufferLocal, pictureBoxLocal);
			drawChart(_pro.RemoteChart.dyn(), BufferRemote, pictureBoxRemote);
			drawChart(_pro.TouchChart.dyn(), BufferTouch, pictureBoxTouch);
			Viewer->leaveSerialize();

			// update controls
			if (isUpdateControls){
				isUpdateControls = false;

				// device/mode selection
				setText(listBoxLocalDevice, _pro.LocalDevice);
				setText(listBoxRemoteDevice, _pro.RemoteDevice);
				setText(listBoxTouchMode, _pro.TouchMode);

			}


		}
		catch (...){
			MessageBoxA(NULL, "GUI update failed. Program needs maintenance.", _pro.UURI, MB_OK);
		}

		// enable GUI update
		isUpdateGUI = false;

		return true;
	};

	//
	// user actions
	//
	#define UPDATE_CONTROLS true
	// local
	private: System::Void listBoxLocalDevice_SelectedIndexChanged(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		processValueChanged("LocalDevice", listBoxLocalDevice->Text, listBoxLocalDevice->SelectedIndex, UPDATE_CONTROLS);
	}
  // remote
	private: System::Void listBoxRemoteDevice_SelectedIndexChanged(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		processValueChanged("RemoteDevice", listBoxRemoteDevice->Text, listBoxRemoteDevice->SelectedIndex, UPDATE_CONTROLS);
	}
  // touch
	private: System::Void listBoxTouchMode_SelectedIndexChanged(System::Object^  /*sender*/, System::EventArgs^  /*e*/) {
		processValueChanged("TouchMode", listBoxTouchMode->Text, listBoxTouchMode->SelectedIndex, UPDATE_CONTROLS);
	}
};

}


