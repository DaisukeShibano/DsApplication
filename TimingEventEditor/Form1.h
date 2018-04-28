#pragma once
#ifndef _DS_SIMU_H_
#include "Simu/DsSimu.h"
#endif
#ifndef _DS_SIMU_UTIL_
#include "DsSimuUtlil.h"
#endif

#include "TagData.h"
#include "Setting.h"


namespace TimingEventEditor {

	using namespace System;
	using namespace System::Windows;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Zusammenfassung f�E Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			DsSimu::Initialize();
			//
			//TODO: Konstruktorcode hier hinzuf�Een.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			DsSimu::Finalize();
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Panel^  panel1;


	private: System::Windows::Forms::TrackBar^  trackBar1;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;


	private: System::Windows::Forms::ToolStripMenuItem^  AddToolStripMenuItem;
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column2;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  �t�@�C��ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::TextBox^  animPathTextBox;
	private: System::Windows::Forms::Button^  animPathOpenButton;


	protected:


	private: System::Windows::Forms::Panel^  panel3;
	private: System::Windows::Forms::Button^  RestartButton;
	private: System::Windows::Forms::Button^  ResetButton;
	private: System::Windows::Forms::Button^  PlayButton;
	private: System::Windows::Forms::ImageList^  imageList1;


#pragma region Private Menber

	private:
		int m_actionNum = 0;
		String^ m_animPath = gcnew String("");
	private: System::Windows::Forms::ListBox^  AnimListBox;

			 bool m_isPlayAnim = false;

#pragma endregion

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode f�E die Designerunterst�Ezung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor ge�ndert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->AddToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->�t�@�C��ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->AnimListBox = (gcnew System::Windows::Forms::ListBox());
			this->animPathOpenButton = (gcnew System::Windows::Forms::Button());
			this->animPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->RestartButton = (gcnew System::Windows::Forms::Button());
			this->ResetButton = (gcnew System::Windows::Forms::Button());
			this->PlayButton = (gcnew System::Windows::Forms::Button());
			this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->panel1->SuspendLayout();
			this->contextMenuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->menuStrip1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox1->Location = System::Drawing::Point(12, 29);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(377, 283);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::pictureBox1_Paint);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseClick);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseMove);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseUp);
			this->pictureBox1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseWheel);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 16;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel1->AutoScroll = true;
			this->panel1->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->ContextMenuStrip = this->contextMenuStrip1;
			this->panel1->Controls->Add(this->trackBar1);
			this->panel1->Location = System::Drawing::Point(248, 318);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(672, 187);
			this->panel1->TabIndex = 1;
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->AddToolStripMenuItem });
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(101, 26);
			// 
			// AddToolStripMenuItem
			// 
			this->AddToolStripMenuItem->Name = L"AddToolStripMenuItem";
			this->AddToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->AddToolStripMenuItem->Text = L"�ǉ�";
			// 
			// trackBar1
			// 
			this->trackBar1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->trackBar1->Location = System::Drawing::Point(-13, 0);
			this->trackBar1->Maximum = 100;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(805, 45);
			this->trackBar1->TabIndex = 2;
			this->trackBar1->ValueChanged += gcnew System::EventHandler(this, &Form1::trackBar1_ValueChanged);
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->dataGridView1->BackgroundColor = System::Drawing::SystemColors::ButtonFace;
			this->dataGridView1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->dataGridView1->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
				this->Column1,
					this->Column2
			});
			this->dataGridView1->GridColor = System::Drawing::SystemColors::ActiveCaption;
			this->dataGridView1->Location = System::Drawing::Point(12, 318);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowHeadersVisible = false;
			this->dataGridView1->RowTemplate->Height = 21;
			this->dataGridView1->Size = System::Drawing::Size(230, 187);
			this->dataGridView1->TabIndex = 2;
			this->dataGridView1->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView1_CellEndEdit);
			// 
			// Column1
			// 
			this->Column1->FillWeight = 130;
			this->Column1->Frozen = true;
			this->Column1->HeaderText = L"Name";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			this->Column1->Width = 130;
			// 
			// Column2
			// 
			this->Column2->Frozen = true;
			this->Column2->HeaderText = L"Value";
			this->Column2->Name = L"Column2";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->�t�@�C��ToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(932, 26);
			this->menuStrip1->TabIndex = 3;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// �t�@�C��ToolStripMenuItem
			// 
			this->�t�@�C��ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->openToolStripMenuItem });
			this->�t�@�C��ToolStripMenuItem->Name = L"�t�@�C��ToolStripMenuItem";
			this->�t�@�C��ToolStripMenuItem->Size = System::Drawing::Size(68, 22);
			this->�t�@�C��ToolStripMenuItem->Text = L"�t�@�C��";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->openToolStripMenuItem->Text = L"�J��";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			// 
			// panel2
			// 
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel2->Controls->Add(this->AnimListBox);
			this->panel2->Controls->Add(this->animPathOpenButton);
			this->panel2->Controls->Add(this->animPathTextBox);
			this->panel2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->panel2->Location = System::Drawing::Point(395, 29);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(525, 251);
			this->panel2->TabIndex = 4;
			// 
			// AnimListBox
			// 
			this->AnimListBox->FormattingEnabled = true;
			this->AnimListBox->HorizontalScrollbar = true;
			this->AnimListBox->ItemHeight = 12;
			this->AnimListBox->Location = System::Drawing::Point(14, 39);
			this->AnimListBox->Name = L"AnimListBox";
			this->AnimListBox->Size = System::Drawing::Size(494, 196);
			this->AnimListBox->TabIndex = 2;
			this->AnimListBox->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::AnimListBox_MouseDoubleClick);
			// 
			// animPathOpenButton
			// 
			this->animPathOpenButton->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->animPathOpenButton->Location = System::Drawing::Point(480, 12);
			this->animPathOpenButton->Name = L"animPathOpenButton";
			this->animPathOpenButton->Size = System::Drawing::Size(28, 23);
			this->animPathOpenButton->TabIndex = 1;
			this->animPathOpenButton->Text = L"...";
			this->animPathOpenButton->UseVisualStyleBackColor = true;
			this->animPathOpenButton->Click += gcnew System::EventHandler(this, &Form1::animPathOpenButton_Click);
			// 
			// animPathTextBox
			// 
			this->animPathTextBox->Location = System::Drawing::Point(14, 14);
			this->animPathTextBox->Name = L"animPathTextBox";
			this->animPathTextBox->Size = System::Drawing::Size(460, 19);
			this->animPathTextBox->TabIndex = 0;
			this->animPathTextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::animPathTextBox_KeyDown);
			// 
			// panel3
			// 
			this->panel3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel3->Controls->Add(this->RestartButton);
			this->panel3->Controls->Add(this->ResetButton);
			this->panel3->Controls->Add(this->PlayButton);
			this->panel3->Location = System::Drawing::Point(395, 283);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(525, 29);
			this->panel3->TabIndex = 5;
			// 
			// RestartButton
			// 
			this->RestartButton->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"RestartButton.Image")));
			this->RestartButton->Location = System::Drawing::Point(61, 2);
			this->RestartButton->Name = L"RestartButton";
			this->RestartButton->Size = System::Drawing::Size(24, 24);
			this->RestartButton->TabIndex = 2;
			this->RestartButton->UseVisualStyleBackColor = true;
			this->RestartButton->Click += gcnew System::EventHandler(this, &Form1::RestartButton_Click);
			// 
			// ResetButton
			// 
			this->ResetButton->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ResetButton.Image")));
			this->ResetButton->Location = System::Drawing::Point(32, 2);
			this->ResetButton->Name = L"ResetButton";
			this->ResetButton->Size = System::Drawing::Size(24, 24);
			this->ResetButton->TabIndex = 1;
			this->ResetButton->UseVisualStyleBackColor = true;
			this->ResetButton->Click += gcnew System::EventHandler(this, &Form1::ResetButton_Click);
			// 
			// PlayButton
			// 
			this->PlayButton->ForeColor = System::Drawing::SystemColors::HighlightText;
			this->PlayButton->ImageIndex = 0;
			this->PlayButton->ImageList = this->imageList1;
			this->PlayButton->Location = System::Drawing::Point(4, 2);
			this->PlayButton->Name = L"PlayButton";
			this->PlayButton->Size = System::Drawing::Size(24, 24);
			this->PlayButton->TabIndex = 0;
			this->PlayButton->UseVisualStyleBackColor = true;
			this->PlayButton->Click += gcnew System::EventHandler(this, &Form1::PlayButton_Click);
			// 
			// imageList1
			// 
			this->imageList1->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^>(resources->GetObject(L"imageList1.ImageStream")));
			this->imageList1->TransparentColor = System::Drawing::Color::Transparent;
			this->imageList1->Images->SetKeyName(0, L"playIcon.bmp");
			this->imageList1->Images->SetKeyName(1, L"stopIcon.bmp");
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(932, 517);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->contextMenuStrip1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

#pragma region MainForm

	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
		this->trackBar1->Maximum = MAX_TICK_NUM;
		this->trackBar1->Size = System::Drawing::Size(MAX_TICK_NUM*TICK_WDTH, 45);

		for (const ACTION_TYPE_NAME& act : ACTION_NAME_ARRAY) {
			ToolStripMenuItem^ item = gcnew ToolStripMenuItem();
			item->Name = gcnew String(act.name);
			item->Size = System::Drawing::Size(180, 22);
			item->Text = gcnew String(act.name);
			item->Click += gcnew System::EventHandler(this, &Form1::AddToolStripMenuItem_Click);
			this->AddToolStripMenuItem->DropDownItems->Add(item);
		}
		
		if (DsSimu::GetIns())
		{
			DsSimu::GetIns()->Setup(pictureBox1->Handle.ToInt64());
		}
	}
#pragma endregion


#pragma region Timer
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
		if (DsSimu::GetIns())
		{
			if (DsSimu::GetIns()->IsInit())
			{
				DsSimu::GetIns()->RefWindow().Display();

				if (m_isPlayAnim) {
					this->trackBar1->Value = (this->trackBar1->Value+1) % (this->trackBar1->Maximum);
				}

			}
		}
	}
#pragma endregion


#pragma region PictureBox
	private: System::Void pictureBox1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		if (DsSimu::GetIns())
		{
			if (DsSimu::GetIns()->IsInit())
			{
				DsSimu::GetIns()->RefWindow().Reshape();
			}
		}
	}
	 //�N���b�N����
	private: System::Void pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		pictureBox1->Focus();//�t�H�[�J�X���ĂȂ��ƃ}�E�X�z�C�[���C�x���g�����s����Ȃ��炵���B
		
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsMouseClickState clickState = DsMouseClickState::DS_NON_CLICK;
			switch (e->Button)
			{
			case Forms::MouseButtons::Left:
				clickState = DsMouseClickState::DS_LEFT_CLICK;
				break;

			case Forms::MouseButtons::Right:
				clickState = DsMouseClickState::DS_RIGHT_CLICK;
				break;

			case Forms::MouseButtons::Middle:
				clickState = DsMouseClickState::DS_CENTER_CLICK;
				break;

			case Forms::MouseButtons::None:
				clickState = DsMouseClickState::DS_NON_CLICK;
				break;

			default:
				break;
			}
			DsSimu::GetIns()->RefWindow().Click(clickState, DsMouseUpDown::DS_DOWN_MOUSE, e->X, e->Y);
		}
	}
	private: System::Void pictureBox1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().Click(DsMouseClickState::DS_NON_CLICK, DsMouseUpDown::DS_UP_MOUSE, e->X, e->Y);
		}
	}
	private: System::Void pictureBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().Drag(e->X, e->Y);
		}
	}
	//�z�C�[������
	private: System::Void pictureBox1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().Wheel(e->Delta / WHEEL_DELTA, e->X, e->Y);
		}
	}
#pragma endregion



#pragma region ActionBar
	private: System::Void textBox1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		TextBox^ textBox = (TextBox^)sender;
		Drawing::Point mousePos = Drawing::Point(Forms::Cursor::Position.X, Forms::Cursor::Position.Y);
		const int margin = 20;

		ActionBarTag^ tag = (ActionBarTag^)textBox->Tag;

		if (e->Button == Forms::MouseButtons::Left) {
			Drawing::Point sPos = mousePos;
			Drawing::Point sPrePos = tag->preMousePos;
			const int dx = sPos.X - sPrePos.X;

			const int right = textBox->Size.Width - margin;
			if (e->X < margin)
			{
				if ((margin < textBox->Size.Width) || (dx<0)) {//�ׂ��Ȃ肷���Ȃ��悤��
					if (0 < (textBox->Location.X + dx)) {//0�����̃t���[���ɂȂ�Ȃ��悤��
						textBox->Size = Drawing::Size(textBox->Size.Width - dx, textBox->Size.Height);
						textBox->Location = Drawing::Point(textBox->Location.X + dx, textBox->Location.Y);
					}
				}

			}
			else if (right < e->X)
			{
				if ((margin < textBox->Size.Width) || (0<dx)) {//�ׂ��Ȃ肷���Ȃ��悤��
					textBox->Size = Drawing::Size(textBox->Size.Width + dx, textBox->Size.Height);
				}
			}
		}

		tag->startTime = SPF * static_cast<float>(textBox->Location.X) / TICK_WDTH;
		tag->endTime = SPF * static_cast<float>(textBox->Location.X+ textBox->Size.Width) / TICK_WDTH;
		tag->preMousePos = mousePos;
	}

	private: System::Void textBox1_Click(System::Object^  sender, System::EventArgs^  e) {
		TextBox^ bar = (TextBox^)(sender);
		ActionBarTag^ tag = (ActionBarTag^)bar->Tag;

		this->dataGridView1->Rows->Clear();
		switch (tag->actionType) {
		case TRACE_EFFECT:
		{
			TRACE_EFFECT_PARAM^ param = (TRACE_EFFECT_PARAM^)tag->param;
			this->dataGridView1->Rows->Add(param->paramNum);
			DataGridViewTextBoxCell^ cell00 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell01 = gcnew DataGridViewTextBoxCell();
			cell00->Value = param->dmyPolyId0.name;
			cell01->Value = param->dmyPolyId0.value;
			cell01->Tag = param->dmyPolyId0.value;
			this->dataGridView1->Rows[0]->Cells[0] = cell00;
			this->dataGridView1->Rows[0]->Cells[1] = cell01;
			DataGridViewTextBoxCell^ cell10 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell11 = gcnew DataGridViewTextBoxCell();
			cell10->Value = param->dmyPolyId1.name;
			cell11->Value = param->dmyPolyId1.value;
			cell11->Tag = param->dmyPolyId1.value;
			this->dataGridView1->Rows[1]->Cells[0] = cell10;
			this->dataGridView1->Rows[1]->Cells[1] = cell11;
			DataGridViewTextBoxCell^ cell20 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell21 = gcnew DataGridViewTextBoxCell();
			cell20->Value = param->effectId.name;
			cell21->Value = param->effectId.value;
			cell21->Tag = param->effectId.value;
			this->dataGridView1->Rows[2]->Cells[0] = cell20;
			this->dataGridView1->Rows[2]->Cells[1] = cell21;
		}
		break;

		case DAMAGE:
		{
			DAMAGE_PARAM^ param = (DAMAGE_PARAM^)tag->param;
			this->dataGridView1->Rows->Add(param->paramNum);
			DataGridViewTextBoxCell^ cell00 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell01 = gcnew DataGridViewTextBoxCell();
			cell00->Value = param->dmyPolyId0.name;
			cell01->Value = param->dmyPolyId0.value;
			cell01->Tag = param->dmyPolyId0.value;
			this->dataGridView1->Rows[0]->Cells[0] = cell00;
			this->dataGridView1->Rows[0]->Cells[1] = cell01;
			DataGridViewTextBoxCell^ cell10 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell11 = gcnew DataGridViewTextBoxCell();
			cell10->Value = param->dmyPolyId1.name;
			cell11->Value = param->dmyPolyId1.value;
			cell11->Tag = param->dmyPolyId1.value;
			this->dataGridView1->Rows[1]->Cells[0] = cell10;
			this->dataGridView1->Rows[1]->Cells[1] = cell11;
			DataGridViewTextBoxCell^ cell20 = gcnew DataGridViewTextBoxCell();
			DataGridViewTextBoxCell^ cell21 = gcnew DataGridViewTextBoxCell();
			cell20->Value = param->damageId.name;
			cell21->Value = param->damageId.value;
			cell21->Tag = param->damageId.value;
			this->dataGridView1->Rows[2]->Cells[0] = cell20;
			this->dataGridView1->Rows[2]->Cells[1] = cell21;
		}
		break;

		default:
			break;
		}
	}

	private: System::Void UpdateLocateTime(TrackBar^ bar) {
		const double time = TICK_SEC * static_cast<double>(bar->Value);
		DsSimu::GetIns()->SetLocalTimeAnim(time);
		if (DsSimu::GetIns()->IsEndAnim()) {
			bar->Value = 0;
		}
	}

	private: System::Void trackBar1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
		TrackBar^ bar = (TrackBar^)(sender);
		UpdateLocateTime(bar);
	}

#pragma endregion


#pragma region ActionContextMenu
	private: System::Void AddToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		ToolStripMenuItem^ item = (ToolStripMenuItem^)sender;
		
		const ACTION_TYPE_NAME* pAct = NULL;

		for (const ACTION_TYPE_NAME& act : ACTION_NAME_ARRAY) {
			String^ str = gcnew String(act.name);
			if (item->Name == str) {
				pAct = &act;
				break;
			}
		}

		if (pAct) {
			float startTime = 0;
			float endTime = 0.1f;
			int barSize = static_cast<int>(FPSf * endTime * static_cast<float>(TICK_WDTH));

			TextBox^ addBar = gcnew TextBox();
			addBar->Anchor = System::Windows::Forms::AnchorStyles::None;
			addBar->BackColor = System::Drawing::Color::BlueViolet;
			addBar->Font = (gcnew System::Drawing::Font(L"�l�r �S�V�b�N", 10.0F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			addBar->ForeColor = System::Drawing::SystemColors::HighlightText;
			addBar->Location = System::Drawing::Point(0, m_actionNum*HEIGHT_BAR + this->trackBar1->Size.Height);
			addBar->Name = L"textBox1";
			addBar->Size = System::Drawing::Size(barSize, HEIGHT_BAR);
			addBar->Text = gcnew String(pAct->name);
			addBar->ReadOnly = true;
			addBar->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::textBox1_MouseMove);
			addBar->Click += gcnew System::EventHandler(this, &Form1::textBox1_Click);

			ActionBarTag^ tag = gcnew ActionBarTag();
			tag->actionType = pAct->type;
			tag->startTime = startTime;
			tag->endTime = endTime;

			//�p�����[�^�̃C���X�^���X�쐬
			switch (tag->actionType) {
			case TRACE_EFFECT:
				tag->param = gcnew TRACE_EFFECT_PARAM();
				break;

			case DAMAGE:
				tag->param = gcnew DAMAGE_PARAM();
				break;

			default:
				break;
			}
			addBar->Tag = tag;

			this->panel1->Controls->Add(addBar);
			++m_actionNum;

		}
	}

#pragma endregion


#pragma region ParamEditor

	private: System::Void dataGridView1_CellEndEdit(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		DataGridView^ grid = (DataGridView^)sender;
		
		Object^ val = grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value;
		Int32 iVal = -1;
		if (val->GetType() == Int32::typeid) {
			iVal = (Int32)val;
		}
		else{
			String^ str = (String^)val;
			if (!Int32::TryParse(str, iVal)) {
				iVal = -1;
			}
		}

		//�ҏW�����p�����[�^���f�[�^�ɔ��f
		grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value = iVal;
		Int32^ src = (Int32^)grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Tag;
		(*src) = iVal;
	}
#pragma endregion

#pragma region MenuBar
	private: System::Void openToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	}
#pragma endregion

#pragma region AnimList
	private: System::Void animPathTextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		if (e->KeyCode == Keys::Enter) {
			TextBox^ tbox = (TextBox^)sender;
			if (IO::File::Exists(tbox->Text)) {
				m_animPath = tbox->Text;
				_CreateModel();
			}
		}
	}
	private: System::Void animPathOpenButton_Click(System::Object^  sender, System::EventArgs^  e) {
		OpenFileDialog ^ openFileDialog = gcnew OpenFileDialog();
		openFileDialog->Filter = L"�A�j�����f��|*.dmdl";
		openFileDialog->Title = L"�A�j�����f���̓ǂݍ���";
		if (openFileDialog->ShowDialog() == Forms::DialogResult::OK)
		{
			m_animPath = openFileDialog->FileName;
			_CreateModel();
		}
	}

	private: System::Void _CreateModel()
	{
		DsSimu::GetIns()->ClearObj();
		const char* animPath = CreateCharArray(m_animPath);
		String^ current = IO::Directory::GetCurrentDirectory();
		String^ tmp = current + L"\\" + L"dummyHit.txt";
		const char* hitPath = CreateCharArray(tmp);
		const std::vector<std::string> clips = DsSimu::GetIns()->RegisterObj(animPath, hitPath, 0, 0, 0, 0, 180, 0, DS_MAP_OBJ_TYPE::CONTROL, DS_MAP_FIELD_OBJ_TYPE::CHR);
		if (!clips.empty()) {
			animPathTextBox->Text = m_animPath;
		}
		else {
			animPathTextBox->Text = L"";
		}

		this->AnimListBox->Items->Clear();
		for (std::string name : clips) {
			this->AnimListBox->Items->Add(gcnew String(name.c_str()));
		}

		delete animPath;
		delete hitPath;
	}

	private: System::Void AnimListBox_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		ListBox^ listBox = (ListBox^)(sender);
		String^ item = (String^)listBox->SelectedItem;
		if (item) {

			const char* name = CreateCharArray(item);
			DsSimu::GetIns()->SetCurrentAnimName(name);

			this->trackBar1->Value = 0;
			UpdateLocateTime(this->trackBar1);

			delete name;
		}
	}

#pragma endregion


#pragma region PlayButton
	private: System::Void PlayButton_Click(System::Object^  sender, System::EventArgs^  e) {
		m_isPlayAnim = !m_isPlayAnim;
		if (m_isPlayAnim) {
			this->PlayButton->ImageIndex = 1;
		}
		else {
			this->PlayButton->ImageIndex = 0;
		}
	}

	private: System::Void ResetButton_Click(System::Object^  sender, System::EventArgs^  e) {
		m_isPlayAnim = false;
		this->trackBar1->Value = 0;
		this->PlayButton->ImageIndex = 0;
	}

	private: System::Void RestartButton_Click(System::Object^  sender, System::EventArgs^  e) {
		this->trackBar1->Value = this->trackBar1->Value+1;
		m_isPlayAnim = false;
		this->PlayButton->ImageIndex = 0;
	}
#pragma endregion

};
}
