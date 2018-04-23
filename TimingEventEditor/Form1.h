#pragma once
#ifndef _DS_SIMU_H_
#include "Simu/DsSimu.h"
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
	/// Zusammenfassung f・ Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			DsSimu::Initialize();
			//
			//TODO: Konstruktorcode hier hinzuf・en.
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





	protected:

	private:
		int m_actionNum = 0;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode f・ die Designerunterst・zung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor ge舅dert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->AddToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->panel1->SuspendLayout();
			this->contextMenuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox1->Location = System::Drawing::Point(12, 12);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(337, 249);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::pictureBox1_Paint);
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
			this->panel1->Location = System::Drawing::Point(243, 277);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(627, 187);
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
			this->AddToolStripMenuItem->Text = L"追加";
			// 
			// trackBar1
			// 
			this->trackBar1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->trackBar1->Location = System::Drawing::Point(3, 3);
			this->trackBar1->Maximum = 100;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(792, 45);
			this->trackBar1->TabIndex = 2;
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
			this->dataGridView1->Location = System::Drawing::Point(12, 277);
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
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(882, 476);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->contextMenuStrip1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

#pragma region MainForm

	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
		this->trackBar1->Maximum = MAX_FRAME;
		this->trackBar1->Size = System::Drawing::Size(WDTH_FRAME*MAX_FRAME, 45);

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
				if ((margin < textBox->Size.Width) || (dx<0)) {//細くなりすぎないように
					if (0 < (textBox->Location.X + dx)) {//0未満のフレームにならないように
						textBox->Size = Drawing::Size(textBox->Size.Width - dx, textBox->Size.Height);
						textBox->Location = Drawing::Point(textBox->Location.X + dx, textBox->Location.Y);
					}
				}

			}
			else if (right < e->X)
			{
				if ((margin < textBox->Size.Width) || (0<dx)) {//細くなりすぎないように
					textBox->Size = Drawing::Size(textBox->Size.Width + dx, textBox->Size.Height);
				}
			}
		}
		else {
		
		}

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

		default:
			break;
		}

		
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
			TextBox^ addBar = gcnew TextBox();
			addBar->Anchor = System::Windows::Forms::AnchorStyles::None;
			addBar->BackColor = System::Drawing::Color::Orange;
			addBar->Font = (gcnew System::Drawing::Font(L"ＭＳ ゴシック", 10.0F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			addBar->Location = System::Drawing::Point(0, m_actionNum*HEIGHT_BAR + this->trackBar1->Size.Height);
			addBar->Name = L"textBox1";
			addBar->Size = System::Drawing::Size(50, HEIGHT_BAR);
			addBar->Text = gcnew String(pAct->name);
			addBar->ReadOnly = true;
			addBar->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::textBox1_MouseMove);
			addBar->Click += gcnew System::EventHandler(this, &Form1::textBox1_Click);

			ActionBarTag^ tag = gcnew ActionBarTag();
			tag->actionType = pAct->type;
			//パラメータのインスタンス作成
			switch (tag->actionType) {
			case TRACE_EFFECT:
				tag->param = gcnew TRACE_EFFECT_PARAM();
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

		//編集したパラメータをデータに反映
		grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value = iVal;
		Int32^ src = (Int32^)grid->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Tag;
		(*src) = iVal;
	}
#pragma endregion


};
}
