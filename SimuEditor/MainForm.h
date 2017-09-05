#pragma once

#ifndef _DS_SIMU_H_
#include "DsSimu.h"
#endif

#include "ObjInsEditor.h"
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
	using namespace System::Text;


	/// <summary>
	/// MainForm の概要
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
			DsSimu::Initialize();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
			DsSimu::Finalize();
		}


	protected:
	private: System::Windows::Forms::Timer^  timer1;



	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  ファイルToolStripMenuItem;



	private:





	public: System::Windows::Forms::ListView^  ObjInsListView;
	private:



	private: System::Windows::Forms::ColumnHeader^  ObjectIns;



	private: System::Windows::Forms::ContextMenuStrip^  ObjInsContextMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  AddObjIns;
	private: System::Windows::Forms::ToolStripMenuItem^  EraseObjIns;
	private: System::Windows::Forms::OpenFileDialog^  ObjOpenFileDialog;
	private: System::Windows::Forms::ToolStripMenuItem^  EditOnjIns;
	private: System::Windows::Forms::ToolStripMenuItem^  開くToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存ToolStripMenuItem;
	private: System::Windows::Forms::PictureBox^  pictureBox1;












	private: System::ComponentModel::IContainer^  components;

	public:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->ファイルToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->開くToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ObjInsListView = (gcnew System::Windows::Forms::ListView());
			this->ObjectIns = (gcnew System::Windows::Forms::ColumnHeader());
			this->ObjInsContextMenu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->AddObjIns = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EditOnjIns = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EraseObjIns = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ObjOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->menuStrip1->SuspendLayout();
			this->ObjInsContextMenu->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 33;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainForm::timer1_Tick);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->ファイルToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1149, 26);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// ファイルToolStripMenuItem
			// 
			this->ファイルToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->開くToolStripMenuItem,
					this->保存ToolStripMenuItem
			});
			this->ファイルToolStripMenuItem->Name = L"ファイルToolStripMenuItem";
			this->ファイルToolStripMenuItem->Size = System::Drawing::Size(68, 22);
			this->ファイルToolStripMenuItem->Text = L"ファイル";
			// 
			// 開くToolStripMenuItem
			// 
			this->開くToolStripMenuItem->Name = L"開くToolStripMenuItem";
			this->開くToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->開くToolStripMenuItem->Text = L"開く";
			this->開くToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::開くToolStripMenuItem_Click);
			// 
			// 保存ToolStripMenuItem
			// 
			this->保存ToolStripMenuItem->Name = L"保存ToolStripMenuItem";
			this->保存ToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->保存ToolStripMenuItem->Text = L"保存";
			this->保存ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::保存ToolStripMenuItem_Click);
			// 
			// ObjInsListView
			// 
			this->ObjInsListView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->ObjInsListView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(1) { this->ObjectIns });
			this->ObjInsListView->ContextMenuStrip = this->ObjInsContextMenu;
			this->ObjInsListView->Location = System::Drawing::Point(12, 478);
			this->ObjInsListView->Name = L"ObjInsListView";
			this->ObjInsListView->Size = System::Drawing::Size(1125, 148);
			this->ObjInsListView->TabIndex = 4;
			this->ObjInsListView->UseCompatibleStateImageBehavior = false;
			this->ObjInsListView->View = System::Windows::Forms::View::Details;
			this->ObjInsListView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::ObjInsListView_KeyDown);
			// 
			// ObjectIns
			// 
			this->ObjectIns->Text = L"オブジェクトインスタンス";
			this->ObjectIns->Width = 262;
			// 
			// ObjInsContextMenu
			// 
			this->ObjInsContextMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->AddObjIns,
					this->EditOnjIns, this->EraseObjIns
			});
			this->ObjInsContextMenu->Name = L"ObjInsContextMenu";
			this->ObjInsContextMenu->Size = System::Drawing::Size(101, 70);
			// 
			// AddObjIns
			// 
			this->AddObjIns->Name = L"AddObjIns";
			this->AddObjIns->Size = System::Drawing::Size(100, 22);
			this->AddObjIns->Text = L"追加";
			this->AddObjIns->Click += gcnew System::EventHandler(this, &MainForm::AddObjIns_Click);
			// 
			// EditOnjIns
			// 
			this->EditOnjIns->Name = L"EditOnjIns";
			this->EditOnjIns->Size = System::Drawing::Size(100, 22);
			this->EditOnjIns->Text = L"編集";
			this->EditOnjIns->Click += gcnew System::EventHandler(this, &MainForm::EditOnjIns_Click);
			// 
			// EraseObjIns
			// 
			this->EraseObjIns->Name = L"EraseObjIns";
			this->EraseObjIns->Size = System::Drawing::Size(100, 22);
			this->EraseObjIns->Text = L"削除";
			this->EraseObjIns->Click += gcnew System::EventHandler(this, &MainForm::EraseObjIns_Click);
			// 
			// ObjOpenFileDialog
			// 
			this->ObjOpenFileDialog->Filter = L"独自形式のモデル情報|*.dmdl";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBox1->Location = System::Drawing::Point(12, 31);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(1125, 441);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainForm::pictureBox1_Paint);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseClick);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseMove);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseUp);
			this->pictureBox1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseWheel);
			this->pictureBox1->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &MainForm::pictureBox1_PreviewKeyDown);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1149, 638);
			this->Controls->Add(this->ObjInsListView);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->menuStrip1);
			this->KeyPreview = true;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"MainForm";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::MainForm_KeyDown);
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::MainForm_KeyUp);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ObjInsContextMenu->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

#pragma region 外部用フォーム操作系メソッド
	

#pragma endregion

	//MainFormとpictureBox
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
		if (DsSimu::GetIns())
		{
			if (DsSimu::GetIns()->IsInit())
			{
				DsSimu::GetIns()->RefWindow().Display();
			}
		}
	}
	private: System::Void pictureBox1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		if (DsSimu::GetIns())
		{
			if (DsSimu::GetIns()->IsInit())
			{
				DsSimu::GetIns()->RefWindow().Reshape();
			}
		}
	}
	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e) {
		if (DsSimu::GetIns())
		{
			DsSimu::GetIns()->Setup(pictureBox1->Handle.ToInt64());
		}
	}
	private: System::Void MainForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().Keyboard(e->KeyValue, 0, 0);
		}
	}
	private: System::Void MainForm_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().KeyboardUp(e->KeyValue, 0, 0);
		}
	}
	private: System::Void pictureBox1_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
	}

	//クリック処理
	private: System::Void pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		pictureBox1->Focus();//フォーカス当てないとマウスホイールイベントが発行されないらしい。
		
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsMouseClickState clickState = DsMouseClickState::DS_NON_CLICK;
			switch (e->Button)
			{
			case Windows::Forms::MouseButtons::Left:
				clickState = DsMouseClickState::DS_LEFT_CLICK;
				break;

			case Windows::Forms::MouseButtons::Right:
				clickState = DsMouseClickState::DS_RIGHT_CLICK;
				break;

			case Windows::Forms::MouseButtons::Middle:
				clickState = DsMouseClickState::DS_CENTER_CLICK;
				break;

			case Windows::Forms::MouseButtons::None:
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

	//ホイール処理
	private: System::Void pictureBox1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		if (DsSimu::GetIns() && DsSimu::GetIns()->IsInit())
		{
			DsSimu::GetIns()->RefWindow().Wheel(e->Delta / WHEEL_DELTA, e->X, e->Y);
		}
	}

//追加
private: System::Void AddObjIns_Click(System::Object^  sender, System::EventArgs^  e) {
	ObjInsEditor^ editor = gcnew ObjInsEditor();
	editor->ShowDialog(this);
}

//編集
private: System::Void EditOnjIns_Click(System::Object^  sender, System::EventArgs^  e) {
	
	if (ObjInsListView->SelectedItems->Count == 1)
	{
		ObjInsEditor^ editor = gcnew ObjInsEditor();
		editor->EditObjInsData(ObjInsListView->SelectedItems[0]);
		editor->ShowDialog(this);
	}
	else{
		//編集は、選択してるのが一つだけでなければ何もしない
	}
}

//削除
private: System::Void EraseObjIns_Click(System::Object^  sender, System::EventArgs^  e) {
	for each(ListViewItem^ item in ObjInsListView->SelectedItems)
	{
		if (DsSimu::GetIns())
		{
			ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
			DsApp::DsFieldObj* pObj = static_cast<DsApp::DsFieldObj*>(data->pFieldObj);
			DsSimu::GetIns()->Unregister(pObj);
		}
		ObjInsListView->Items->Remove(item);
	}
}

private: System::Void ObjInsListView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::Delete){
		for each(ListViewItem^ item in ObjInsListView->SelectedItems)
		{
			if (DsSimu::GetIns())
			{
				ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
				DsApp::DsFieldObj* pObj = static_cast<DsApp::DsFieldObj*>(data->pFieldObj);
				DsSimu::GetIns()->Unregister(pObj);
			}
			ObjInsListView->Items->Remove(item);
		}
	}
}


//メニュー
private: System::Void 開くToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog ^ openFileDialog = gcnew OpenFileDialog();
	openFileDialog->Filter = L"独自形式のマップ情報|*.dmap";
	openFileDialog->Title = L"マップ情報の読み込み";
	if (openFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		{//一旦全てのアイテムを破棄する
			ObjInsListView->Items->Clear();
			//オブジェクト削除
			DsSimu::GetIns()->ClearObj();
		}

		System::IO::FileStream ^ fs = safe_cast<System::IO::FileStream^>(openFileDialog->OpenFile());
		
		array<Byte>^ intTmp = gcnew array<Byte>(4);
		array<Byte>^ doubleTmp = gcnew array<Byte>(8);
		int intVal = 0;
		double doubleVal = 0;

		fs->Read(intTmp, 0, sizeof(int));//version
		int version = BitConverter::ToInt32(intTmp, 0);

		fs->Read(intTmp, 0, sizeof(int));
		const int objInsNum = BitConverter::ToInt32(intTmp, 0);
		for (int i = 0; i < objInsNum; ++i)
		{
			ObjInsFormat^ data = gcnew ObjInsFormat();
			{
				fs->Read(intTmp, 0, sizeof(int));
				intVal = BitConverter::ToInt32(intTmp, 0);
				array<Byte>^ objInsName = gcnew array<Byte>(intVal);
				fs->Read(objInsName, 0, intVal);
				data->objInsName = Encoding::ASCII->GetString(objInsName);

				fs->Read(intTmp, 0, sizeof(int));
				intVal = BitConverter::ToInt32(intTmp, 0);
				array<Byte>^ drawModelPath = gcnew array<Byte>(intVal);
				fs->Read(drawModelPath, 0, intVal);
				data->drawModelPath = Encoding::ASCII->GetString(drawModelPath);

				fs->Read(intTmp, 0, sizeof(int));
				intVal = BitConverter::ToInt32(intTmp, 0);
				array<Byte>^ hitModelPath = gcnew array<Byte>(intVal);
				fs->Read(hitModelPath, 0, intVal);
				data->hitModelPath = Encoding::ASCII->GetString(hitModelPath);

				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->posX = doubleVal;
				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->posY = doubleVal;
				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->posZ = doubleVal;

				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->angX = doubleVal;
				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->angY = doubleVal;
				fs->Read(doubleTmp, 0, sizeof(double));
				doubleVal = BitConverter::ToDouble(doubleTmp, 0);
				data->angZ = doubleVal;

				fs->Read(intTmp, 0, sizeof(int));
				intVal = BitConverter::ToInt32(intTmp, 0);
				data->objType = intVal;

				fs->Read(intTmp, 0, sizeof(int));
				intVal = BitConverter::ToInt32(intTmp, 0);
				data->fieldObjType = intVal;

			}

			{//オブジェクト生成
				char* drawModelPath = CreateCharArray(data->drawModelPath);
				char* hitModelPath = CreateCharArray(data->hitModelPath);

				data->pFieldObj =
				DsSimu::GetIns()->RegisterObj(drawModelPath, hitModelPath,
					data->posX, data->posY, data->posZ,
					data->angX, data->angY, data->angZ,
					static_cast<DS_MAP_OBJ_TYPE>(data->objType),
					static_cast<DS_MAP_FIELD_OBJ_TYPE>(data->fieldObjType));
				delete drawModelPath;
				delete hitModelPath;
			}

			{//ListViewに追加
				ListViewItem^ item = gcnew ListViewItem();
				item->Text = data->objInsName;
				item->Tag = data;
				ObjInsListView->Items->Add(item);
			}
		}
		fs->Close();
	}
}
private: System::Void 保存ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	SaveFileDialog ^ saveFileDialog = gcnew SaveFileDialog();
	saveFileDialog->Filter = L"独自形式のマップ情報|*.dmap";
	saveFileDialog->Title = L"マップ情報の保存";
	saveFileDialog->ShowDialog();
	if (saveFileDialog->FileName != "")
	{
		System::IO::FileStream ^ fs = safe_cast<System::IO::FileStream^>(saveFileDialog->OpenFile());

		int version = 1;
		fs->Write(BitConverter::GetBytes(version), 0, sizeof(int));

		fs->Write(BitConverter::GetBytes(ObjInsListView->Items->Count), 0, sizeof(int));
		for each(ListViewItem^ item in ObjInsListView->Items)
		{
			ObjInsFormat^ data = static_cast<ObjInsFormat^>(item->Tag);
			//String^ objInsName;
			//String^ drawModelPath;
			//String^ hitModelPath;
			//double posX;
			//double posY;
			//double posZ;
			//double angX;
			//double angY;
			//double angZ;
			//int objType;

			//マウスで移動されているかもしれないので、反映させる
			{
				const DsApp::DsFieldObj* pObj = static_cast<DsApp::DsFieldObj*>(data->pFieldObj);
				const DsVec3d pos = pObj->GetPosition();
				data->posX = pos.x;
				data->posY = pos.y;
				data->posZ = pos.z;
			}


			array<Byte>^ tmpName = Encoding::ASCII->GetBytes(data->objInsName);
			fs->Write(BitConverter::GetBytes(tmpName->Length), 0, sizeof(int));
			fs->Write(tmpName, 0, tmpName->Length*sizeof(char));

			tmpName = Encoding::ASCII->GetBytes(data->drawModelPath);
			fs->Write(BitConverter::GetBytes(tmpName->Length), 0, sizeof(int));
			fs->Write(tmpName, 0, tmpName->Length*sizeof(char));

			tmpName = Encoding::ASCII->GetBytes(data->hitModelPath);
			fs->Write(BitConverter::GetBytes(tmpName->Length), 0, sizeof(int));
			fs->Write(tmpName, 0, tmpName->Length*sizeof(char));

			fs->Write(BitConverter::GetBytes(data->posX), 0, sizeof(double));
			fs->Write(BitConverter::GetBytes(data->posY), 0, sizeof(double));
			fs->Write(BitConverter::GetBytes(data->posZ), 0, sizeof(double));
			fs->Write(BitConverter::GetBytes(data->angX), 0, sizeof(double));
			fs->Write(BitConverter::GetBytes(data->angY), 0, sizeof(double));
			fs->Write(BitConverter::GetBytes(data->angZ), 0, sizeof(double));

			fs->Write(BitConverter::GetBytes(data->objType), 0, sizeof(int));
			fs->Write(BitConverter::GetBytes(data->fieldObjType), 0, sizeof(int));
		}

		fs->Close();
	}
}

};
}
