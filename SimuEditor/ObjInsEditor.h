#pragma once

#ifndef _DS_DATA_FORMAT_H_
#include "DsDataFormat.h"
#endif
#ifndef _DS_SIMU_UTIL_
#include "DsSimuUtlil.h"
#endif

namespace SimuEditor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// ObjInsEditor の概要
	/// </summary>
	public ref class ObjInsEditor : public System::Windows::Forms::Form
	{
	public:
		ObjInsEditor(void)
		{
			m_editItem = nullptr;
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~ObjInsEditor()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		ListViewItem^ m_editItem;



	private: System::Windows::Forms::Label^  ObjInsNameLabel;
	public:
	private: System::Windows::Forms::TextBox^  ObjInsNameTextBox;
	private: System::Windows::Forms::Label^  DrawModelPathLabel;
	private: System::Windows::Forms::TextBox^  drawModelPathTextBox;



	private: System::Windows::Forms::Label^  ObjPosition;
	private: System::Windows::Forms::NumericUpDown^  PosX;
	private: System::Windows::Forms::NumericUpDown^  PosY;
	private: System::Windows::Forms::NumericUpDown^  PosZ;
	private: System::Windows::Forms::Label^  X;
	private: System::Windows::Forms::Label^  Y;
	private: System::Windows::Forms::Label^  Z;
	private: System::Windows::Forms::Label^  ObjAngle;
	private: System::Windows::Forms::NumericUpDown^  AngX;
	private: System::Windows::Forms::NumericUpDown^  AngY;
	private: System::Windows::Forms::NumericUpDown^  AngZ;
	private: System::Windows::Forms::Label^  ObjType;
	private: System::Windows::Forms::ComboBox^  ObjTypeComboBox;
	private: System::Windows::Forms::Button^  Complete;
	private: System::Windows::Forms::Label^  FieldObjType;
	private: System::Windows::Forms::ComboBox^  FieldObjComboBox;
	private: System::Windows::Forms::Button^  OpenFileDrawModelPathButton;


	private: System::Windows::Forms::Label^  hitModelLabel;
	private: System::Windows::Forms::TextBox^  hitModelPathTextBox;
	private: System::Windows::Forms::Button^  OpenFileHitModelPathButton;








	public:
	private:

	private:

	protected:

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->ObjInsNameLabel = (gcnew System::Windows::Forms::Label());
			this->ObjInsNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->DrawModelPathLabel = (gcnew System::Windows::Forms::Label());
			this->drawModelPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ObjPosition = (gcnew System::Windows::Forms::Label());
			this->PosX = (gcnew System::Windows::Forms::NumericUpDown());
			this->PosY = (gcnew System::Windows::Forms::NumericUpDown());
			this->PosZ = (gcnew System::Windows::Forms::NumericUpDown());
			this->X = (gcnew System::Windows::Forms::Label());
			this->Y = (gcnew System::Windows::Forms::Label());
			this->Z = (gcnew System::Windows::Forms::Label());
			this->ObjAngle = (gcnew System::Windows::Forms::Label());
			this->AngX = (gcnew System::Windows::Forms::NumericUpDown());
			this->AngY = (gcnew System::Windows::Forms::NumericUpDown());
			this->AngZ = (gcnew System::Windows::Forms::NumericUpDown());
			this->ObjType = (gcnew System::Windows::Forms::Label());
			this->ObjTypeComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->Complete = (gcnew System::Windows::Forms::Button());
			this->FieldObjType = (gcnew System::Windows::Forms::Label());
			this->FieldObjComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->OpenFileDrawModelPathButton = (gcnew System::Windows::Forms::Button());
			this->hitModelLabel = (gcnew System::Windows::Forms::Label());
			this->hitModelPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->OpenFileHitModelPathButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosX))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosY))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosZ))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngX))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngY))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngZ))->BeginInit();
			this->SuspendLayout();
			// 
			// ObjInsNameLabel
			// 
			this->ObjInsNameLabel->AutoSize = true;
			this->ObjInsNameLabel->Location = System::Drawing::Point(12, 57);
			this->ObjInsNameLabel->Name = L"ObjInsNameLabel";
			this->ObjInsNameLabel->Size = System::Drawing::Size(121, 12);
			this->ObjInsNameLabel->TabIndex = 3;
			this->ObjInsNameLabel->Text = L"オブジェクトインスタンス名";
			// 
			// ObjInsNameTextBox
			// 
			this->ObjInsNameTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ObjInsNameTextBox->Location = System::Drawing::Point(144, 50);
			this->ObjInsNameTextBox->Name = L"ObjInsNameTextBox";
			this->ObjInsNameTextBox->Size = System::Drawing::Size(370, 19);
			this->ObjInsNameTextBox->TabIndex = 4;
			this->ObjInsNameTextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ObjInsEditor::ObjInsNameTextBox_KeyDown);
			// 
			// DrawModelPathLabel
			// 
			this->DrawModelPathLabel->AutoSize = true;
			this->DrawModelPathLabel->Location = System::Drawing::Point(12, 10);
			this->DrawModelPathLabel->Name = L"DrawModelPathLabel";
			this->DrawModelPathLabel->Size = System::Drawing::Size(77, 12);
			this->DrawModelPathLabel->TabIndex = 5;
			this->DrawModelPathLabel->Text = L"描画モデルパス";
			// 
			// drawModelPathTextBox
			// 
			this->drawModelPathTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->drawModelPathTextBox->Location = System::Drawing::Point(144, 7);
			this->drawModelPathTextBox->Name = L"drawModelPathTextBox";
			this->drawModelPathTextBox->Size = System::Drawing::Size(337, 19);
			this->drawModelPathTextBox->TabIndex = 6;
			// 
			// ObjPosition
			// 
			this->ObjPosition->AutoSize = true;
			this->ObjPosition->Location = System::Drawing::Point(16, 92);
			this->ObjPosition->Name = L"ObjPosition";
			this->ObjPosition->Size = System::Drawing::Size(29, 12);
			this->ObjPosition->TabIndex = 7;
			this->ObjPosition->Text = L"位置";
			// 
			// PosX
			// 
			this->PosX->DecimalPlaces = 3;
			this->PosX->Location = System::Drawing::Point(144, 90);
			this->PosX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
			this->PosX->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, System::Int32::MinValue });
			this->PosX->Name = L"PosX";
			this->PosX->Size = System::Drawing::Size(111, 19);
			this->PosX->TabIndex = 8;
			// 
			// PosY
			// 
			this->PosY->DecimalPlaces = 3;
			this->PosY->Location = System::Drawing::Point(261, 90);
			this->PosY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
			this->PosY->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, System::Int32::MinValue });
			this->PosY->Name = L"PosY";
			this->PosY->Size = System::Drawing::Size(95, 19);
			this->PosY->TabIndex = 9;
			// 
			// PosZ
			// 
			this->PosZ->DecimalPlaces = 3;
			this->PosZ->Location = System::Drawing::Point(362, 90);
			this->PosZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
			this->PosZ->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, System::Int32::MinValue });
			this->PosZ->Name = L"PosZ";
			this->PosZ->Size = System::Drawing::Size(93, 19);
			this->PosZ->TabIndex = 10;
			// 
			// X
			// 
			this->X->AutoSize = true;
			this->X->Location = System::Drawing::Point(142, 75);
			this->X->Name = L"X";
			this->X->Size = System::Drawing::Size(12, 12);
			this->X->TabIndex = 11;
			this->X->Text = L"X";
			// 
			// Y
			// 
			this->Y->AutoSize = true;
			this->Y->Location = System::Drawing::Point(259, 75);
			this->Y->Name = L"Y";
			this->Y->Size = System::Drawing::Size(12, 12);
			this->Y->TabIndex = 12;
			this->Y->Text = L"Y";
			// 
			// Z
			// 
			this->Z->AutoSize = true;
			this->Z->Location = System::Drawing::Point(360, 75);
			this->Z->Name = L"Z";
			this->Z->Size = System::Drawing::Size(12, 12);
			this->Z->TabIndex = 13;
			this->Z->Text = L"Z";
			// 
			// ObjAngle
			// 
			this->ObjAngle->AutoSize = true;
			this->ObjAngle->Location = System::Drawing::Point(16, 120);
			this->ObjAngle->Name = L"ObjAngle";
			this->ObjAngle->Size = System::Drawing::Size(29, 12);
			this->ObjAngle->TabIndex = 14;
			this->ObjAngle->Text = L"角度";
			// 
			// AngX
			// 
			this->AngX->DecimalPlaces = 3;
			this->AngX->Location = System::Drawing::Point(144, 118);
			this->AngX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, 0 });
			this->AngX->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, System::Int32::MinValue });
			this->AngX->Name = L"AngX";
			this->AngX->Size = System::Drawing::Size(111, 19);
			this->AngX->TabIndex = 15;
			// 
			// AngY
			// 
			this->AngY->DecimalPlaces = 3;
			this->AngY->Location = System::Drawing::Point(261, 118);
			this->AngY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, 0 });
			this->AngY->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, System::Int32::MinValue });
			this->AngY->Name = L"AngY";
			this->AngY->Size = System::Drawing::Size(94, 19);
			this->AngY->TabIndex = 16;
			// 
			// AngZ
			// 
			this->AngZ->DecimalPlaces = 3;
			this->AngZ->Location = System::Drawing::Point(362, 118);
			this->AngZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, 0 });
			this->AngZ->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 180, 0, 0, System::Int32::MinValue });
			this->AngZ->Name = L"AngZ";
			this->AngZ->Size = System::Drawing::Size(93, 19);
			this->AngZ->TabIndex = 17;
			// 
			// ObjType
			// 
			this->ObjType->AutoSize = true;
			this->ObjType->Location = System::Drawing::Point(12, 151);
			this->ObjType->Name = L"ObjType";
			this->ObjType->Size = System::Drawing::Size(66, 12);
			this->ObjType->TabIndex = 18;
			this->ObjType->Text = L"オブジェタイプ";
			// 
			// ObjTypeComboBox
			// 
			this->ObjTypeComboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->ObjTypeComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->ObjTypeComboBox->FormattingEnabled = true;
			this->ObjTypeComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"静的オブジェ", L"動的オブジェ", L"自律オブジェ", L"操作オブジェ" });
			this->ObjTypeComboBox->Location = System::Drawing::Point(144, 148);
			this->ObjTypeComboBox->Name = L"ObjTypeComboBox";
			this->ObjTypeComboBox->Size = System::Drawing::Size(370, 20);
			this->ObjTypeComboBox->TabIndex = 19;
			// 
			// Complete
			// 
			this->Complete->Location = System::Drawing::Point(439, 200);
			this->Complete->Name = L"Complete";
			this->Complete->Size = System::Drawing::Size(75, 23);
			this->Complete->TabIndex = 20;
			this->Complete->Text = L"完了";
			this->Complete->UseVisualStyleBackColor = true;
			this->Complete->Click += gcnew System::EventHandler(this, &ObjInsEditor::Complete_Click);
			// 
			// FieldObjType
			// 
			this->FieldObjType->AutoSize = true;
			this->FieldObjType->Location = System::Drawing::Point(12, 177);
			this->FieldObjType->Name = L"FieldObjType";
			this->FieldObjType->Size = System::Drawing::Size(84, 12);
			this->FieldObjType->TabIndex = 21;
			this->FieldObjType->Text = L"フィールドオブジェ";
			// 
			// FieldObjComboBox
			// 
			this->FieldObjComboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->FieldObjComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->FieldObjComboBox->FormattingEnabled = true;
			this->FieldObjComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"キャラ", L"オブジェ", L"ヒット", L"プレイヤー" });
			this->FieldObjComboBox->Location = System::Drawing::Point(144, 174);
			this->FieldObjComboBox->Name = L"FieldObjComboBox";
			this->FieldObjComboBox->Size = System::Drawing::Size(370, 20);
			this->FieldObjComboBox->TabIndex = 22;
			// 
			// OpenFileDrawModelPathButton
			// 
			this->OpenFileDrawModelPathButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->OpenFileDrawModelPathButton->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
			this->OpenFileDrawModelPathButton->Location = System::Drawing::Point(487, 7);
			this->OpenFileDrawModelPathButton->Name = L"OpenFileDrawModelPathButton";
			this->OpenFileDrawModelPathButton->Size = System::Drawing::Size(27, 18);
			this->OpenFileDrawModelPathButton->TabIndex = 23;
			this->OpenFileDrawModelPathButton->Text = L"…";
			this->OpenFileDrawModelPathButton->UseVisualStyleBackColor = true;
			this->OpenFileDrawModelPathButton->Click += gcnew System::EventHandler(this, &ObjInsEditor::OpenFileDrawModelPathButton_Click);
			// 
			// hitModelLabel
			// 
			this->hitModelLabel->AutoSize = true;
			this->hitModelLabel->Location = System::Drawing::Point(13, 31);
			this->hitModelLabel->Name = L"hitModelLabel";
			this->hitModelLabel->Size = System::Drawing::Size(76, 12);
			this->hitModelLabel->TabIndex = 24;
			this->hitModelLabel->Text = L"ヒットモデルパス";
			// 
			// hitModelPathTextBox
			// 
			this->hitModelPathTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->hitModelPathTextBox->Location = System::Drawing::Point(144, 28);
			this->hitModelPathTextBox->Name = L"hitModelPathTextBox";
			this->hitModelPathTextBox->Size = System::Drawing::Size(337, 19);
			this->hitModelPathTextBox->TabIndex = 25;
			// 
			// OpenFileHitModelPathButton
			// 
			this->OpenFileHitModelPathButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->OpenFileHitModelPathButton->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9));
			this->OpenFileHitModelPathButton->Location = System::Drawing::Point(487, 28);
			this->OpenFileHitModelPathButton->Name = L"OpenFileHitModelPathButton";
			this->OpenFileHitModelPathButton->Size = System::Drawing::Size(27, 18);
			this->OpenFileHitModelPathButton->TabIndex = 26;
			this->OpenFileHitModelPathButton->Text = L"…";
			this->OpenFileHitModelPathButton->UseVisualStyleBackColor = true;
			this->OpenFileHitModelPathButton->Click += gcnew System::EventHandler(this, &ObjInsEditor::OpenFileHitModelPathButton_Click);
			// 
			// ObjInsEditor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(526, 228);
			this->Controls->Add(this->OpenFileHitModelPathButton);
			this->Controls->Add(this->hitModelPathTextBox);
			this->Controls->Add(this->hitModelLabel);
			this->Controls->Add(this->OpenFileDrawModelPathButton);
			this->Controls->Add(this->FieldObjComboBox);
			this->Controls->Add(this->FieldObjType);
			this->Controls->Add(this->Complete);
			this->Controls->Add(this->ObjTypeComboBox);
			this->Controls->Add(this->ObjType);
			this->Controls->Add(this->AngZ);
			this->Controls->Add(this->AngY);
			this->Controls->Add(this->AngX);
			this->Controls->Add(this->ObjAngle);
			this->Controls->Add(this->Z);
			this->Controls->Add(this->Y);
			this->Controls->Add(this->X);
			this->Controls->Add(this->PosZ);
			this->Controls->Add(this->PosY);
			this->Controls->Add(this->PosX);
			this->Controls->Add(this->ObjPosition);
			this->Controls->Add(this->drawModelPathTextBox);
			this->Controls->Add(this->DrawModelPathLabel);
			this->Controls->Add(this->ObjInsNameTextBox);
			this->Controls->Add(this->ObjInsNameLabel);
			this->Name = L"ObjInsEditor";
			this->Text = L"ObjInsEditor";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosX))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosY))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PosZ))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngX))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngY))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AngZ))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
#pragma region プライベートメソッド
	private:
		ListViewItem^ _SetObjInsData();
#pragma endregion

#pragma region パブリックメソッド
	public:
		void EditObjInsData(ListViewItem^ item);
#pragma endregion

	//オブジェインス名にキー入力
	private: System::Void ObjInsNameTextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);

	//完了ボタン
	private: System::Void Complete_Click(System::Object^  sender, System::EventArgs^  e);
	
	//データパス開くボタン
	private: System::Void OpenFileDrawModelPathButton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void OpenFileHitModelPathButton_Click(System::Object^  sender, System::EventArgs^  e);
};
}
